#include "CyberGameMode.h"
#include "CyberGameState.h"
#include "Kismet/GameplayStatics.h"

void ACyberGameMode::SetColorsIndex(int32 newColorsIndex)
{
	ColorsIndex = newColorsIndex;
}

int32 ACyberGameMode::GetColorsIndex()
{
	return ColorsIndex;
}

void ACyberGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ACyberGameState* CyberGameState = GetGameState<ACyberGameState>())
	{
		CyberGameState->PlayerJoined();
	}
}

void ACyberGameMode::StartPlay()
{
	Super::StartPlay();
	EnemySpawner = FindEnemySpawner();
}

void ACyberGameMode::ActivateEnemySpawner()
{
	if (EnemySpawner)
	{
		EnemySpawner->SetActiveState(true);
	}
	else
	{
		// Handle the case where EnemySpawner is null
		UE_LOG(LogTemp, Warning, TEXT("Unable to find EnemySpawner"));
	}
}

ACyberEnemySpawner* ACyberGameMode::FindEnemySpawner()
{
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACyberEnemySpawner::StaticClass());
	if (FoundActor)
	{
		return Cast<ACyberEnemySpawner>(FoundActor);
	}
	
	// Handle the case where no EnemySpawner is found
	UE_LOG(LogTemp, Warning, TEXT("No EnemySpawner found in the world"));
	return nullptr;
}
