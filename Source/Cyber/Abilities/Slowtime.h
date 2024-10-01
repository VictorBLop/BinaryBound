// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Slowtime.generated.h"

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class CYBER_API USlowtime : public UGameplayAbility
{
	GENERATED_BODY()

public:

	USlowtime();

protected:

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	float CustomAbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Abilities | Slowtime ")
	TSubclassOf<AActor> CyberEnemyClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Special Abilities | Slowtime")
	TSubclassOf<UGameplayEffect> SlowtimeEffect;

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
