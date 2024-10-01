// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberEnemyRanged.h"

ACyberEnemyRanged::ACyberEnemyRanged()
{
	// Initialize and attach the Sword Mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), FName("hand_r"));
	WeaponMesh->AddSocketOverride(WeaponMesh->GetAttachSocketName(), FName("hand_r"), true);

	ProjectileReference = CreateDefaultSubobject<UChildActorComponent>(TEXT("Projectile Reference"));
	ProjectileReference->SetupAttachment(WeaponMesh, FName(TEXT("Ammo")));
	ProjectileReference->SetChildActorClass(ProjectileClass);
	ProjectileReference->SetupAttachment(WeaponMesh);
	ProjectileSpeed = 800.0f;

}

void ACyberEnemyRanged::BeginPlay()
{
	Super::BeginPlay();
}
