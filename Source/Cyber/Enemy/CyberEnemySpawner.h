#pragma once

#include "CoreMinimal.h"
#include "CyberEnemySpawnpoint.h"
#include "GameFramework/Actor.h"
#include "..\Interfaces/Saveable.h"
#include "..\CyberStructs.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CyberEnemySpawner.generated.h"

class ACyberEnemy;
class UCyberSaveGame;
class UCyberAttributeSet;
class UCombatAttributeSet;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemiesUpdate, int32, EnemiesRemaining);

UCLASS()
class CYBER_API ACyberEnemySpawner : public AActor, public ISaveable, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ACyberEnemySpawner();

	virtual void Tick(float DeltaTime) override;
	
	void SetActiveState(bool activated);

	UFUNCTION()
	void DestroyEnemy(ACyberEnemy* Enemy);

	UFUNCTION()
	void DestroyAllEnemies();

	UFUNCTION()
	void ResetEnemySpawner();

	UPROPERTY(BlueprintAssignable)
	FOnWaveCompleted OnWaveCompleted;

	UPROPERTY(BlueprintAssignable)
	FOnEnemiesUpdate OnEnemiesUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnGameWin OnGameWin;

	TArray<ACyberEnemy*> GetAliveEnemies();

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int CurrentWaveIndex = 0;

	int WaveCount = 0;

	int GetWaveCount();

	void SpawnEnemy(FVector location, TSubclassOf<ACyberEnemy> enemy);

	FHitResult LineTraceToGround(FVector location);

	TArray<ACyberEnemySpawnpoint*> Spawnpoints;
	
	void FindEnemySpawnpoints();
	
	UPROPERTY(BlueprintReadWrite, Category = "Spawned Enemies")
	TArray<ACyberEnemy*> Enemies;
	
	bool IsSpawnerActive = false;

	bool CanSpawnNextWave();

public:

	/* Save Game */

	UPROPERTY(EditDefaultsOnly, Category = "GAS - Save Data")
	FEnemySaveData MeleeEnemySaveData;

	UPROPERTY(EditDefaultsOnly, Category = "GAS - Save Data")
	FEnemySaveData RangedEnemySaveData;

	virtual void SaveData_Implementation(UCyberSaveGame* saveGameRef) override;

	virtual void LoadData_Implementation(UCyberSaveGame* saveGameRef) override;

	UFUNCTION()
	void OnGameLoadedEvent(UCyberSaveGame* saveGameRef);

	UFUNCTION()
	void OnGameSavedEvent();

	// Ability System Component

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	int32 GetNumberOfEnemies();

protected:

	/* Ability System Component*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> GASAbilitySystemComponent = nullptr;

	void SetupGameplayAbilitySystemComponent();

	void SetUpInitialEffects();

	/* Attribute Sets */
	void InitializeAttributeSet();

	UPROPERTY()
	TObjectPtr<UCyberAttributeSet> CyberAttributeSet;

	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

	bool InitializingAttributes = false;

	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);

	void OnDamagePowerChanged(const FOnAttributeChangeData& Data);

	void OnStunTimeChanged(const FOnAttributeChangeData& Data);

	void OnKnockbackStrenghtChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintReadOnly, Category = "GAS - Gameplay Effects")
	TArray<FGameplayTag> InitialAttributesSetByCallerTags;

	UPROPERTY(BlueprintReadOnly, Category = "GAS - Gameplay Effects")
	TArray<float> InitialAttributesMagnitudes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS - Gameplay Effects")
	TArray<AttributeTypes> ListOfAttributeTypes;

	void SetUpEnemyInitialEffects(ACyberEnemy* enemy, EnemyType enemyType);

	/* List of Wave Multipliers */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effect")
	TArray<float> WaveMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effect")
	TSubclassOf<UGameplayEffect> MultiplierEffect;

	void ApplyMultiplierEffect(int32 WaveIndex, ACyberEnemy* enemy);

	/* Debug for Save Game */
	UFUNCTION(BlueprintCallable)
	bool ApplyGameplayEffect(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	UFUNCTION(Server, Reliable)
	void ServerRPC_ApplyGameplayEffect(TSubclassOf<UGameplayEffect> effect, FGameplayTag SetByCallerTag, float Magnitude);

	/* Replicated variables to send Wave and Enemies info to UI */

	UPROPERTY(ReplicatedUsing = OnRep_EnemiesChanged)
	int32 NumberOfEnemies = 0;
	
	UFUNCTION()
	void OnRep_EnemiesChanged();

	UPROPERTY(ReplicatedUsing = OnRep_WaveChanged)
	bool bWaveChanged = false;

	UFUNCTION()
	void OnRep_WaveChanged();

	/* Get Tag and Magnitude */

	FGameplayTag GetAttributeTag(AttributeTypes AttributeType);

	float GetAttributeMagnitude(AttributeTypes AttributeType, FEnemySaveData EnemySaveData);

};
