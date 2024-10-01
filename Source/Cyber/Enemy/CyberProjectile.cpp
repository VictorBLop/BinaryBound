// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberProjectile.h"
#include "..\Core\CyberCharacter.h"
#include "..\Cyber.h"
#include "CyberEnemyRanged.h"
#include "..\AttributeSets/CombatAttributeSet.h"
#include "Components/SphereComponent.h" 
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ACyberProjectile::ACyberProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Collider"));
	ProjectileCollider->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
	ProjectileCollider->SetCollisionObjectType(ECC_WorldStatic);
	ProjectileCollider->SetGenerateOverlapEvents(true);
	ProjectileCollider->SetSimulatePhysics(false);
	ProjectileCollider->SetMobility(EComponentMobility::Movable);
	ProjectileCollider->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	ProjectileCollider->SetEnableGravity(false);
	SetRootComponent(ProjectileCollider);

	// Initialize and attach the Sword Mesh
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	ProjectileMesh->SetEnableGravity(false);
	ProjectileMesh->SetSimulatePhysics(false);
	ProjectileMesh->SetupAttachment(ProjectileCollider);

	// Initialize Niagara System
	ProjectileVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Projectile VFX"));
	ProjectileVFX->SetupAttachment(ProjectileMesh);

	// Initialize Projectile Movement Component
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Component"));

	bReplicates = true;
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ACyberProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ProjectileCollider->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
		ProjectileCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileBeginOverlap);
	}
}

void ACyberProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Overlap.
}

void ACyberProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || !GetOwner())
	{
		return;
	}

	if (OtherActor->IsA<ACyberCharacter>())
	{
		if (GetOwner()->IsA<ACyberEnemyRanged>())
		{
			ACyberEnemyRanged* ShooterEnemy = Cast<ACyberEnemyRanged>(GetOwner());

			if (ShooterEnemy)
			{
				ShooterEnemy->ApplyDamageEffect(OtherActor);

				ProjectileVFX->Activate();

				Destroy();
			}
		}
	}
}

// Called every frame
void ACyberProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

