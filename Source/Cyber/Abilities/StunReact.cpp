// Fill out your copyright notice in the Description page of Project Settings.


#include "StunReact.h"
#include "..\Cyber.h"
#include "..\Enemy\CyberEnemy.h"
#include "GameFramework/Character.h"

UStunReact::UStunReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UStunReact::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UStunReact::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	CyberEnemy = Cast<ACyberEnemy>(GetAvatarActorFromActorInfo());

	if (!CyberEnemy)
	{
		return;
	}

	/* Wait Gameplay Tags to be Added - GameplayEffect_MarkInArray is added to Player to mark the start of the ability */
	WaitAddTagStun = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, StunEffectTag, GetAvatarActorFromActorInfo(), false);
	WaitAddTagStun->Added.AddDynamic(this, &ThisClass::OnStunTagAdded);
	WaitAddTagStun->ReadyForActivation();
}

void UStunReact::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UStunReact::OnStunTagAdded()
{
	if (CyberEnemy)
	{
		CyberEnemy->SetCharacterMovement(false);
	}

	/* Wait Gameplay Tags to be Removed - GameplayEffect_MarkInArray is removed from Player to mark the end of the ability */

	WaitRemoveTagStun = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, StunEffectTag, GetAvatarActorFromActorInfo(), false);
	WaitRemoveTagStun->Removed.AddDynamic(this, &ThisClass::OnStunTagRemoved);
	WaitRemoveTagStun->ReadyForActivation();
}

void UStunReact::OnStunTagRemoved()
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		if (!ASI->GetAbilitySystemComponent())
		{
			return;
		}
		
		if (ASI->GetAbilitySystemComponent()->GetTagCount(StunEffectTag) <= 0)
		{
			CyberEnemy->SetCharacterMovement(true);
		}
	}
}
