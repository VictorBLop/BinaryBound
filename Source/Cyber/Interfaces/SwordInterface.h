// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SwordInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class USwordInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CYBER_API ISwordInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, Category = "Sword")
	virtual void TurnOnSwordCollision();

	UFUNCTION(BlueprintCallable, Category = "Sword")
	virtual void TurnOffSwordCollision();

	UFUNCTION(BlueprintCallable, Category = "Sword")
	virtual void SetActivateKnockback(bool ActivateKnockback);

	UFUNCTION(BlueprintCallable, Category = "Sword | Color")
	virtual void SetSwordColor(FName ParameterName, FLinearColor SwordColor);
};
