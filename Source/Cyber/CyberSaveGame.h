// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Interfaces/Saveable.h"
#include "CyberStructs.h"
#include "CyberSaveGame.generated.h"


/**
 * 
 */
UCLASS()
class CYBER_API UCyberSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	void SaveAllData(TArray<UObject*> SaveableObjects);

	void LoadDataForRequester(UObject* Requester);

	// Data to be saved

	UPROPERTY(VisibleAnywhere)
	TArray<FPlayerSaveData> PlayersSaveData;

	UPROPERTY(VisibleAnywhere)
	FPlayerSpecialAbilitiesInfo Player1SpecialAbilitiesInfo;

	UPROPERTY(VisibleAnywhere)
	FPlayerSpecialAbilitiesInfo Player2SpecialAbilitiesInfo;

	UPROPERTY(VisibleAnywhere)
	FEnemySaveData MeleeEnemiesSaveData;

	UPROPERTY(VisibleAnywhere)
	FEnemySaveData RangedEnemiesSaveData;

	UPROPERTY(BlueprintReadwrite)
	TArray<FUpgradeAbilitySlotListEntry> Player1Upgrades;

	UPROPERTY(BlueprintReadwrite)
	TArray<FUpgradeAbilitySlotListEntry> Player2Upgrades;

	UPROPERTY(BlueprintReadwrite)
	FPlayerEndStats PlayerStats;
};
