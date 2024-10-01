#include "Heal.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Cyber/Cyber.h"
#include "Cyber/Core/CyberCharacter.h"
#include "Cyber/Core/CyberPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "..\Core\CyberGameState.h"

UHeal::UHeal()
{
	AbilityTags.AddTag(SpecialAbility_Character_Heal);

	// Add Trigger Data
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SpecialAbility_Character_Heal;
	AbilityTriggers.Add(TriggerData);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UHeal::ActivateAbility
(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	CustomAbilityLevel = TriggerEventData->EventMagnitude;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		if (ASI->GetAbilitySystemComponent() && HealTagEffect)
		{
			ASI->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(HealTagEffect.GetDefaultObject(), CustomAbilityLevel, FGameplayEffectContextHandle(), FPredictionKey());
		}
	}

	if(GetAvatarActorFromActorInfo()->HasAuthority())
	{
		if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
		{
			CyberGameState->ApplyGameplayEffectGameState(HealEffect, GameplayEffect_Heal_SetByCaller, levels[static_cast<int>(CustomAbilityLevel) - 1]);
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UHeal::EndAbility
(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);
}

const FGameplayTagContainer* UHeal::GetCooldownTags() const
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

void UHeal::ApplyCooldown
(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo
) const
{
	UGameplayEffect* HealGE = GetCooldownGameplayEffect();
	if (HealGE)
	{
		// Set Level as AbilityLevel, which will be the EventMagnitude in the Payload when triggering the Ability from Gameplay Event.
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(HealGE->GetClass(), CustomAbilityLevel);

		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
