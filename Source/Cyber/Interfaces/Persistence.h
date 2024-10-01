// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Persistence.generated.h"

UINTERFACE(Blueprintable)
class UPersistence : public UInterface
{
    GENERATED_BODY()
};

class IPersistence
{
    GENERATED_BODY()

public:

    virtual void RequestSave(bool Async) = 0; // Pure virtual

    virtual void RequestLoad(UObject* Requester) = 0; // Pure virtual
};