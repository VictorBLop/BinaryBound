#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "..\Enemy\CyberEnemySpawner.h"
#include "CyberGameMode.generated.h"

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
	
protected:

	int32 ColorsIndex = 0;
	
	virtual void StartPlay() override;

	ACyberEnemySpawner* FindEnemySpawner();
};
