// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h" 
#include "ShieldActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetUpdate, FText, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWidgetColorUpdate, FLinearColor, NewColor);

UCLASS()
class CYBER_API AShieldActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShieldActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Mesh **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	/** Widget */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* ShieldWidget;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FOnWidgetUpdate OnWidgetUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_bUpdateWidget)
	FText WidgetText;

	UFUNCTION()
	void OnRep_bUpdateWidget();

	UPROPERTY(BlueprintAssignable)
	FOnWidgetColorUpdate OnWidgetColorUpdate;

	UPROPERTY(ReplicatedUsing = OnRep_bUpdateColor)
	FLinearColor Color;

	UFUNCTION()
	void OnRep_bUpdateColor();

public:	

	/* Replicate Color */
	UFUNCTION(BlueprintCallable, Category = "Shield Widget")
	void SetShieldText(FText Text);

	/* Replicate Color */
	UFUNCTION(BlueprintCallable, Category = "Shield Widget")
	void SetShieldColor(FLinearColor NewColor);
};
