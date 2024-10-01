// Fill out your copyright notice in the Description page of Project Settings.


#include "Shield.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "GameplayEffect.h"
#include "ShieldActor.h"
#include "..\Cyber.h"
#include "..\Core\CyberCharacter.h"
#include "..\Interfaces\ColorInterface.h"

UShield::UShield()
{
	// Add Trigger Data
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SpecialAbility_Character_Shield;
	AbilityTriggers.Add(TriggerData);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UShield::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);

	if (!ShieldActorClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CustomAbilityLevel = TriggerEventData->EventMagnitude;

	/* Spawn Actor */

	FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	FRotator Rotation = GetAvatarActorFromActorInfo()->GetActorRotation();

	// Set Scale multiplier depending on the Ability Level.

	FActorSpawnParameters SpawnInfo;

	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnInfo.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnInfo.Owner = GetAvatarActorFromActorInfo();

	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(Rotation.Quaternion());
	Transform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	/* Spawn Actor */
	ShieldActor = GetWorld()->SpawnActor<AActor>(
		ShieldActorClass,
		Transform,
		SpawnInfo
	);

	///* Change Color of Widget to Player's color */
	if (ShieldActor)
	{
		if (AShieldActor* ShieldActorSpec = Cast<AShieldActor>(ShieldActor))
		{
			if (IColorInterface* IColor = Cast<IColorInterface>(GetAvatarActorFromActorInfo()))
			{
				ShieldActorSpec->SetShieldColor(IColor->GetPlayerColor());
			}
		}
	}

	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);

	bool WasSuccessfullyAttached = ShieldActor->AttachToActor(GetAvatarActorFromActorInfo(), AttachmentRules, TEXT(""));

	if (!WasSuccessfullyAttached)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	// Add as many tags as necessary to the actor.
	for (int32 index = 0; index < CustomAbilityLevel; index++)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
		{
			ASI->GetAbilitySystemComponent()->AddLooseGameplayTag(ShieldHitTag);
		}
	}

	SetShieldRemainingHits();

	/* Wait Gameplay Event to be received - whenever a ShieldHitTag is removed, the event happens. It is triggered from GA_HitReact. */

	TaskWaitRemoveShieldHitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GameplayEvent_RemoveShieldHit, nullptr, false, true);
	TaskWaitRemoveShieldHitEvent->EventReceived.AddDynamic(this, &ThisClass::RemoveShieldEventReceived);
	TaskWaitRemoveShieldHitEvent->ReadyForActivation();

}

void UShield::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ShieldActor)
	{
		ShieldActor->Destroy();
	}

	RemoveResidualTags();
}

void UShield::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	RemoveResidualTags();
}

void UShield::SetShieldRemainingHits()
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		if(ShieldActor)
		{
			if (AShieldActor* ShieldActorSpec = Cast<AShieldActor>(ShieldActor))
			{
				FText ShieldTagCountText = UKismetTextLibrary::Conv_IntToText(ASI->GetAbilitySystemComponent()->GetTagCount(GameplayEffect_Shield_Hit), false, true, 1, 324);

				ShieldActorSpec->SetShieldText(ShieldTagCountText);
			}
		}

		// If no more tags are in, end ability.
		if (ASI->GetAbilitySystemComponent()->GetTagCount(GameplayEffect_Shield_Hit) <= 0)
		{
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		}
	}
}

void UShield::RemoveShieldEventReceived(FGameplayEventData Payload)
{
	SetShieldRemainingHits();
}

void UShield::RemoveResidualTags()
{
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		for (int32 index = 0; index < ASI->GetAbilitySystemComponent()->GetTagCount(GameplayEffect_Shield_Hit) + 3; index++)
		{
			ASI->GetAbilitySystemComponent()->RemoveLooseGameplayTag(GameplayEffect_Shield_Hit);
		}
	}
}

const FGameplayTagContainer* UShield::GetCooldownTags() const
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

void UShield::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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
