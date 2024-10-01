#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Heal.generated.h"

class UGameplayEffect;

UCLASS()
class CYBER_API UHeal : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UHeal();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> levels;
	
	virtual void ActivateAbility
	(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility
	(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	float CustomAbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Abilities | Heal")
	TSubclassOf<UGameplayEffect> HealEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Abilities | Heal")
	TSubclassOf<UGameplayEffect> HealTagEffect;
	
	const FGameplayTagContainer* GetCooldownTags() const override;

	void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	FScalableFloat CooldownDuration;

	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
};
