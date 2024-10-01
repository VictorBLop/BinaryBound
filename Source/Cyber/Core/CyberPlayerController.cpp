// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberPlayerController.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CyberPlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "CyberGameState.h"
#include "Kismet/GameplayStatics.h"

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
				PreGameTimerWidget->AddToPlayerScreen();

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
		if (PlayerInfoWidgetClass)
		{
			if(!PlayerInfoWidget)
			{
				PlayerInfoWidget = CreateWidget(this, PlayerInfoWidgetClass);
				PlayerInfoWidget->AddToPlayerScreen();
			}

			SetInputMode(FInputModeGameOnly());
			SetShowMouseCursor(false);
			
			ResetIgnoreMoveInput();
		}
	}
}

void ACyberPlayerController::RemovePlayerInfo()
{
	if (!IsLocalController())
	{
		return;
	}

	if (!PlayerInfoWidget)
	{
		return;
	}

	if (PlayerInfoWidget->IsInViewport())
	{
		PlayerInfoWidget->RemoveFromParent();
		PlayerInfoWidget = nullptr;
	}

}

void ACyberPlayerController::ShowGameOverScreen()
{
	if (IsLocalController())
	{
		if (GameOverWidgetClass && PlayerInfoWidget)
		{
			if (PlayerInfoWidget->IsInViewport())
			{
				PlayerInfoWidget->RemoveFromParent();
				PlayerInfoWidget = nullptr;
			}

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
