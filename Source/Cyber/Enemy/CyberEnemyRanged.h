#pragma once

#include "CoreMinimal.h"
#include "CyberEnemyMelee.h"
#include "CyberEnemyRanged.generated.h"

/**
 * 
 */
UCLASS()
class CYBER_API ACyberEnemyRanged : public ACyberEnemy
{
	GENERATED_BODY()

protected:

	ACyberEnemyRanged();

	virtual void BeginPlay() override;

	/* Color Change */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor EnemyColorOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FLinearColor EnemyColorTwo;

	/*********** Weapon ***********/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMesh = nullptr;

	/*********** Projectile ***********/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* ProjectileReference = nullptr;

	/* Projectile Class */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	float ProjectileSpeed = 0.0f;

	/*********** Rotation to Target ***********/

	UPROPERTY(BlueprintReadWrite)
	float RotationTargetZ = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float RotationTargetY = 0.0f;

};
