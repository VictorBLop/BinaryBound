// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CyberTraps.generated.h"

class UCombatAttributeSet;

UCLASS()
class CYBER_API ACyberTraps : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACyberTraps();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//////* GAS - Gameplay Ability System *//////

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> GASAbilitySystemComponent = nullptr;

	UPROPERTY()
	TObjectPtr<UCombatAttributeSet> CombatAttributeSet;

	UFUNCTION(BlueprintCallable)
	void SetupGameplayAbilitySystemComponent();

	void SetUpInitialEffects();

	void InitializeAttributeSet();

	// Initial Gameplay Effects

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	// Initial Values for Combat Attribute Set

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	float InitialStunTime = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	float InitialDamagePower = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	float InitialKnockbackStrength = 0.0f;

public:	

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
