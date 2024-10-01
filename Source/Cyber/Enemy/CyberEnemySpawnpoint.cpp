#include "CyberEnemySpawnpoint.h"

ACyberEnemySpawnpoint::ACyberEnemySpawnpoint()
{
	PrimaryActorTick.bCanEverTick = false;
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	RootComponent = BillboardComponent;
}

void ACyberEnemySpawnpoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACyberEnemySpawnpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
