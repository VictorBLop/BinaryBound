// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "TokensAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class CYBER_API UTokensAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	/* Enemy Tokens - Receive x tokens per each enemy destroyed */
	UPROPERTY(BlueprintReadOnly, Category = "Tokens", ReplicatedUsing = OnRep_EnemyTokens)
	FGameplayAttributeData EnemyTokens;

	ATTRIBUTE_ACCESSORS(UTokensAttributeSet, EnemyTokens)

	UFUNCTION()
	void OnRep_EnemyTokens(const FGameplayAttributeData& OldEnemyTokens);


	/* Enemy Tokens Reward - How many tokens are received per enemy destroyed */
	UPROPERTY(BlueprintReadOnly, Category = "Tokens", ReplicatedUsing = OnRep_TokensPerEnemy)
	FGameplayAttributeData TokensPerEnemy;

	ATTRIBUTE_ACCESSORS(UTokensAttributeSet, TokensPerEnemy)

	UFUNCTION()
	void OnRep_TokensPerEnemy(const FGameplayAttributeData& OldTokensPerEnemy);


	/* Cycle Tokens - Receive 1 token per survived cycle */
	UPROPERTY(BlueprintReadOnly, Category = "Tokens", ReplicatedUsing = OnRep_CycleTokens)
	FGameplayAttributeData CycleTokens;

	ATTRIBUTE_ACCESSORS(UTokensAttributeSet, CycleTokens)

	UFUNCTION()
	void OnRep_CycleTokens(const FGameplayAttributeData& OldCycleTokens);


	/* Cycle Tokens Reward - How many tokens are received when a cycle is survived */
	UPROPERTY(BlueprintReadOnly, Category = "Tokens", ReplicatedUsing = OnRep_TokensPerCycle)
	FGameplayAttributeData TokensPerCycle;

	ATTRIBUTE_ACCESSORS(UTokensAttributeSet, TokensPerCycle)

	UFUNCTION()
	void OnRep_TokensPerCycle(const FGameplayAttributeData& OldTokensPerCycle);


	// Replication
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	// Clamp The base attribute value being changed. 
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Clamp The current attribute value being changed. 
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
