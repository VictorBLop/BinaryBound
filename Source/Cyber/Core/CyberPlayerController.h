// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h" 
#include "CyberPlayerController.generated.h"

class ACyberPlayerState;

/**
 * 
 */
UCLASS()
class CYBER_API ACyberPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	ACyberPlayerController();
	UFUNCTION()
	void ApplyStun(bool Apply);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_PlayerState() override;

	/* Timer Elements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Timer")
	TSubclassOf<UUserWidget> PlayerInfoWidgetClass;

	UPROPERTY()
	UUserWidget* PlayerInfoWidget = nullptr;

	/* Indicator for Other Player position */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Indicator")
	TSubclassOf<UUserWidget> IndicatorWidgetClass;

	UPROPERTY()
	UUserWidget* IndicatorWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Indicator", meta = (AllowPrivateAccess = "true"))
	FVector2D OtherPlayerScreenPositionCPP = FVector2D::ZeroVector;

	/* Pre Game Timer for Time before starting the game */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Pre Game Timer")
	TSubclassOf<UUserWidget> PreGameTimerWidgetClass;

	UPROPERTY()
	UUserWidget* PreGameTimerWidget = nullptr;

	/* Game Over Elements */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Power")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY()
	UUserWidget* GameOverWidget = nullptr;

	/* Player State Reference */

	UPROPERTY(BlueprintReadOnly)
	ACyberPlayerState* CyberPlayerState;

public:

	void SetPlayerStateReference(ACyberPlayerState* playerState);
	
	void SetUpIndicatorforOtherPlayer();

	UFUNCTION()
	void ShowPreGameTimer();
	
	void RemovePreGameTimer();

	void ShowPlayerInfo();

	void RemovePlayerInfo();

	void ShowGameOverScreen();

	void RemoveGameOverWidget();

	void DeactivateInput();
};
