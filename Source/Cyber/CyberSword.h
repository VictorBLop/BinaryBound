// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Interfaces/SwordInterface.h"
#include "Interfaces/ColorInterface.h"
#include "CyberSword.generated.h"

class UBoxComponent;
class ACyberEnemy;
class UNiagaraComponent;

UCLASS()
class CYBER_API ACyberSword : public AActor, public ISwordInterface, public IColorInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACyberSword();
	
protected:

	/** Sword Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SwordMesh;

	/** Sword Collision Box */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* SwordCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* TrailVFX;

protected:

	/* Sword Interface*/

	void TurnOnSwordCollision() override;

	void TurnOffSwordCollision() override;

	void SetActivateKnockback(bool ActivateKnockback) override;

	virtual void SetSwordColor(FName ParameterName, FLinearColor SwordColor) override;

	/* Color Interface */

	void ActivateTrail() override;

	void DeactivateTrail() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Sword Elements */

	UFUNCTION(BlueprintCallable)
	void ApplyDamageEffect(AActor* HitActor);

	UFUNCTION(BlueprintCallable)
	bool DealDamageOnEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem);

	UFUNCTION(BlueprintCallable)
	bool TryChangeEnemyColor(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem);

	UFUNCTION(BlueprintCallable)
	bool StunEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem);

	UFUNCTION(BlueprintCallable)
	void KnockbackEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem);

	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<AActor>> HitActors;

	UFUNCTION()
	void OnSwordColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Deal Damage Effect

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> DealDamageFromColorOne = nullptr;

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> DealDamageFromColorTwo = nullptr;

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> DealDamage = nullptr;

	// Add Color Tags Gameplay Effects

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	TSubclassOf<UGameplayEffect> AddColorOneEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	TSubclassOf<UGameplayEffect> AddColorTwoEffect = nullptr;

	// Stun Effect

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> StunEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	bool StunOnFirstHit = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/* Generic Apply Effect Method (to be called only from server) */
	bool ApplyEffectSpec(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> Effect, AActor* InstigatorActor, AActor* EffectCauserActor, FGameplayTag SetByCallerTag, float Magnitude);

	// Sword Colliders

	UFUNCTION(BlueprintCallable)
	void TurnOffSwordCollider();

	UFUNCTION(BlueprintCallable)
	void TurnOnSwordCollider();

	// Knockback

	UPROPERTY(BlueprintReadOnly)
	bool ApplyKnockback = false;

	// Sword Color

	//void SetSwordColor(FName VectorParameter, FLinearColor SwordColor);
};
