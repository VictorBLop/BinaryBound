// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Shield.generated.h"

class UAbilityTask_WaitGameplayEvent;

/**
 * 
 */
UCLASS()
class CYBER_API UShield : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UShield();

protected:

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	/* Shield Elements*/

	float CustomAbilityLevel = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Shield | Tags")
	FGameplayTag ShieldHitTag = FGameplayTag();

	UPROPERTY(EditAnywhere, Category = "Shield | Actor")
	TSubclassOf<AActor> ShieldActorClass = nullptr;

	UPROPERTY()
	AActor* ShieldActor = nullptr;

	void SetShieldRemainingHits();

	UAbilityTask_WaitGameplayEvent* TaskWaitRemoveShieldHitEvent;

	UFUNCTION()
	void RemoveShieldEventReceived(FGameplayEventData Payload);

	void RemoveResidualTags();

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
