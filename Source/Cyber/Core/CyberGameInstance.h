// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "..\Interfaces/Persistence.h"
#include "CyberGameInstance.generated.h"

class UCyberSaveGame;
class USaveGame;
class IOnlineSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameSaved);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginCompleted, bool, WasSuccessful);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameLoaded, UCyberSaveGame*, saveGameRef);


/**
 * 
 */
UCLASS()
class CYBER_API UCyberGameInstance : public UGameInstance, public IPersistence
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Login();

protected:

	virtual void Init() override;

	void OnLoginCompleted(int NumberOfPlayers, bool WasSuccessful, const FUniqueNetId& UserId, const FString& Error);

	UPROPERTY(BlueprintAssignable)
	FOnLoginCompleted OnLoginCompletedDelegate;

private:

	IOnlineSubsystem* OnlineSubsystem;
	TSharedPtr<class IOnlineIdentity, ESPMode::ThreadSafe> OnlineIdentity;

public:

	/* Saveable Interface*/

	UFUNCTION(BlueprintCallable)
	void SaveGame(bool Async);

	UPROPERTY(BlueprintAssignable)
	FOnGameSaved OnGameSaved;

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool Async);

	UPROPERTY(BlueprintAssignable)
	FOnGameLoaded OnGameLoaded;

	UFUNCTION(BlueprintCallable)
	void CreateSaveGame();

	void OnAsyncLoadGameFromSlot(const FString& slotName, const int32 userIndex, USaveGame* saveGameRef);

	void OnAsyncSaveGameToSlot(const FString& slotName, const int32 userIndex, bool Success);


protected:

	UPROPERTY(EditAnywhere, Category = "Save Game | Object Reference")
	TObjectPtr<UCyberSaveGame> CyberSaveGame = nullptr;

	const FString SlotName = TEXT("Slot_01");

	const int32 UserIndex = 0;

	/* From Persistence Interface*/

	virtual void RequestSave(bool Async) override;

	TArray<AActor*> SavedActors;

	virtual void RequestLoad(UObject* Requester) override;

};
