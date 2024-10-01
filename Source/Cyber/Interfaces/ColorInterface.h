// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ColorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Meta = (CannotImplementInterfaceInBlueprint))
class UColorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CYBER_API IColorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable)
	virtual void ActivateTrail();

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateTrail();

	UFUNCTION(BlueprintCallable)
	virtual FLinearColor GetPlayerColor();
};
