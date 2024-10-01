// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "..\CyberSaveGame.h"
#include "Saveable.generated.h"

class UCyberSaveGame;

UINTERFACE(Blueprintable)
class USaveable : public UInterface
{
    GENERATED_BODY()
};

class ISaveable
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Game")
    void SaveData(UCyberSaveGame* saveGameRef);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Game")
    void LoadData(UCyberSaveGame* saveGameRef);
};