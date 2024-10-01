// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h" 
#include "DashAbility.generated.h"

/**
 * 
 */
UCLASS()
class CYBER_API UDashAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UDashAbility();

protected:

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnDashFinish();

	UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = nullptr;

	UPROPERTY(EditAnywhere, Category = "Dash Effect")
	float DashStrength = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Dash Effect")
	float DashDuration = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Effect")
	TSubclassOf<UGameplayEffect> DashEffectClass;

	/* COOLDOWN */

	const FGameplayTagContainer* GetCooldownTags() const override;

	void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	FScalableFloat CooldownDuration;

	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
};
