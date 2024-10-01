// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 

void UCombatAttributeSet::OnRep_DamagePower(const FGameplayAttributeData& OldDamagePower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamagePower, OldDamagePower)
}

void UCombatAttributeSet::OnRep_KnockbackStrength(const FGameplayAttributeData& OldKnockbackStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, KnockbackStrength, OldKnockbackStrength)
}

void UCombatAttributeSet::OnRep_StunTime(const FGameplayAttributeData& OldStunTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, StunTime, OldStunTime)
}

void UCombatAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DamagePower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, KnockbackStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, StunTime, COND_None, REPNOTIFY_Always);
}

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetDamagePowerAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetKnockbackStrengthAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetStunTimeAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetDamagePowerAttribute())
	{
		// Do not allow health to go negative or above max health.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetKnockbackStrengthAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetStunTimeAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamagePowerAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetDamagePower(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetKnockbackStrengthAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetKnockbackStrength(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStunTimeAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetStunTime(0.0f);
		}
	}
}
