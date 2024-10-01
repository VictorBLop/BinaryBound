// Fill out your copyright notice in the Description page of Project Settings.


#include "Slowtime.h"
#include "..\Enemy\CyberEnemy.h"
#include "..\Cyber.h"
#include "Kismet/GameplayStatics.h"
#include "..\Core\CyberGameMode.h"

USlowtime::USlowtime()
{
	AbilityTags.AddTag(SpecialAbility_Character_Slowtime);

	// Add Trigger Data
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SpecialAbility_Character_Slowtime;
	AbilityTriggers.Add(TriggerData);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USlowtime::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!SlowtimeEffect)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CustomAbilityLevel = TriggerEventData->EventMagnitude;

	// Get all Enemies and apply Gameplay Effect Slowtime.
	TArray<ACyberEnemy*> Enemies;

	if (ACyberGameMode* CyberGameMode = Cast<ACyberGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		Enemies = CyberGameMode->EnemySpawner->GetAliveEnemies();
	}

	for (int index = 0; index < Enemies.Num(); index++)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Enemies[index]))
		{
			if (ASI->GetAbilitySystemComponent())
			{
				ASI->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(SlowtimeEffect.GetDefaultObject(), CustomAbilityLevel, FGameplayEffectContextHandle(), FPredictionKey());
			}
		}
	}

	// Apply Effect to Player as well.
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		if (ASI->GetAbilitySystemComponent())
		{
			ASI->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(SlowtimeEffect.GetDefaultObject(), CustomAbilityLevel, FGameplayEffectContextHandle(), FPredictionKey());
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void USlowtime::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (!bWasCancelled)
	{
		CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);
	}
}

const FGameplayTagContainer* USlowtime::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void USlowtime::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		// Set Level as AbilityLevel, which will be the EventMagnitude in the Payload when triggering the Ability from Gameplay Event.
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), CustomAbilityLevel);

		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}