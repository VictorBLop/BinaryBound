// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CyberLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CYBER_API ACyberLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Maps")
	void MovePlayersToMap();

	virtual void MovePlayersToMap_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Maps")
	FString MapName = TEXT("");
};
