// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MovementAttributeSet.generated.h"

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
class CYBER_API UMovementAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	/* Marked Power - How much time neutral color enemies are marked with a color */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_DashCooldown)
	FGameplayAttributeData DashCooldown;

	ATTRIBUTE_ACCESSORS(UMovementAttributeSet, DashCooldown)

	UFUNCTION()
	void OnRep_DashCooldown(const FGameplayAttributeData& OldDashCooldown);


	// Replication
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


protected:

	// Clamp The base attribute value being changed. 
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	// Clamp The current attribute value being changed. 
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

};
