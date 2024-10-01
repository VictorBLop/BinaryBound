// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CyberCharacter.h"
#include "..\CyberStructs.h"
#include "..\Interfaces/Saveable.h"
#include "CyberPlayerState.generated.h"

class UCyberAttributeSet;
class UTokensAttributeSet;
class UCombatAttributeSet;
class UColorAttributeSet;
class UMovementAttributeSet;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntValueUpdate, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerReadyToStart);

/**
 * 
 */
UCLASS()
class CYBER_API ACyberPlayerState : public APlayerState, public IAbilitySystemInterface, public ISaveable
{
	GENERATED_BODY()
	
public:

	ACyberPlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	TObjectPtr<UCyberAttributeSet> GetCyberAttributeSet();

	TObjectPtr<UTokensAttributeSet> GetTokensAttributeSet();

	TObjectPtr<UCombatAttributeSet> GetCombatAttributeSet();

	TObjectPtr<UColorAttributeSet> GetColorAttributeSet();

	TObjectPtr<UMovementAttributeSet> GetMovementAttributeSet();

	void InitializeAttributeSet();

	/* Death Elements */

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsCharacterDead = false;

	UFUNCTION(BlueprintCallable)
	void ResetPlayerDeathState(bool isCharacterDead);

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> GASAbilitySystemComponent = nullptr;

	/* Attributes Related Elements*/
		
	UPROPERTY()
	TObjectPtr<UCyberAttributeSet> CyberAttributeSet;

	UPROPERTY()
	TObjectPtr<UTokensAttributeSet> TokensAttributeSet;

	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

	UPROPERTY()
	TObjectPtr<UColorAttributeSet> ColorAttributeSet;

	UPROPERTY()
	TObjectPtr<UMovementAttributeSet> MovementAttributeSet;

		// Health

	void HealthChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintAssignable)
	FOnIntValueUpdate OnHealthUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float HealthForBroadcasting;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void CharactersHealthReachedZero();

	UFUNCTION(Server, Reliable)
	void ServerRPC_CharactersHealthReachedZero();

		// Enemy Tokens
	void EnemyTokensChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintAssignable)
	FOnIntValueUpdate OnEnemyTokensUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_EnemyTokens)
	int32 EnemyTokensForBroadcasting;

	UFUNCTION()
	void OnRep_EnemyTokens();

		// Cycle Tokens
	
	void CycleTokensChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintAssignable)
	FOnIntValueUpdate OnCycleTokensUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_CycleTokens)
	int32 CycleTokensForBroadcasting;

	UFUNCTION()
	void OnRep_CycleTokens();


	/* Replication */
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	// Restart Game

	UPROPERTY(Replicated)
	bool ReadyToRestart = false;

	// Save Game

	virtual void SaveData_Implementation(UCyberSaveGame* saveGameRef) override;

	virtual void LoadData_Implementation(UCyberSaveGame* saveGameRef) override;

	void SaveData_SpecificPlayer(UCyberSaveGame* saveGameRef, const FUpgradeInfo& playerInfo);

	void LoadData_SpecificPlayer(UCyberSaveGame* saveGameRef, const FUpgradeInfo& playerInfo);

	UFUNCTION()
	void OnGameLoadedEvent(UCyberSaveGame* saveGameRef);

	UFUNCTION()
	void OnGameSavedEvent();


public:

	UFUNCTION(BlueprintCallable)
	ACyberCharacter* GetOtherPlayerReference();

	UFUNCTION(BlueprintCallable)
	void SetOtherPlayerReference(ACyberCharacter* playerCharacter);

	void HealthReachedZero();

	// Restart Game

	UFUNCTION(BlueprintCallable)
	void SetReadyToRestart(bool IsPlayerReadyToRestart);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetReadyToRestart(bool IsPlayerReadyToRestart);

	UPROPERTY(BlueprintAssignable)
	FOnPlayerReadyToStart OnPlayerReadyToStart;

	/* Player State Index for Save Game */
	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 CyberPlayerIndex = 0;

	/* List of Upgrades */

	UPROPERTY(BlueprintReadWrite)
	TArray<FUpgradeAbilitySlotListEntry> PlayerUpgradeAbilitySlotList;

	UFUNCTION(BlueprintCallable)
	void SetPlayerUpgradesInServer(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetPlayerUpgradesInServer(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetPlayerUpgradesInClient(const TArray<FUpgradeAbilitySlotListEntry>& PlayerListAbilitesUpgrades);


	/* Apply Gameplay Effects Upgrades */
	
		// All Players using Game State.

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectAllPlayers(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyGameplayEffectAllPlayers(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

		// Specific Player using Player State.

	UFUNCTION()
	void ApplyGameplayEffectPlayerStateFromDelegate(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UFUNCTION(BlueprintCallable)
	bool ApplyGameplayEffectPlayerState(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyGameplayEffectPlayerState(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

		// All Enemies using Enemy Spawner.

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectEnemySpawner(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyGameplayEffectEnemySpawner(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

protected:

	UPROPERTY(BlueprintReadOnly)
	ACyberCharacter* OtherPlayer = nullptr;
};
