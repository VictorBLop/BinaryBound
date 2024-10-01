// Fill out your copyright notice in the Description page of Project Settings.


#include "MovementAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 

void UMovementAttributeSet::OnRep_DashCooldown(const FGameplayAttributeData& OldDashCooldown)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DashCooldown, OldDashCooldown)
}

void UMovementAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DashCooldown, COND_None, REPNOTIFY_Always);
}

void UMovementAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetDashCooldownAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.25f)
		{
			NewValue = 0.25f;
		}
	}
}

void UMovementAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDashCooldownAttribute())
	{
		// Do not allow health to go negative or above max health.
		if (NewValue <= 0.25f)
		{
			NewValue = 0.25f;
		}
	}

}

void UMovementAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDashCooldownAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.25f)
		{
			SetDashCooldown(0.25f);
		}
	}
}
