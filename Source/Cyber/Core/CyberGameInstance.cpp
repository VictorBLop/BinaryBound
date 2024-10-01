// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberGameInstance.h"
#include "..\CyberSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "..\Interfaces/Saveable.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"

void UCyberGameInstance::CreateSaveGame()
{
	CyberSaveGame = Cast<UCyberSaveGame>(UGameplayStatics::CreateSaveGameObject(UCyberSaveGame::StaticClass()));
}

void UCyberGameInstance::Login()
{
	if (OnlineIdentity)
	{
		if(!UKismetSystemLibrary::IsLoggedIn(GetFirstLocalPlayerController(GetWorld())))
		{
			FOnlineAccountCredentials OnlineAccountCredentials;

			OnlineAccountCredentials.Type = "accountportal";
			OnlineAccountCredentials.Id = "";
			OnlineAccountCredentials.Token = "";

			OnlineIdentity->Login(0, OnlineAccountCredentials); // LocalUserNum will always be 0 unless it is local multiplayer/splitscreen.
		}
		else
		{
			OnLoginCompletedDelegate.Broadcast(true);
		}
	}
}

void UCyberGameInstance::Init()
{
	Super::Init();

	OnlineSubsystem = IOnlineSubsystem::Get();

	OnlineIdentity = OnlineSubsystem->GetIdentityInterface();
	OnlineIdentity->OnLoginCompleteDelegates->AddUObject(this, &UCyberGameInstance::OnLoginCompleted);
}

void UCyberGameInstance::OnLoginCompleted(int NumberOfPlayers, bool WasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (WasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Logged in!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Login failed! : %s "), *Error);
	}

	OnLoginCompletedDelegate.Broadcast(WasSuccessful);
}

void UCyberGameInstance::SaveGame(bool Async)
{
	if (Async)
	{
		UGameplayStatics::AsyncSaveGameToSlot(CyberSaveGame, SlotName, UserIndex, FAsyncSaveGameToSlotDelegate::CreateUObject(this, &ThisClass::OnAsyncSaveGameToSlot));
	}
	else
	{
		if (UGameplayStatics::SaveGameToSlot(CyberSaveGame, SlotName, UserIndex))
		{
			OnGameSaved.Broadcast();
		}
	}
}

void UCyberGameInstance::OnAsyncSaveGameToSlot(const FString& slotName, const int32 userIndex, bool Success)
{
	if (Success)
	{
		// Game was saved correctly
		OnGameSaved.Broadcast();
	}
}

void UCyberGameInstance::LoadGame(bool Async)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		if (Async)
		{
			UGameplayStatics::AsyncLoadGameFromSlot(SlotName, UserIndex, FAsyncLoadGameFromSlotDelegate::CreateUObject(this, &ThisClass::OnAsyncLoadGameFromSlot));
		}
		else
		{
			if (UCyberSaveGame* saveGameReference = Cast<UCyberSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)))
			{
				CyberSaveGame = saveGameReference;

				OnGameLoaded.Broadcast(CyberSaveGame);
			}
		}
	}
	else
	{
		CreateSaveGame();
	}
}

void UCyberGameInstance::OnAsyncLoadGameFromSlot(const FString& slotName, const int32 userIndex, USaveGame* saveGameRef)
{
	if (UCyberSaveGame* saveGameReference = Cast<UCyberSaveGame>(saveGameRef))
	{
		CyberSaveGame = saveGameReference;

		OnGameLoaded.Broadcast(CyberSaveGame);
	}
}

void UCyberGameInstance::RequestSave(bool Async)
{
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveable::StaticClass(), SavedActors);

	// Define a TArray of UObject pointers to store the casted objects
	TArray<UObject*> SaveableObjects;

	// Iterate through the found actors
	for (AActor* Actor : SavedActors)
	{
		// Cast each actor to UObject and add it to the SaveableObjects array
		UObject* SaveableObject = Cast<UObject>(Actor);
		if (SaveableObject)
		{
			SaveableObjects.Add(SaveableObject);
		}
	}

	// Save All Data in Save Game.
	CyberSaveGame->SaveAllData(SaveableObjects);

	// Save Game.
	SaveGame(Async);
}

void UCyberGameInstance::RequestLoad(UObject* Requester)
{
	CyberSaveGame->LoadDataForRequester(Requester);
}
