// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "CyberEnemy.h"
#include "..\CyberSword.h"
#include "..\Interfaces\SwordInterface.h"
#include "CyberEnemyMelee.generated.h"

class UCyberAttributeSet;
class UCombatAttributeSet;
class UBoxComponent;

// DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyValueUpdate, float, Value, float, MaxValue);

UCLASS()
class CYBER_API ACyberEnemyMelee : public ACyberEnemy, public ISwordInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACyberEnemyMelee();

	// UPROPERTY()
	// TObjectPtr<ACyberEnemySpawner> EnemySpawner = nullptr;

	/** Sword */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* CyberBatonComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ACyberSword* CyberBaton = nullptr;

protected:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_ApplySwordColor)
	bool bApplySwordColor = false;

	UFUNCTION()
	void OnRep_ApplySwordColor();

	/* Sword Interface */

	void TurnOnSwordCollision() override;

	void TurnOffSwordCollision() override;

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetSwordColor();

	/* Sword Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attack")
	TArray<FName> AnimationSections;

	UPROPERTY(BlueprintReadOnly, Category = "GAS - Attack")
	int32 ComboIndex = 0;

	UFUNCTION(BlueprintCallable)
	void IncreaseComboIndex();

	FTimerHandle SwordEventTimer;

	TArray<TObjectPtr<AActor>> HitActors;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
