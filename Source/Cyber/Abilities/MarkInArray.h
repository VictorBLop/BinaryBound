// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h" 
#include "MarkInArray.generated.h"

class ACyberCharacter;

/**
 * 
 */
UCLASS()
class CYBER_API UMarkInArray : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UMarkInArray();

protected:

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UAbilityTask_WaitGameplayTagAdded* WaitAddTagTask = nullptr;

	UFUNCTION()
	void OnMarkInArrayEffectApplied();

	UAbilityTask_WaitGameplayTagRemoved* WaitRemoveTagTask = nullptr;

	UFUNCTION()
	void OnMarkInArrayEffectRemoved();

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;


	/* Mark In Array Elements*/

	UPROPERTY()
	ACyberCharacter* CyberCharacter = nullptr;

	UPROPERTY()
	AActor* MarkingActor = nullptr;

	float CustomAbilityLevel = 1.0f;

	UPROPERTY(EditAnywhere)
	float MarkArrayEffectDuration = 2.5f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MarkArrayActorClass = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<float> SphereRadiuses;

	UPROPERTY(EditDefaultsOnly, Category = "Mark in Array Effect")
	TSubclassOf<UGameplayEffect> MarkInArrayUIEffect = nullptr;

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
