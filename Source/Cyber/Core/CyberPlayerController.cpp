// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CyberPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CyberGameState.h"
#include "Kismet/GameplayStatics.h"
#include "..\CyberSessionSubsystem.h"
#include "CyberGameInstance.h"
#include "CyberGameMode.h"

ACyberPlayerController::ACyberPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}
void ACyberPlayerController::ApplyStun(bool Apply)
{
	if (Apply)
	{
		SetIgnoreMoveInput(true);
	}
	else
	{
		ResetIgnoreMoveInput();
	}
}

void ACyberPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ACyberCharacter* CyberCharacter = Cast<ACyberCharacter>(GetPawn()))
	{
		CyberCharacter->OnStunApplied.AddDynamic(this, &ThisClass::ApplyStun);
	}
}

void ACyberPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACyberPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void ACyberPlayerController::SetPlayerStateReference(ACyberPlayerState* playerState)
{
	CyberPlayerState = playerState;
}

void ACyberPlayerController::SetUpIndicatorforOtherPlayer()
{
	if(IsLocalController())
	{
		if (IndicatorWidgetClass)
		{
			IndicatorWidget = CreateWidget(this, IndicatorWidgetClass);
			IndicatorWidget->AddToPlayerScreen();
		}
	}
}

void ACyberPlayerController::ShowPreGameTimer()
{
	if (IsLocalController())
	{
		if (PreGameTimerWidgetClass)
		{
			if (!PreGameTimerWidget)
			{
				PreGameTimerWidget = CreateWidget(this, PreGameTimerWidgetClass);
				PreGameTimerWidget->AddToViewport();

				SetIgnoreMoveInput(true);
			}
		}
	}
}

void ACyberPlayerController::RemovePreGameTimer()
{
	if (!IsLocalController())
	{
		return;
	}
	
	if (!PreGameTimerWidget)
	{
		return;
	}
	
	if (PreGameTimerWidget->IsInViewport())
	{
		PreGameTimerWidget->RemoveFromParent();
		PreGameTimerWidget = nullptr;
	}
}

void ACyberPlayerController::ShowPlayerInfo()
{
	if (IsLocalController())
	{
		UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance());

		if (!CyberGameInstance)
		{
			return;
		}

		if (!CyberGameInstance->IsLocalMatch())
		{
			// Is Online Match
			if (OnlinePlayerInfoWidgetClass)
			{
				if (!OnlinePlayerInfoWidget)
				{
					OnlinePlayerInfoWidget = CreateWidget(this, OnlinePlayerInfoWidgetClass);
					OnlinePlayerInfoWidget->AddToPlayerScreen();
				}
			}
		}
		else // Is Local Match
		{
			ACyberGameMode* CyberGameMode = Cast<ACyberGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

			if (!CyberGameMode)
			{
				return;
			}

			TSubclassOf<UUserWidget> LocalPlayerInfoWidgetClass = CyberGameMode->GetLocalPlayerWidgetClass(this);

			// Is Local Match
			if (LocalPlayerInfoWidgetClass)
			{
				if (!LocalPlayerInfoWidget)
				{
					LocalPlayerInfoWidget = CreateWidget(this, LocalPlayerInfoWidgetClass);
					LocalPlayerInfoWidget->AddToPlayerScreen();
				}
			}

			if (CyberGameMode->GetSharedInfoWidgetClass())
			{
				if(GetLocalPlayer()->IsPrimaryPlayer())
				{
					if (!SharedInfoWidget)
					{
						SharedInfoWidget = CreateWidget(this, CyberGameMode->GetSharedInfoWidgetClass());
						SharedInfoWidget->AddToViewport();
					}
				}
			}
		}

		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		ResetIgnoreMoveInput();
	}
}

void ACyberPlayerController::RemovePlayerInfo()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!OnlinePlayerInfoWidget && !SharedInfoWidget)
	{
		return;
	}

	if(OnlinePlayerInfoWidget)
	{
		if (OnlinePlayerInfoWidget->IsInViewport()) // Online Match
		{
			OnlinePlayerInfoWidget->RemoveFromParent();
			OnlinePlayerInfoWidget = nullptr;
		}
	}
	else if(SharedInfoWidget)
	{
		if (SharedInfoWidget->IsInViewport()) // Local Match
		{
			SharedInfoWidget->RemoveFromParent();
			SharedInfoWidget = nullptr;

			if(LocalPlayerInfoWidget)
			{
				LocalPlayerInfoWidget->RemoveFromParent();
				LocalPlayerInfoWidget = nullptr;
			}
		}
	}
}

void ACyberPlayerController::ShowGameOverScreen()
{
	if (IsLocalController())
	{
		if (GameOverWidgetClass)
		{
			if(OnlinePlayerInfoWidget)
			{
				if (OnlinePlayerInfoWidget->IsInViewport()) // Online Match
				{
					OnlinePlayerInfoWidget->RemoveFromParent();
					OnlinePlayerInfoWidget = nullptr;
				}
			}
			else if (LocalPlayerInfoWidget) // Local Match
			{
				// Remove Widget which belongs only to Player
				LocalPlayerInfoWidget->RemoveFromParent();
				LocalPlayerInfoWidget = nullptr;

				if (SharedInfoWidget) // Remove Widget of Shared info between players
				{
					if (SharedInfoWidget->IsInViewport()) 
					{
						SharedInfoWidget->RemoveFromParent();
						SharedInfoWidget = nullptr;
					}
				}
			}

			// Add to Player Screen the Game Over widget
			if(!GameOverWidget)
			{
				GameOverWidget = CreateWidget(this, GameOverWidgetClass);
				GameOverWidget->AddToPlayerScreen();
			}

			SetInputMode(FInputModeUIOnly());
			SetShowMouseCursor(true);
		}
	}
}

void ACyberPlayerController::RemoveGameOverWidget()
{
	if (IsLocalController())
	{
		if (GameOverWidget->IsInViewport())
		{
			GameOverWidget->RemoveFromParent();
		}

		GameOverWidget = nullptr;
	}
}

void ACyberPlayerController::DeactivateInput()
{
	if (IsLocalController())
	{
		SetInputMode(FInputModeUIOnly());
		SetIgnoreMoveInput(true);
	}
}