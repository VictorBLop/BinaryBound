// Fill out your copyright notice in the Description page of Project Settings.

#include "CyberSessionSubsystem.h"
#include <OnlineSubsystemUtils.h>
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CyberSessionSubsystem)

bool FCyberSessionSettings::IsValid() const
{
	if (MaxPlayers < 2)
	{
		return false;
	}

	return true;
}

void UCyberSessionSubsystem::CreateLocalMatch(FName LevelName)
{
	bIsLocalMatch = true;
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

void UCyberSessionSubsystem::CreateSession(const FCyberSessionSettings SessionSettings)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid() || !SessionSettings.IsValid())
	{
		OnCreateSessionCompleted.Broadcast(false);
		return;
	}

	LastSessionSettings = MakeShareable<FOnlineSessionSettings>(new FOnlineSessionSettings());
	LastSessionSettings->NumPublicConnections = SessionSettings.MaxPlayers;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bIsLANMatch = SessionSettings.bLan;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bAllowInvites = true;
		
	LastSessionSettings->Set(FName(TEXT("SETTING_MAPNAME")), SessionSettings.LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	OnCreateSessionCompleteHandle = sessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionCreated));

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	const bool bSessionCreated = sessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings);

	if (!bSessionCreated)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);

		OnCreateSessionCompleted.Broadcast(false);
	}
}

void UCyberSessionSubsystem::OnSessionCreated(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteHandle);
	}

	OnCreateSessionCompleted.Broadcast(bSuccessful);

	if(bSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Created session with name: %s"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create a session with name: %s"), *SessionName.ToString());
	}
}

void UCyberSessionSubsystem::UpdateSession(const FCyberSessionSettings SessionSettings)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid() || !SessionSettings.IsValid())
	{
		OnUpdateSessionCompleted.Broadcast(false);
		return;
	}

	TSharedPtr<FOnlineSessionSettings> updatedSessionSettings = MakeShareable<FOnlineSessionSettings>(new FOnlineSessionSettings(*LastSessionSettings));
	updatedSessionSettings->NumPublicConnections = SessionSettings.MaxPlayers;
	updatedSessionSettings->bIsLANMatch = SessionSettings.bLan;
	updatedSessionSettings->Set(FName(TEXT("SETTING_MAPNAME")), SessionSettings.LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

	OnUpdateSessionCompleteHandle = sessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionUpdated));

	if (sessionInterface->UpdateSession(NAME_GameSession, *updatedSessionSettings))
	{
		LastSessionSettings = updatedSessionSettings;
	}
	else
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteHandle);
		OnUpdateSessionCompleted.Broadcast(false);
	}
}

void UCyberSessionSubsystem::OnSessionUpdated(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteHandle);
	}

	OnUpdateSessionCompleted.Broadcast(bSuccessful);
}

void UCyberSessionSubsystem::StartSession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnStartSessionCompleted.Broadcast(false);
		return;
	}

	OnStartSessionCompleteHandle = sessionInterface->AddOnStartSessionCompleteDelegate_Handle(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionStarted));

	if (!sessionInterface->StartSession(NAME_GameSession))
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
		OnStartSessionCompleted.Broadcast(false);
	}
}

void UCyberSessionSubsystem::OnSessionStarted(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteHandle);
	}

	OnStartSessionCompleted.Broadcast(bSuccessful);

	FString levelName;
	LastSessionSettings.Get()->Get(FName(TEXT("SETTING_MAPNAME")), levelName);

	GetWorld()->ServerTravel(levelName + TEXT("?listen"), true);
}

void UCyberSessionSubsystem::FindSessions(int32 MaxSearchResults, bool bLan)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface is not valid."));
		OnSessionsFound.Broadcast(TArray<FCyberSessionResult>());
		return;
	}

	OnFindSessionsCompleteHandle = sessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessions));

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = bLan;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Error, TEXT("Searching for sessions has failed."));

		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);

		OnSessionsFound.Broadcast(TArray<FCyberSessionResult>());
	}
}

void UCyberSessionSubsystem::OnFindSessions(bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteHandle);
	}

	if (!bSuccessful || LastSessionSearch->SearchResults.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Find sessions has not been successful."));

		OnSessionsFound.Broadcast(TArray<FCyberSessionResult>());
		return;
	}

	TArray<FCyberSessionResult> Sessions;

	for (FOnlineSessionSearchResult& searchResult : LastSessionSearch->SearchResults)
	{
		FCyberSessionResult session;
		session.SessionResult = searchResult;
		Sessions.Add(session);

		UE_LOG(LogTemp, Warning, TEXT("Found session with id: %s"), *searchResult.GetSessionIdStr());
	}

	OnSessionsFound.Broadcast(Sessions);
}

bool UCyberSessionSubsystem::IsValid(const FCyberSessionResult& Session)
{
	return Session.SessionResult.IsSessionInfoValid() && Session.SessionResult.Session.NumOpenPublicConnections > 0;
}

int32 UCyberSessionSubsystem::GetPingInMs(const FCyberSessionResult& Session)
{
	return Session.SessionResult.PingInMs;
}

FString UCyberSessionSubsystem::GetHostName(const FCyberSessionResult& Session)
{
	return Session.SessionResult.Session.OwningUserName;
}

FString UCyberSessionSubsystem::GetMapName(const FCyberSessionResult& Session)
{
	FString mapName;
	Session.SessionResult.Session.SessionSettings.Get(FName(TEXT("SETTING_MAPNAME")), mapName);
	return mapName;
}

int32 UCyberSessionSubsystem::GetCurrentPlayers(const FCyberSessionResult& Session)
{
	return Session.SessionResult.Session.SessionSettings.NumPublicConnections - Session.SessionResult.Session.NumOpenPublicConnections;
}

int32 UCyberSessionSubsystem::GetMaxPlayers(const FCyberSessionResult& Session)
{
	return Session.SessionResult.Session.SessionSettings.NumPublicConnections;
}

void UCyberSessionSubsystem::JoinSession(const FCyberSessionResult& Session)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Session interface is not valid"));

		OnJoinSessionCompleted.Broadcast(false);
		return;
	}

	OnJoinSessionsCompleteHandle = sessionInterface->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionJoined));

	OnSessionParticipantJoinedDelegate = sessionInterface->AddOnSessionParticipantJoinedDelegate_Handle(FOnSessionParticipantJoinedDelegate::CreateUObject(this, &ThisClass::ParticipantsJoined));

	OnSessionParticipantLeftDelegate = sessionInterface->AddOnSessionParticipantLeftDelegate_Handle(FOnSessionParticipantLeftDelegate::CreateUObject(this, &ThisClass::ParticipantsLeft));

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!sessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Session.SessionResult))
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not Join session with name : %s"), *Session.SessionResult.GetSessionIdStr());

		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionsCompleteHandle);

		OnJoinSessionCompleted.Broadcast(false);
	}
}


void UCyberSessionSubsystem::OnSessionJoined(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionsCompleteHandle);
	}

	OnJoinSessionCompleted.Broadcast(true);

	FString connectString;

	if (!sessionInterface->GetResolvedConnectString(NAME_GameSession, connectString))
	{
		UE_LOG(LogTemp, Warning, TEXT("Did not get resolved connect string"));

		return;
	}

	APlayerController* playerController = GetWorld()->GetFirstPlayerController();
	playerController->ClientTravel(connectString, TRAVEL_Absolute);

	UE_LOG(LogTemp, Warning, TEXT("Client Travel was executed! (ConnectString: %s"), *connectString);
}

void UCyberSessionSubsystem::DestroySession()
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (!sessionInterface.IsValid())
	{
		OnDestroySessionCompleted.Broadcast(false);
		return;
	}

	OnDestroySessionCompleteHandle = sessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnSessionDestroyed));

	if (!sessionInterface->DestroySession(NAME_GameSession))
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);

		OnDestroySessionCompleted.Broadcast(false);
	}
}

void UCyberSessionSubsystem::OnSessionDestroyed(FName SessionName, bool bSuccessful)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteHandle);
	}

	OnDestroySessionCompleted.Broadcast(bSuccessful);
}

void UCyberSessionSubsystem::ParticipantsJoined(FName SessionName, const FUniqueNetId& Id)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnSessionParticipantJoinedDelegate_Handle(OnSessionParticipantJoinedDelegate);
	}

	OnParticipantsJoined.Broadcast(true);
}

void UCyberSessionSubsystem::ParticipantsLeft(FName SessionName, const FUniqueNetId& Id, EOnSessionParticipantLeftReason ReasonToLeave)
{
	const IOnlineSessionPtr sessionInterface = Online::GetSessionInterface(GetWorld());
	if (sessionInterface)
	{
		sessionInterface->ClearOnSessionParticipantLeftDelegate_Handle(OnSessionParticipantLeftDelegate);
	}

	OnParticipantsLeft.Broadcast(true);

}
