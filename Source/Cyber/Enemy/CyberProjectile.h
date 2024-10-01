// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h" 
#include "GameplayEffect.h"
#include "CyberProjectile.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class CYBER_API ACyberProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACyberProjectile();

	/** Projectile Collider */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USphereComponent* ProjectileCollider;

	/** Projectile Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	/** VFX */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ProjectileVFX;

	/* Projectile Movement Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Projectile")
	float ProjectileInitialSpeed = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
