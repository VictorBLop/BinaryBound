#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BillboardComponent.h"
#include "CyberEnemySpawnpoint.generated.h"

class ACyberEnemy;

USTRUCT(BlueprintType)
struct FEnemySpawnInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<ACyberEnemy> EnemyType  = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int EnemyCount = 0;
};

USTRUCT(BlueprintType)
struct FWave
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<FEnemySpawnInfo> EnemiesToSpawn;

	int GetEnemiesInWaveCount() const
	{
		int TotalCount = 0;

		for (FEnemySpawnInfo EnemyInfo : EnemiesToSpawn)
		{
			TotalCount += EnemyInfo.EnemyCount;
		} 
		return TotalCount;
	}
	
};

UCLASS()
class CYBER_API ACyberEnemySpawnpoint : public AActor
{
	GENERATED_BODY()
	
public:
	ACyberEnemySpawnpoint();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnInfo")
	UBillboardComponent* BillboardComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnInfo")
	TArray<FWave> Waves;
};
