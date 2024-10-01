// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberGameState.h"
#include "CyberPlayerState.h"
#include "CyberPlayerController.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\AttributeSets/TokensAttributeSet.h"
#include "..\Cyber.h"
#include "CyberGameInstance.h"
#include "..\CyberSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "..\Enemy\CyberEnemySpawner.h"
#include "CyberGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "..\Interfaces/Persistence.h"

ACyberGameState::ACyberGameState()
{
	//PrimaryActorTick.bCanEverTick = false;
	BackgroundMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BackgroundMusic"));
}

void ACyberGameState::BeginPlay()
{
	Super::BeginPlay();

	CyberEnemySpawner = Cast<ACyberEnemySpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ACyberEnemySpawner::StaticClass()));

	if (HasAuthority() && CyberEnemySpawner)
	{
		CyberEnemySpawner->OnWaveCompleted.AddDynamic(this, &ThisClass::OnWaveCompleted);
		CyberEnemySpawner->OnGameWin.AddDynamic(this, &ThisClass::OnGameWin);
	}

	if (UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance()))
	{
		CyberGameInstance->OnGameLoaded.AddDynamic(this, &ThisClass::OnGameLoadedEvent);
		CyberGameInstance->OnGameSaved.AddDynamic(this, &ThisClass::OnGameSavedEvent);
	}
}

void ACyberGameState::TriggerPreGameStart()
{
	// Start timer only on server
	if (HasAuthority())
	{
		// Start Pre Game Timer and One Second Timer
		if (PreGameDuration > 0.0f)
		{
			// Start Pre Game Timer
			GetWorld()->GetTimerManager().SetTimer(
				PreGameTimer, // handle to cancel timer at a later time
				this, // the owning object
				&ACyberGameState::OnPreGameTimerEnd, // function to call on elapsed
				PreGameDuration, // float delay until elapsed
				false); // looping

			PreGameTimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(PreGameTimer);

			// Start One Second Timer
			GetWorld()->GetTimerManager().SetTimer(
				OneSecondTimer, // handle to cancel timer at a later time
				this, // the owning object
				&ACyberGameState::OneSecondPreGamePassedBy, // function to call on elapsed
				1.0f, // float delay until elapsed
				true); // looping
		}

		LoadGameForPlayers();

		// Increase Attempts	
		PlayerStats.Attempts = PlayerStats.Attempts + 1;
	}
}

void ACyberGameState::TriggerGameStart()
{
	// Play Music
	bPlayBackgroundMusic = !bPlayBackgroundMusic;
	OnRep_PlayBackgroundMusic();

	HasDeathEffectsApplied = false;

	// Start timer only on server
	if (HasAuthority())
	{
		ACyberGameMode* CyberGameMode = Cast<ACyberGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		if (CyberGameMode)
		{
			CyberGameMode->ActivateEnemySpawner();
		}

		// Start Game Timer and One Second Timer
		if (GameDuration > 0.0f)
		{
			// Start Game Timer
			GetWorld()->GetTimerManager().SetTimer(
				GameTimer, // handle to cancel timer at a later time
				this, // the owning object
				&ACyberGameState::OnGameTimerEnd, // function to call on elapsed
				GameDuration, // float delay until elapsed
				false); // looping

			TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(GameTimer);

			// Start One Second Timer
			GetWorld()->GetTimerManager().SetTimer(
				OneSecondTimer, // handle to cancel timer at a later time
				this, // the owning object
				&ACyberGameState::OneSecondPassedBy, // function to call on elapsed
				1.0f, // float delay until elapsed
				true); // looping
		}
	}
}

void ACyberGameState::OnRep_PreGameTimerRemaining()
{
	if (PreGameTimeRemaining > 0.0f)
	{
		OnGameTimerUpdate.Broadcast(0, (int32)PreGameTimeRemaining);
	}
}

void ACyberGameState::OnPreGameTimerEnd()
{
	// Clear Timer
	GetWorld()->GetTimerManager().ClearTimer(PreGameTimer);
	GetWorld()->GetTimerManager().ClearTimer(OneSecondTimer);

	// Remove Widget from Screen
	bHasPreGameTimerEnded = !bHasPreGameTimerEnded;

	OnRep_HasPreGameTimerEnded();

	// Trigger Game Start.
	TriggerGameStart();
}

void ACyberGameState::OneSecondPreGamePassedBy()
{
	PreGameTimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(PreGameTimer);

	OnRep_PreGameTimerRemaining();
}

void ACyberGameState::OnRep_HasPreGameTimerEnded()
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			if (ACyberPlayerController* PlayerController = Cast<ACyberPlayerController>(PlayerState->GetPlayerController()))
			{
				PlayerController->RemovePreGameTimer();

				PlayerController->ShowPlayerInfo();

				PlayerController->SetUpIndicatorforOtherPlayer();
			}
		}
	}
}

void ACyberGameState::OneSecondPassedBy()
{
	TimeRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(GameTimer);

	OnRep_TimerRemaining();
}

void ACyberGameState::OnRep_TimerRemaining()
{
	if (TimeRemaining > 0.0f)
	{
		int32 MinutesRemaining;
		int32 SecondsRemaining;

		MinutesRemaining = (int32)TimeRemaining / 60;
		SecondsRemaining = (int32)TimeRemaining % 60;

		OnGameTimerUpdate.Broadcast(MinutesRemaining, SecondsRemaining);
	}
}

void ACyberGameState::OnGameTimerEnd()
{
	// This is only called on the server.
	GetWorld()->GetTimerManager().ClearTimer(GameTimer);
	GetWorld()->GetTimerManager().ClearTimer(OneSecondTimer);

	bHasGameTimerEnded = !bHasGameTimerEnded;
	OnRep_HasGameTimerEnded();
}

void ACyberGameState::OnRep_HasGameTimerEnded()
{
	GameEnd(false, true);
}

void ACyberGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TimeRemaining, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, PreGameTimeRemaining, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bHasPreGameTimerEnded, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bHasGameTimerEnded, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, RestartBothPlayers, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bHealthReachedZero, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bAddStartWidget, COND_None, REPNOTIFY_Always);	
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bReferenceOtherPlayer, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bGameWonRep, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bPlayBackgroundMusic, COND_None, REPNOTIFY_Always);
}

void ACyberGameState::GameEnd(bool Win, bool CycleCompleted)
{
	bCycleCompleted = CycleCompleted;

	bGameWin = Win;

	OnGameEndAllPlayers.Broadcast(); // To alert server and clients of game ending.

	StopBackgroundMusic();

	if(HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(GameTimer);
		GetWorld()->GetTimerManager().ClearTimer(OneSecondTimer);

		if (CycleCompleted)
		{
			SetCycleTokens();
		}

		OnGameOver.Broadcast(); // Alert Enemy Spawner game has ended.

		TArray<TSubclassOf<UGameplayEffect>> GameEndEffects;

		if (Win)
		{
			GameEndEffects = GameWinEffects;

			if (HasAuthority())
			{
				Client_SetPlayerStats(PlayerStats);
			}
		}
		else
		{
			GameEndEffects = DeathEffects;
		}

		if(!HasDeathEffectsApplied)
		{
			for (TSubclassOf<UGameplayEffect> GameEndEffect : GameEndEffects)
			{
				ApplyEffectOnAllPlayers.Broadcast(GameEndEffect, GameplayEffect_Basic, 1.0f);
			}

			HasDeathEffectsApplied = true;
		}

		SaveGameForPlayers();
	}

	// Deactivate Input on Game Over
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			if (ACyberPlayerController* PlayerController = Cast<ACyberPlayerController>(PlayerState->GetPlayerController()))
			{
				PlayerController->DeactivateInput();
			}

			if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerState))
			{
				ASI->GetAbilitySystemComponent()->CancelAllAbilities();
			}
		}
	}

	// Start Pre Game Timer
	GetWorld()->GetTimerManager().SetTimer(
		GameOverTimer, // handle to cancel timer at a later time
		this, // the owning object
		&ACyberGameState::CallGameOverAfterTimer, // function to call on elapsed
		3.0f, // float delay until elapsed
		false); // looping
}

void ACyberGameState::CallGameOverAfterTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(GameOverTimer);

	// Show Power-Ups Menu - Game Over
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			if (ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(PlayerState))
			{
				CyberPlayerState->ResetPlayerDeathState(false);
			}

			if (ACyberPlayerController* PlayerController = Cast<ACyberPlayerController>(PlayerState->GetPlayerController()))
			{
				PlayerController->ShowGameOverScreen();
			}
		}
	}

	bCycleCompleted = false;

	bGameWin = false;
}

void ACyberGameState::SetCycleTokens()
{
	// Add Enemy tokens to each one of the players.
	for (APlayerState* PlayerState : PlayerArray)
	{
		ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(PlayerState);

		if (CyberPlayerState && ModifyCycleTokens)
		{
			// Create Context Handle from characterHit Ability System Component
			FGameplayEffectContextHandle effectContext = CyberPlayerState->GetAbilitySystemComponent()->MakeEffectContext();
			FGameplayEffectSpec* Spec = new FGameplayEffectSpec(ModifyCycleTokens.GetDefaultObject(), effectContext, 1.0f);

			// In Spec, using Set By Caller, initialize the values.
			Spec->SetSetByCallerMagnitude(GameplayEffect_TokensValue_Cycle, CyberPlayerState->GetTokensAttributeSet()->GetTokensPerCycle());

			//Apply Gameplay Effect Spec
			FActiveGameplayEffectHandle GameplayEffectHandle = CyberPlayerState->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
		}
	}
}

void ACyberGameState::RestartCyberGame()
{
	// Move Players to PlayerStart Position.
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			if (ACyberCharacter* CyberCharacter = Cast<ACyberCharacter>(PlayerState->GetPawn()))
			{
				CyberCharacter->MoveToStartLocation();
			}
		}
	}

	// Save Game before Loading.
	if (HasAuthority())
	{
		SaveGameForPlayers();
	}

	RestartBothPlayers = !RestartBothPlayers;

	OnRep_RestartBothPlayers();
}

void ACyberGameState::OnRep_RestartBothPlayers()
{
	OnRestartGame.Broadcast();

	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState)
		{
			if (ACyberPlayerController* PlayerController = Cast<ACyberPlayerController>(PlayerState->GetPlayerController()))
			{
				PlayerController->RemoveGameOverWidget();

				PlayerController->ShowPreGameTimer();
			}
		}
	}

	PlayersRestarted = 0;

	TriggerPreGameStart();
	
}

void ACyberGameState::PlayerPressedRestart()
{
	PlayersRestarted++;

	if (PlayersRestarted == 2)
	{
		RestartCyberGame();
	}
}

void ACyberGameState::LoadGameForPlayers()
{	
	// Create Save Game and Load Game on Pre Game Timer Start.
	if (GetDefaultGameMode())
	{
		if (UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance()))
		{
			CyberGameInstanceRef = CyberGameInstance;

			CyberGameInstanceRef->LoadGame(false);
		}
	}
}

void ACyberGameState::SaveGameForPlayers()
{		
	// Save Game on Game Over (will be moved to another place)
	if (IPersistence* PersistenceInterface = Cast<IPersistence>(CyberGameInstanceRef))
	{
		PersistenceInterface->RequestSave(false);
	}
}

void ACyberGameState::SaveData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (saveGameRef)
	{
		saveGameRef->PlayerStats = PlayerStats;
	}
}

void ACyberGameState::LoadData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (saveGameRef)
	{
		PlayerStats = saveGameRef->PlayerStats;
	}
}

void ACyberGameState::OnGameLoadedEvent(UCyberSaveGame* saveGameRef)
{
	// Request Load to Game Instance.
	if (IPersistence* persistence = Cast<IPersistence>(GetGameInstance()))
	{
		persistence->RequestLoad(this);
	}
}

void ACyberGameState::OnGameSavedEvent()
{
	// Event triggered when Game is saved.
}

void ACyberGameState::Server_SetPlayerStats_Implementation()
{
	Client_SetPlayerStats(PlayerStats);
}

void ACyberGameState::Client_SetPlayerStats_Implementation(FPlayerEndStats ServerPlayerStats)
{
	PlayerStats = ServerPlayerStats;

	UE_LOG(LogTemp, Warning, TEXT("(CLIENT) Attempts: %i, Enemies: %i, Waves: %i"), PlayerStats.Attempts, PlayerStats.EnemiesDestroyed, PlayerStats.WavesCompleted);
}

void ACyberGameState::OnGameWin()
{
	bGameWonRep = !bGameWonRep;

	OnRep_GameWonRep();
}

void ACyberGameState::OnRep_GameWonRep()
{
	GameEnd(true, true);
}

void ACyberGameState::CommonHealthReachedZero()
{
	bHealthReachedZero = !bHealthReachedZero;

	OnRep_HealthReachedZero();
}

void ACyberGameState::OnRep_HealthReachedZero()
{
	GameEnd(false, false);
}

bool ACyberGameState::HasCycleBeenCompleted()
{
	return bCycleCompleted;
}


void ACyberGameState::ApplyGameplayEffectGameState(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
	if(effect == DealDamageEffect)
	{
		if (!PlayersHaveShield())
		{
			ApplyEffectOnAllPlayers.Broadcast(effect, SetByCallerTag, Magnitude);
		}
	}
	else
	{
		ApplyEffectOnAllPlayers.Broadcast(effect, SetByCallerTag, Magnitude);
	}
}

void ACyberGameState::ApplyGameplayEffectEnemySpawner(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
	if (CyberEnemySpawner)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = CyberEnemySpawner->GetAbilitySystemComponent()->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = CyberEnemySpawner->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
	}
}

void ACyberGameState::OnRep_AddStartWidget()
{
	for (int32 index = 0; index < PlayerArray.Num(); index++)
	{
		ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(PlayerArray[index]);

		if (CyberPlayerState)
		{
			if (ACyberPlayerController* PlayerController = Cast<ACyberPlayerController>(CyberPlayerState->GetPlayerController()))
			{
				PlayerController->ShowPreGameTimer();
			}
		}
	}
}

void ACyberGameState::OnWaveCompleted()
{
	// Add to wave count.
	PlayerStats.WavesCompleted = PlayerStats.WavesCompleted + 1;


	SetCycleTokens();
}

void ACyberGameState::OnRep_PlayBackgroundMusic()
{
	if (BackgroundMusic)
	{
		if(BackgroundMusicComponent)
		{
			BackgroundMusicComponent->Play();
		}
	}
}

void ACyberGameState::StopBackgroundMusic()
{
	if (BackgroundMusicComponent->IsValidLowLevel())
	{
		BackgroundMusicComponent->Stop();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Stop music!"));
	}
}


void ACyberGameState::PlayerJoined()
{
	if (PlayerArray.Num() == 2)
	{
		// Reference other players.
		bReferenceOtherPlayer = !bReferenceOtherPlayer;
		OnRep_ReferenceOtherPlayer();

		// Add Start Widget.
		bAddStartWidget = !bAddStartWidget;
		OnRep_AddStartWidget();

		// Create PlayerIndex and Bind OnPlayerReadyToStart delegate.
		for (int32 index = 0; index < PlayerArray.Num(); index++)
		{
			if(ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(PlayerArray[index]))
			{
				CyberPlayerState->OnPlayerReadyToStart.AddDynamic(this, &ThisClass::PlayerPressedRestart);

				CyberPlayerState->CyberPlayerIndex = index;
			}
		}

		// Trigger Pre Game Timer Start
		TriggerPreGameStart();
		
	}
}

void ACyberGameState::SetEnemyTokens()
{
	// Add to enemy count
	PlayerStats.EnemiesDestroyed = PlayerStats.EnemiesDestroyed + 1;

	// Add Enemy tokens to each one of the players.
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(PlayerState))
		{
			// Create Context Handle from characterHit Ability System Component
			FGameplayEffectContextHandle effectContext = CyberPlayerState->GetAbilitySystemComponent()->MakeEffectContext();
			FGameplayEffectSpec* Spec = new FGameplayEffectSpec(ModifyEnemyTokens.GetDefaultObject(), effectContext, 1.0f);

			// In Spec, using Set By Caller, initialize the values.
			Spec->SetSetByCallerMagnitude(GameplayEffect_TokensValue_Enemy, CyberPlayerState->GetTokensAttributeSet()->GetTokensPerEnemy());

			//Apply Gameplay Effect Spec
			FActiveGameplayEffectHandle GameplayEffectHandle = CyberPlayerState->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());	
		}
	}
}

void ACyberGameState::OnRep_ReferenceOtherPlayer()
{
	ReferenceOtherPlayer();
}

void ACyberGameState::ReferenceOtherPlayer()
{
	if (PlayerArray.IsValidIndex(0))
	{
		ACyberCharacter* CharacterOne = Cast<ACyberCharacter>(PlayerArray[0]->GetPawn());

		if (PlayerArray.IsValidIndex(1))
		{
			ACyberCharacter* CharacterTwo = Cast<ACyberCharacter>(PlayerArray[1]->GetPawn());

			if (CharacterOne && CharacterTwo)
			{
				Cast<ACyberPlayerState>(PlayerArray[0])->SetOtherPlayerReference(CharacterTwo);
				Cast<ACyberPlayerState>(PlayerArray[1])->SetOtherPlayerReference(CharacterOne);
			}
		}
	}
}

bool ACyberGameState::PlayersHaveShield()
{
	bool PlayersHaveShield = false;

	for (APlayerState* PlayerState : PlayerArray)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PlayerState))
		{
			if (ASI->GetAbilitySystemComponent()->GetTagCount(GameplayEffect_Shield_Hit) > 0)
			{
				PlayersHaveShield = true;
			}
		}
	}
	
	return PlayersHaveShield;
}


