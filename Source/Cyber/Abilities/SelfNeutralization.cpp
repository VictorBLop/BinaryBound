// Fill out your copyright notice in the Description page of Project Settings.


#include "SelfNeutralization.h"
#include "..\Core\CyberCharacter.h"
#include "..\Cyber.h"

USelfNeutralization::USelfNeutralization()
{
	AbilityTags.AddTag(SpecialAbility_Character_SelfNeutralization);

	// Add Trigger Data
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SpecialAbility_Character_SelfNeutralization;
	AbilityTriggers.Add(TriggerData);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void USelfNeutralization::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!SelfNeutralizationEffect)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CustomAbilityLevel = TriggerEventData->EventMagnitude;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		FGameplayEffectContextHandle EffectContext = ASI->GetAbilitySystemComponent()->MakeEffectContext();

		FGameplayEffectSpecHandle SpecHandle = ASI->GetAbilitySystemComponent()->MakeOutgoingSpec(SelfNeutralizationEffect, CustomAbilityLevel, EffectContext);
		
		FGameplayEffectSpec* Spec;

		Spec = new FGameplayEffectSpec(SelfNeutralizationEffect.GetDefaultObject(), EffectContext, CustomAbilityLevel);

		FActiveGameplayEffectHandle EffectHandle = ASI->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());

		if (!EffectHandle.WasSuccessfullyApplied())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
		
		if (ACyberCharacter* CyberCharacter = Cast<ACyberCharacter>(GetAvatarActorFromActorInfo()))
		{
			CyberCharacter->ChangeCharacterColor(NeutralColor);
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void USelfNeutralization::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (!bWasCancelled)
	{
		CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);
	}
}

const FGameplayTagContainer* USelfNeutralization::GetCooldownTags() const
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

void USelfNeutralization::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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

