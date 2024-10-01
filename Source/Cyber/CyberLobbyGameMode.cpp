// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberLobbyGameMode.h"
#include "Core\CyberGameState.h"

void ACyberLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ACyberGameState* CyberGameState = GetGameState<ACyberGameState>())
	{
		if (CyberGameState->PlayerArray.Num() == 2)
		{
			// Start Game!
			FTimerHandle GameModeTimerHandle;

			GetWorld()->GetTimerManager().SetTimer(
				GameModeTimerHandle, // handle to cancel timer at a later time
				this, // the owning object
				&ACyberLobbyGameMode::MovePlayersToMap, // function to call on elapsed
				3.0f, // float delay until elapsed
				false); // looping
		}
	}
}

void ACyberLobbyGameMode::MovePlayersToMap_Implementation()
{
	// Empty (Only in Blueprints)
}
