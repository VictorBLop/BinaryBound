// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "Components/WidgetComponent.h" 
#include "MarkInArrayActor.generated.h"

class USphereComponent;

UCLASS()
class CYBER_API AMarkInArrayActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMarkInArrayActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Collider **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USphereComponent* Collider;

	/** Mesh **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** Widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* MarkZoneWidget;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS - Mark in Array Effect")
	TSubclassOf<UGameplayEffect> ColorOneEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS - Mark in Array Effect")
	TSubclassOf<UGameplayEffect> ColorTwoEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS - Mark in Array Effect")
	TSubclassOf<UGameplayEffect> RemoveAllColorsEffect = nullptr;

	/* Color Replication */

	UPROPERTY(ReplicatedUsing = OnRep_Color)
	FLinearColor Color = FLinearColor(0.0f, 0.0f, 0.0f, 1.0f);

	UFUNCTION()
	void OnRep_Color();

	/* Widget Replication */

	UFUNCTION(BlueprintCallable)
	UWidgetComponent* GetMarkZoneWidget();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UStaticMeshComponent* GetMesh();

	/* Replicate Color */
	UFUNCTION(BlueprintCallable)
	void ChangeColor(FLinearColor NewColor);

};
