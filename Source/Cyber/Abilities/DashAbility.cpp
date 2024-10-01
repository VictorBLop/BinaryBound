// Fill out your copyright notice in the Description page of Project Settings.


#include "DashAbility.h"
#include "..\Cyber.h"
#include "..\Core\CyberCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayEffect.h"
#include "..\AttributeSets\MovementAttributeSet.h"

UDashAbility::UDashAbility()
{
	AbilityTags.AddTag(Ability_Character_Dash);
	ActivationOwnedTags.AddTag(Ability_Character_Dash);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UDashAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);

	if (ACyberCharacter* CyberCharacter = Cast<ACyberCharacter>(GetAvatarActorFromActorInfo()))
	{
		FVector NormalizedAcceleration = UKismetMathLibrary::Vector_Normal2D(CyberCharacter->GetCyberCurrentAcceleration(), 0.0001); // Get Normalized Acceleration

		bool AccelerationIsNearlyZero = UKismetMathLibrary::Vector_IsNearlyZero(CyberCharacter->GetCyberCurrentAcceleration(), 0.0001); // Check if Acceleration is almost zero
		
		FVector WorldDirection = UKismetMathLibrary::SelectVector(CyberCharacter->GetActorForwardVector(), NormalizedAcceleration, AccelerationIsNearlyZero); // Select Vector from Forward Vector or Acceleration
			
		// Apply RootMotionConstantForce Ability Task.
		RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT("DashTask"),
			WorldDirection,
			DashStrength,
			DashDuration,
			false,
			nullptr,
			ERootMotionFinishVelocityMode::ClampVelocity,
			FVector(0.0f, 0.0f, 0.0f),
			CyberCharacter->GetCyberMaxWalkSpeed(),
			true
		);

		if (RootMotionTask)
		{
			RootMotionTask->OnFinish.AddDynamic(this, &UDashAbility::OnDashFinish);

			RootMotionTask->ReadyForActivation();
		}	

		// Apply Gameplay Effect Dash to call Gameplay Cue from it.
		ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, DashEffectClass.GetDefaultObject(), 1.0f, 1);
		
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UDashAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);	
}

void UDashAbility::OnDashFinish()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);

	RootMotionTask->OnFinish.RemoveAll(this);
}

const FGameplayTagContainer* UDashAbility::GetCooldownTags() const
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

void UDashAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		// Combat Attribute Set
		if (ACyberCharacter* CyberCharacter = Cast<ACyberCharacter>(GetAvatarActorFromActorInfo()))
		{
			if (const UMovementAttributeSet* MovementAttributeSet = Cast<UMovementAttributeSet>(CyberCharacter->GetAbilitySystemComponent()->GetAttributeSet(UMovementAttributeSet::StaticClass())))
			{
				FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
				SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(Ability_Character_Dash_Cooldown_SetByCaller, MovementAttributeSet->DashCooldown.GetCurrentValue());
				ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			}
		}
	}
}
