// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 

void UColorAttributeSet::OnRep_MarkedTime(const FGameplayAttributeData& OldMarkedTime)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MarkedTime, OldMarkedTime)
}

void UColorAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MarkedTime, COND_None, REPNOTIFY_Always);
}

void UColorAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetMarkedTimeAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UColorAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMarkedTimeAttribute())
	{
		// Do not allow health to go negative or above max health.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UColorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetMarkedTimeAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetMarkedTime(0.0f);
		}
	}
}


