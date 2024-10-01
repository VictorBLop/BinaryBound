// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Subsystems/GameInstanceSubsystem.h>
#include <OnlineSessionSettings.h>
#include <Interfaces/OnlineSessionInterface.h>
#include "CyberSessionSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCyberSessionSettings
{
	GENERATED_BODY()

	/* This structure is used to store information
	for the Session Settings. It includes:
	- Max number of players for the session
	- bLan: if session is LAN or not.
	- Name of the level
	- IsValid() function to check if there are more
	than 2 players. Otherwise it will not be valid.
	*/

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadWrite)
	bool bLan = true;

	UPROPERTY(BlueprintReadWrite)
	FName LevelName;

	bool IsValid() const;
};

// This delegate is created to be used when a session is created successfully or not.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionDelegate, bool, Successful);

USTRUCT(BlueprintType)
struct FCyberSessionResult
{
	GENERATED_BODY()

	FOnlineSessionSearchResult SessionResult;
};

// This delegate is created to be used when finding sessions, with the Session results as output.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsFoundDelegate, const TArray<FCyberSessionResult>&, SessionResults);

// This delegate is created to be used when joining sessions successfully, to return the Session Result.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSessionsJoinedSuccessfullyDelegate, const FCyberSessionResult&, SessionResult);


UCLASS()
class CYBER_API UCyberSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	/* Create Local Match */
	UFUNCTION(BlueprintCallable)
	void CreateLocalMatch(FName LevelName);

	UPROPERTY(BlueprintReadOnly)
	bool bIsLocalMatch = false;

	/* Create Session*/
	UFUNCTION(BlueprintCallable)
	void CreateSession(const FCyberSessionSettings SessionSettings);
	void OnSessionCreated(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnCreateSessionCompleted;

	/* Start Session */
	UFUNCTION(BlueprintCallable)
	void StartSession();
	void OnSessionStarted(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnStartSessionCompleted;

	/* Update Session */
	UFUNCTION(BlueprintCallable)
	void UpdateSession(const FCyberSessionSettings SessionSettings);
	void OnSessionUpdated(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnUpdateSessionCompleted;

	/* Find Sessions */
	UFUNCTION(BlueprintCallable)
	void FindSessions(int32 MaxSearchResults, bool bLan);
	void OnFindSessions(bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionsFoundDelegate OnSessionsFound;

	UFUNCTION(BlueprintPure)
	bool IsValid(const FCyberSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetPingInMs(const FCyberSessionResult& Session);

	UFUNCTION(BlueprintPure)
	FString GetHostName(const FCyberSessionResult& Session);

	UFUNCTION(BlueprintPure)
	FString GetMapName(const FCyberSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetCurrentPlayers(const FCyberSessionResult& Session);

	UFUNCTION(BlueprintPure)
	int32 GetMaxPlayers(const FCyberSessionResult& Session);

	/* Join Session*/
	UFUNCTION(BlueprintCallable)
	void JoinSession(const FCyberSessionResult& Session);
	void OnSessionJoined(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnJoinSessionCompleted;
	UPROPERTY(BlueprintAssignable)
	FOnSessionsJoinedSuccessfullyDelegate OnSessionsJoinedSuccessfully;

	/* Destroy Session*/
	UFUNCTION(BlueprintCallable)
	void DestroySession();
	void OnSessionDestroyed(FName SessionName, bool bSuccessful);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnDestroySessionCompleted;

	/* Participants Joined */
	void ParticipantsJoined(FName SessionName, const FUniqueNetId& Id);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnParticipantsJoined;

	/* Participants Left */
	void ParticipantsLeft(FName SessionName, const FUniqueNetId& Id, EOnSessionParticipantLeftReason ReasonToLeave);
	UPROPERTY(BlueprintAssignable)
	FOnSessionDelegate OnParticipantsLeft;


private:
	/* Create Session*/
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	FDelegateHandle OnCreateSessionCompleteHandle;

	/* Start Session*/
	FDelegateHandle OnStartSessionCompleteHandle;
	FDelegateHandle OnUpdateSessionCompleteHandle;

	/* Find Sessions*/
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	FDelegateHandle OnFindSessionsCompleteHandle;

	/* Join Sessions*/
	FDelegateHandle OnJoinSessionsCompleteHandle;

	/* Destroy Session*/
	FDelegateHandle OnDestroySessionCompleteHandle;

	/* Participants Joined*/
	FDelegateHandle OnSessionParticipantJoinedDelegate;
	
	/* Participants Left*/
	FDelegateHandle OnSessionParticipantLeftDelegate;
};
