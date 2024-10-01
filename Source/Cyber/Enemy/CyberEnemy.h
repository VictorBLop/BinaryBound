#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CyberEnemySpawner.h"
#include "CyberEnemy.generated.h"

class UCyberAttributeSet;
class UCombatAttributeSet;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyValueUpdate, float, Value, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, ACyberEnemy*, CyberEnemy);

UCLASS()
class CYBER_API ACyberEnemy : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACyberEnemy();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetCharacterMovement(bool Activate);

	static FOnEnemyDied OnEnemyDied;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//////* GAS - Gameplay Ability System *//////

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> GASAbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UCyberAttributeSet> CyberAttributeSet;

	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

	UFUNCTION(BlueprintCallable)
	void SetupGameplayAbilitySystemComponent();

	void SetupMaterialColors();

	void SetUpInitialEffects();

	void SetUpInitialEffectsFromSpawner(TArray<FGameplayTag> GameplayTags, TArray<float> Magnitudes);

	// Attribute Set Elements
	void InitializeAttributeSet();

	void HealthChanged(const FOnAttributeChangeData& Data);

	UPROPERTY(BlueprintAssignable)
	FOnEnemyValueUpdate OnHealthUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float CurrentHealth = 0.0f;

	UFUNCTION()
	void OnRep_CurrentHealth();

	// Death Elements
	UPROPERTY(ReplicatedUsing = OnRep_IsEnemyDead)
	bool bIsEnemyDead = false;

	UFUNCTION()
	void OnRep_IsEnemyDead();

	// Initial Gameplay Effects

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TSubclassOf<UGameplayEffect> InitialSpecificColorEffect;

	UPROPERTY(BlueprintReadOnly)
	FEnemySaveData DefaultEnemyStats;

	/* Color Mechanic Elements */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FName ColorVectorParameter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor EnemyColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor ColorOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor ColorTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	TObjectPtr<UMaterialInterface> CharacterColorMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_ApplyColors)
	bool bApplyColors = false;

	UFUNCTION()
	void OnRep_ApplyColors();

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	bool bUseWholeMaterials = false;

	/* Damage Materials */

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterialInterfaces;

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> CurrentMaterialInterfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attack")
	TObjectPtr<UMaterialInterface> DamageMaterialInterface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor DamageColor;

	UPROPERTY(BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor OriginalColor;

	UPROPERTY(BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor CurrentColor;

	UPROPERTY(ReplicatedUsing = OnRep_DamageMaterial)
	bool bReplicateApplyDamageMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentMaterials)
	bool bReplicateCurrentMaterials;

	UFUNCTION()
	void OnRep_DamageMaterial();

	UFUNCTION()
	void OnRep_CurrentMaterials();

	// Apply Damage - Player Feedback
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attack")
	float DelayForEnemyHitFeedback = 0.0f;

	UPROPERTY(EditAnywhere, Category = "GAS - Enemy Attack")
	TSubclassOf<UGameplayEffect> DealDamage = nullptr;

	UFUNCTION(BlueprintCallable)
	void ApplyDamagePlayerFeedback();

	UFUNCTION(BlueprintCallable)
	void EndDamagePlayerFeedback();

	void ApplyDamageEffect(AActor* HitActor);

	// Hit Reaction

	void TriggerReaction(AActor* HitActor, AActor* InstigatorActor);

	/* Change Colors During Runtime (Neutral Enemies) */

	UFUNCTION(BlueprintCallable)
	void ColorSetup(UMaterialInterface* ColorMaterialToSet, FLinearColor ColorToSet);

	UFUNCTION(BlueprintCallable)
	void ResetToOriginalColors();

	/* Death Elements */
	UPROPERTY(EditAnywhere, Category = "GAS - Death Effect")
	TArray<TSubclassOf<UGameplayEffect>> DeathEffects;

	void DestroyEnemy();

	UPROPERTY(EditAnywhere, Category = "GAS - Death Effect")
	float TimeBeforeDestroyingEnemy = 0.0f;

	/* Slowtime */

	UFUNCTION(BlueprintCallable)
	void SetCyberTimeDilation(float NewTimeDilation);

	UFUNCTION(BlueprintCallable)
	float GetCyberTimeDilation();

protected:

	/* Slowtime Ability */

	UPROPERTY(ReplicatedUsing = OnRep_CyberTimeDilation)
	float CyberTimeDilation = 1.0f;

	UFUNCTION()
	void OnRep_CyberTimeDilation();
};
