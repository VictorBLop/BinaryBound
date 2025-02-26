#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "..\Enemy\CyberEnemySpawner.h"
#include "CyberGameMode.generated.h"

class ACyberPlayerController;

UCLASS()
class CYBER_API ACyberGameMode : public AGameMode
{
	GENERATED_BODY()

public:

	/* Color Elements */
	void SetColorsIndex(int32 newColorsIndex);

	int32 GetColorsIndex();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void ActivateEnemySpawner();

	ACyberEnemySpawner* EnemySpawner = nullptr;

	/* Local Multiplayer Widgets */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Local Multiplayer")
	TArray<TSubclassOf<UUserWidget>> LocalPlayerInfoWidgetClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget | Local Multiplayer")
	TSubclassOf<UUserWidget> SharedInfoWidgetClass;

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetLocalPlayerWidgetClass(AController* controller);

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetSharedInfoWidgetClass();

protected:

	int32 ColorsIndex = 0;
	
	virtual void StartPlay() override;

	ACyberEnemySpawner* FindEnemySpawner();

	int32 PlayerIndex = 0;

};
