// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberLocalMultiplayerGameMode.h"
#include "Kismet/GameplayStatics.h"

ACyberLocalMultiplayerGameMode::ACyberLocalMultiplayerGameMode()
{
	LocalMultiplayerOptions.LocalMultiplayer = MultiplayerMode::TwoPlayers;
	LocalMultiplayerOptions.NumberOfPlayers = 2;
}

void ACyberLocalMultiplayerGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (int32 PlayerNumber = 0; PlayerNumber < LocalMultiplayerOptions.NumberOfPlayers; PlayerNumber++)
	{
		
		// Create Player and assign controllerID = NumberOfPlayer

		FString OutError = TEXT("");

		//LocalPlayers.Add(GetGameInstance()->CreateLocalPlayer(PlayerNumber, OutError, true));

		//UGameplayStatics::CreatePlayer(GetWorld(), PlayerNumber, true);
	}
}
