// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "..\CyberStructs.h"
#include "CyberLocalMultiplayerGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CYBER_API ACyberLocalMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	ACyberLocalMultiplayerGameMode();

protected:

	UPROPERTY(EditAnywhere, Category = "Local Multiplayer")
	FLocalMultiplayerOptions LocalMultiplayerOptions;

	TArray<ULocalPlayer*> LocalPlayers;

	virtual void BeginPlay() override;
};
