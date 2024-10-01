// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberPlayerState.h"
#include "CyberGameState.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\AttributeSets/TokensAttributeSet.h"
#include "..\AttributeSets/CombatAttributeSet.h"
#include "..\AttributeSets/ColorAttributeSet.h"
#include "..\AttributeSets/MovementAttributeSet.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "..\Interfaces/Persistence.h"
#include "CyberGameInstance.h"

ACyberPlayerState::ACyberPlayerState()
{
    GASAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
    GASAbilitySystemComponent->SetIsReplicated(true);
    GASAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

    CyberAttributeSet = CreateDefaultSubobject<UCyberAttributeSet>(TEXT("GAS Attribute Set"));
    TokensAttributeSet = CreateDefaultSubobject<UTokensAttributeSet>(TEXT("Tokens Attribute Set"));
    CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("Combat Attribute Set"));
    ColorAttributeSet = CreateDefaultSubobject<UColorAttributeSet>(TEXT("Color Attribute Set"));
    MovementAttributeSet = CreateDefaultSubobject<UMovementAttributeSet>(TEXT("Movement Attribute Set"));

    // Set the Player State to update at the same frequency as a pawn would.
    NetUpdateFrequency = 100.0f;
}

void ACyberPlayerState::ResetPlayerDeathState(bool isCharacterDead)
{
    bIsCharacterDead = isCharacterDead;
}

void ACyberPlayerState::BeginPlay()
{
    Super::BeginPlay();

    // Bind Game State event.
    if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
    {
        CyberGameState->ApplyEffectOnAllPlayers.AddDynamic(this, &ThisClass::ApplyGameplayEffectPlayerStateFromDelegate);
    }

    if (UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance()))
    {
        CyberGameInstance->OnGameLoaded.AddDynamic(this, &ThisClass::OnGameLoadedEvent);
        CyberGameInstance->OnGameSaved.AddDynamic(this, &ThisClass::OnGameSavedEvent);
    }
}

void ACyberPlayerState::InitializeAttributeSet()
{
    if (GASAbilitySystemComponent)
    {
        GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CyberAttributeSet);
        GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(TokensAttributeSet);
        GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CombatAttributeSet);
        GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(ColorAttributeSet);
        GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(MovementAttributeSet);

        GASAbilitySystemComponent->ForceReplication();

        // Health
        GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CyberAttributeSet->GetHealthAttribute()).AddUObject(this, &ACyberPlayerState::HealthChanged);

        // Enemy Tokens
        GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TokensAttributeSet->GetEnemyTokensAttribute()).AddUObject(this, &ACyberPlayerState::EnemyTokensChanged);

        // Cycle Tokens
        GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(TokensAttributeSet->GetCycleTokensAttribute()).AddUObject(this, &ACyberPlayerState::CycleTokensChanged);
    }
}

UAbilitySystemComponent* ACyberPlayerState::GetAbilitySystemComponent() const
{
    return GASAbilitySystemComponent;
}

TObjectPtr<UCyberAttributeSet> ACyberPlayerState::GetCyberAttributeSet()
{
    return CyberAttributeSet;
}

TObjectPtr<UTokensAttributeSet> ACyberPlayerState::GetTokensAttributeSet()
{
    return TokensAttributeSet;
}

TObjectPtr<UCombatAttributeSet> ACyberPlayerState::GetCombatAttributeSet()
{
    return CombatAttributeSet;
}

TObjectPtr<UColorAttributeSet> ACyberPlayerState::GetColorAttributeSet()
{
    return ColorAttributeSet;
}

TObjectPtr<UMovementAttributeSet> ACyberPlayerState::GetMovementAttributeSet()
{
    return MovementAttributeSet;
}


void ACyberPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
    HealthForBroadcasting = Data.NewValue;

    OnRep_Health();
}

void ACyberPlayerState::OnRep_Health()
{
    OnHealthUpdate.Broadcast(HealthForBroadcasting);

    // Alert Game State Common Health has reached zero value.
    if (HealthForBroadcasting <= 0.0f)
    {
        bIsCharacterDead = true;

        if(HasAuthority())
        {
            CharactersHealthReachedZero();
        }
        else
        {
            ServerRPC_CharactersHealthReachedZero();
        }
    }
    else
    {
        bIsCharacterDead = false;
    }
}

void ACyberPlayerState::CharactersHealthReachedZero()
{
    if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
    {
        CyberGameState->CommonHealthReachedZero();
    }
}

void ACyberPlayerState::ServerRPC_CharactersHealthReachedZero_Implementation()
{
    CharactersHealthReachedZero();
}

void ACyberPlayerState::EnemyTokensChanged(const FOnAttributeChangeData& Data)
{
    EnemyTokensForBroadcasting = Data.NewValue;

    OnRep_EnemyTokens();
}

void ACyberPlayerState::OnRep_EnemyTokens()
{
    OnEnemyTokensUpdate.Broadcast(EnemyTokensForBroadcasting);
}

void ACyberPlayerState::CycleTokensChanged(const FOnAttributeChangeData& Data)
{
    CycleTokensForBroadcasting = Data.NewValue;

    OnRep_CycleTokens();
}

void ACyberPlayerState::OnRep_CycleTokens()
{
    OnCycleTokensUpdate.Broadcast(CycleTokensForBroadcasting);
}

void ACyberPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, bIsCharacterDead);
    DOREPLIFETIME(ThisClass, EnemyTokensForBroadcasting);
    DOREPLIFETIME(ThisClass, CycleTokensForBroadcasting);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HealthForBroadcasting, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CyberPlayerIndex, COND_None, REPNOTIFY_Always);
}

ACyberCharacter* ACyberPlayerState::GetOtherPlayerReference()
{
    return OtherPlayer;
}

void ACyberPlayerState::SetOtherPlayerReference(ACyberCharacter* playerCharacter)
{
    OtherPlayer = playerCharacter;
}

void ACyberPlayerState::SetReadyToRestart(bool IsPlayerReadyToRestart)
{
    if (HasAuthority())
    {
        ReadyToRestart = IsPlayerReadyToRestart;

        OnPlayerReadyToStart.Broadcast();
    }
    else
    {
        ServerRPC_SetReadyToRestart(IsPlayerReadyToRestart);
    }
    
}

void ACyberPlayerState::ServerRPC_SetReadyToRestart_Implementation(bool IsPlayerReadyToRestart)
{
    ReadyToRestart = IsPlayerReadyToRestart;

    OnPlayerReadyToStart.Broadcast();
}

void ACyberPlayerState::SetPlayerUpgradesInServer(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades)
{
    if (HasAuthority())
    {
        PlayerUpgradeAbilitySlotList = PlayerListAbilitesUpgrades;
    }
    else
    {
        ServerRPC_SetPlayerUpgradesInServer(PlayerListAbilitesUpgrades);
    }
}

void ACyberPlayerState::ClientRPC_SetPlayerUpgradesInClient_Implementation(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades)
{
    PlayerUpgradeAbilitySlotList = PlayerListAbilitesUpgrades;
}

void ACyberPlayerState::ServerRPC_SetPlayerUpgradesInServer_Implementation(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades)
{
    PlayerUpgradeAbilitySlotList = PlayerListAbilitesUpgrades;
}

void ACyberPlayerState::ApplyGameplayEffectAllPlayers(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
    if(HasAuthority())
    {
        // Bind Game State event.
        if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
        {
            CyberGameState->ApplyGameplayEffectGameState(effect, SetByCallerTag, Magnitude);
        }
    }
    else
    {
        ServerRPC_ApplyGameplayEffectAllPlayers(effect, SetByCallerTag, Magnitude);
    }
}

void ACyberPlayerState::ServerRPC_ApplyGameplayEffectAllPlayers_Implementation(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{        
    // Bind Game State event.
    if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
    {
        CyberGameState->ApplyGameplayEffectGameState(effect, SetByCallerTag, Magnitude);
    }
}

void ACyberPlayerState::ApplyGameplayEffectPlayerStateFromDelegate(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
    ApplyGameplayEffectPlayerState(effect, SetByCallerTag, Magnitude);
}

bool ACyberPlayerState::ApplyGameplayEffectPlayerState(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
    if(HasAuthority())
    {
        // Create Context Handle from characterHit Ability System Component
        FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
        FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

        // In Spec, using Set By Caller, initialize the values.
        Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

        //Apply Gameplay Effect Spec
        FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());

        return GameplayEffectHandle.WasSuccessfullyApplied();
    }
    else
    {
        ServerRPC_ApplyGameplayEffectPlayerState(effect, SetByCallerTag, Magnitude);

        return false;
    }
}

void ACyberPlayerState::ServerRPC_ApplyGameplayEffectPlayerState_Implementation(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{    
    // Create Context Handle from characterHit Ability System Component
    FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
    FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

    // In Spec, using Set By Caller, initialize the values.
    Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

    //Apply Gameplay Effect Spec
    FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
}


void ACyberPlayerState::ApplyGameplayEffectEnemySpawner(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
    if (HasAuthority())
    {
        if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
        {
            CyberGameState->ApplyGameplayEffectEnemySpawner(effect, SetByCallerTag, Magnitude);
        }
    }
    else
    {
        ServerRPC_ApplyGameplayEffectEnemySpawner(effect, SetByCallerTag, Magnitude);
    }
}

void ACyberPlayerState::ServerRPC_ApplyGameplayEffectEnemySpawner_Implementation(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude)
{
    if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
    {
        CyberGameState->ApplyGameplayEffectEnemySpawner(effect, SetByCallerTag, Magnitude);
    }
}

void ACyberPlayerState::SaveData_Implementation(UCyberSaveGame* saveGameRef)
{
    if (saveGameRef)
    {        
        if (CyberPlayerIndex == 0)
        {
            saveGameRef->Player1Upgrades = PlayerUpgradeAbilitySlotList; // Player 1
        }
        else
        {
            saveGameRef->Player2Upgrades = PlayerUpgradeAbilitySlotList; // Player 2
        }
    }
}

void ACyberPlayerState::LoadData_Implementation(UCyberSaveGame* saveGameRef)
{
    if(saveGameRef)
    {
        if (CyberPlayerIndex == 0)
        {
            PlayerUpgradeAbilitySlotList = saveGameRef->Player1Upgrades; // Player 1
        }
        else 
        {
            PlayerUpgradeAbilitySlotList = saveGameRef->Player2Upgrades; // Player 2

            ClientRPC_SetPlayerUpgradesInClient(PlayerUpgradeAbilitySlotList);
        }
    }
}

void ACyberPlayerState::OnGameLoadedEvent(UCyberSaveGame* saveGameRef)
{	
    // Request Load to Game Instance.
    if (IPersistence* persistence = Cast<IPersistence>(GetGameInstance()))
    {
        persistence->RequestLoad(this);
    }
}

void ACyberPlayerState::OnGameSavedEvent()
{
    // Event triggered when Game is saved.
}
