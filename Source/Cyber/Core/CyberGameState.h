// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameplayEffect.h"
#include "..\CyberStructs.h"
#include "..\Interfaces\Saveable.h"
#include "CyberGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameTimerUpdate, int32, RemainingMinutes, int32, RemainingSeconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValueUpdate, float, currentValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnApplyEffectOnAllPlayers, TSubclassOf<UGameplayEffect>, effect, FGameplayTag, SetByCallerTag, float, Magnitude);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRemoveWidget);

class ACyberPlayerState;
class ACyberPlayerController;
class UCyberGameInstance;
class ACyberEnemySpawner;
class UAudioComponent;
class USoundBase;

/**
 * 
 */
UCLASS()
class CYBER_API ACyberGameState : public AGameState, public ISaveable
{
	GENERATED_BODY()
	
protected:

	ACyberGameState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void TriggerGameStart();

	UFUNCTION(BlueprintCallable)
	void TriggerPreGameStart();

protected:

	/* Pre Game Timer Elements */

	FTimerHandle PreGameTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Time | Pre Game Time")
	float PreGameDuration = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_PreGameTimerRemaining)
	float PreGameTimeRemaining = 0.0f;

	UFUNCTION()
	void OnRep_PreGameTimerRemaining();

	void OnPreGameTimerEnd();

	void OneSecondPreGamePassedBy();

	UPROPERTY(ReplicatedUsing = OnRep_HasPreGameTimerEnded)
	bool bHasPreGameTimerEnded = false;

	UFUNCTION()
	void OnRep_HasPreGameTimerEnded();

	/* Game Timer Elements */

	FTimerHandle OneSecondTimer;

	void OneSecondPassedBy();

	FTimerHandle GameTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Time")
	float GameDuration = 0.0f;

	UPROPERTY(ReplicatedUsing = OnRep_TimerRemaining)
	float TimeRemaining = 0.0f;

	UFUNCTION()
	void OnRep_TimerRemaining();

	void OnGameTimerEnd();

	UPROPERTY(ReplicatedUsing = OnRep_HasGameTimerEnded)
	bool bHasGameTimerEnded = false;

	UFUNCTION()
	void OnRep_HasGameTimerEnded();

	UPROPERTY(BlueprintAssignable)
	FOnGameTimerUpdate OnGameTimerUpdate;

	/* Enemy Tokens Related Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TSubclassOf<UGameplayEffect> ModifyEnemyTokens = nullptr;

	/* Cycle Tokens Related Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TSubclassOf<UGameplayEffect> ModifyCycleTokens = nullptr;

	/* Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* Game Over */
	void GameEnd(bool Win, bool CycleCompleted);

	UPROPERTY(BlueprintReadOnly)
	bool bCycleCompleted = false;

	UPROPERTY(BlueprintReadOnly)
	bool bGameWin = false;

	void CallGameOverAfterTimer();

	FTimerHandle GameOverTimer;

	void SetCycleTokens();

	/* Restart Players */
	UPROPERTY(BlueprintAssignable)
	FOnRemoveWidget OnRestartGame;

	UPROPERTY(ReplicatedUsing = OnRep_RestartBothPlayers)
	bool RestartBothPlayers = false;

	UFUNCTION()
	void OnRep_RestartBothPlayers();

	UPROPERTY(Replicated)
	int32 PlayersRestarted = 0;

	UFUNCTION()
	void PlayerPressedRestart();

	/* Save Game */

	UPROPERTY()
	TObjectPtr<UCyberGameInstance> CyberGameInstanceRef = nullptr;

	void LoadGameForPlayers();

	void SaveGameForPlayers();

	// Save Game

	virtual void SaveData_Implementation(UCyberSaveGame* saveGameRef) override;

	virtual void LoadData_Implementation(UCyberSaveGame* saveGameRef) override;

	UFUNCTION()
	void OnGameLoadedEvent(UCyberSaveGame* saveGameRef);

	UFUNCTION()
	void OnGameSavedEvent();


public:

	/* Player Stats */

	UPROPERTY(BlueprintReadOnly)
	FPlayerEndStats PlayerStats;

	UFUNCTION(NetMulticast, Reliable)
	void Client_SetPlayerStats(FPlayerEndStats ServerPlayerStats);

	UFUNCTION(Server, Reliable)
	void Server_SetPlayerStats();

	/* Music */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* BackgroundMusic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* BackgroundMusicComponent;

	UPROPERTY(BlueprintReadOnly)
	bool HasDeathEffectsApplied = false;

	UPROPERTY(ReplicatedUsing = OnRep_PlayBackgroundMusic)
	bool bPlayBackgroundMusic = false;

	UFUNCTION()
	void OnRep_PlayBackgroundMusic();

	UFUNCTION()
	void StopBackgroundMusic();

	/* Enemy Spawner */

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ACyberEnemySpawner> CyberEnemySpawner = nullptr;

	UFUNCTION()
	void OnWaveCompleted();

	void PlayerJoined();

	void SetEnemyTokens();

	UFUNCTION(BlueprintCallable)
	void RestartCyberGame();
	
	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameOver;

	UPROPERTY(BlueprintAssignable)
	FOnGameOver OnGameEndAllPlayers;

	UFUNCTION()
	void OnGameWin();

	UPROPERTY(ReplicatedUsing = OnRep_GameWonRep)
	bool bGameWonRep = false;

	UFUNCTION()
	void OnRep_GameWonRep();

	/* Health Elements */

	void CommonHealthReachedZero();

	UPROPERTY(ReplicatedUsing = OnRep_HealthReachedZero)
	bool bHealthReachedZero = false;

	UFUNCTION()
	void OnRep_HealthReachedZero();

	/* Cycle Completed */

	UFUNCTION(BlueprintCallable)
	bool HasCycleBeenCompleted();

	/* Debug for Save Game */

	UFUNCTION(BlueprintCallable)
	void ApplyGameplayEffectGameState(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UPROPERTY(BlueprintAssignable)
	FOnApplyEffectOnAllPlayers ApplyEffectOnAllPlayers;

	UFUNCTION()
	void ApplyGameplayEffectEnemySpawner(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	/* Widget Manager */

	UPROPERTY(ReplicatedUsing = OnRep_AddStartWidget)
	bool bAddStartWidget = false;

	UFUNCTION()
	void OnRep_AddStartWidget();

	/* Game Over Effects - Remove all cooldowns and active gameplay effects */
	UPROPERTY(EditAnywhere, Category = "GAS - Death Effect")
	TArray<TSubclassOf<UGameplayEffect>> DeathEffects;

	/* Game Win Effect*/
	UPROPERTY(EditAnywhere, Category = "GAS - Death Effect")
	TArray<TSubclassOf<UGameplayEffect>> GameWinEffects;

	/* Deal Damage Effect*/
	UPROPERTY(EditAnywhere, Category = "GAS - Death Effect")
	TSubclassOf<UGameplayEffect> DealDamageEffect;

	bool PlayersHaveShield();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_ReferenceOtherPlayer)
	bool bReferenceOtherPlayer = false;

	UFUNCTION()
	void OnRep_ReferenceOtherPlayer();

	UFUNCTION(BlueprintCallable)
	void ReferenceOtherPlayer();
};
