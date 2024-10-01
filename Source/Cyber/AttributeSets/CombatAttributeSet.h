// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"

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
class CYBER_API UCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:

	/* Damage Power - How much damage is applied when attacking */
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_DamagePower)
	FGameplayAttributeData DamagePower;

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, DamagePower)

	UFUNCTION()
	void OnRep_DamagePower(const FGameplayAttributeData& OldDamagePower);

	
	/* Knockback Strength - How much knockback is applied to the target */
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_KnockbackStrength)
	FGameplayAttributeData KnockbackStrength;

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, KnockbackStrength)

	UFUNCTION()
	void OnRep_KnockbackStrength(const FGameplayAttributeData& OldKnockbackStrength);


	/* Stun Time - How much time the target is stunned */
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_StunTime)
	FGameplayAttributeData StunTime;

	ATTRIBUTE_ACCESSORS(UCombatAttributeSet, StunTime)

	UFUNCTION()
	void OnRep_StunTime(const FGameplayAttributeData& OldStunTime);


	// Replication
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

protected:

	// Clamp The base attribute value being changed. 
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Clamp The current attribute value being changed. 
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
};
