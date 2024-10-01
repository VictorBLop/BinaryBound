// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayTag.h" 
#include "StunReact.generated.h"

class ACyberEnemy;

/**
 * 
 */
UCLASS()
class CYBER_API UStunReact : public UGameplayAbility
{
	GENERATED_BODY()
	
public:

	UStunReact();

protected:

	/** Called when the avatar actor is set/changes */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	bool bActivateAbilityOnGranted = true;

	UFUNCTION()
	void OnStunTagAdded();

	UFUNCTION()
	void OnStunTagRemoved();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag StunEffectTag;

	UAbilityTask_WaitGameplayTagAdded* WaitAddTagStun = nullptr;

	UAbilityTask_WaitGameplayTagRemoved* WaitRemoveTagStun = nullptr;

	ACyberEnemy* CyberEnemy = nullptr;
};
