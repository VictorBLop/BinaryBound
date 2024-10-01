// Fill out your copyright notice in the Description page of Project Settings.


#include "TokensAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 

void UTokensAttributeSet::OnRep_EnemyTokens(const FGameplayAttributeData& OldEnemyTokens)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, EnemyTokens, OldEnemyTokens)
}

void UTokensAttributeSet::OnRep_TokensPerEnemy(const FGameplayAttributeData& OldTokensPerEnemy)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, TokensPerEnemy, OldTokensPerEnemy)
}

void UTokensAttributeSet::OnRep_CycleTokens(const FGameplayAttributeData& OldCycleTokens)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, CycleTokens, OldCycleTokens)
}

void UTokensAttributeSet::OnRep_TokensPerCycle(const FGameplayAttributeData& OldTokensPerCycle)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, TokensPerCycle, OldTokensPerCycle)
}

void UTokensAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, EnemyTokens, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CycleTokens, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TokensPerEnemy, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, TokensPerCycle, COND_None, REPNOTIFY_Always);
}

void UTokensAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetEnemyTokensAttribute() || Attribute == GetCycleTokensAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetTokensPerEnemyAttribute() || Attribute == GetTokensPerCycleAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UTokensAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetEnemyTokensAttribute() || Attribute == GetCycleTokensAttribute())
	{
		// Do not allow health to go negative or above max health.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
	else if (Attribute == GetTokensPerEnemyAttribute() || Attribute == GetTokensPerCycleAttribute())
	{
		// Do not allow tokens to go below zerp.
		if (NewValue <= 0.0f)
		{
			NewValue = 0.0f;
		}
	}
}

void UTokensAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetEnemyTokensAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetEnemyTokens(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetCycleTokensAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetCycleTokens(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetTokensPerEnemyAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetTokensPerEnemy(0.0f);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetTokensPerCycleAttribute())
	{
		if (Data.EvaluatedData.Attribute.GetNumericValue(this) <= 0.0f)
		{
			SetTokensPerCycle(0.0f);
		}
	}
}
