#include "CyberEnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "..\Interfaces/Persistence.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\AttributeSets/CombatAttributeSet.h"
#include "..\Cyber.h"
#include "..\CyberSaveGame.h"
#include "..\Core\CyberGameState.h"
#include "CyberEnemy.h"
#include "..\Core\CyberGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "CyberEnemyRanged.h"

ACyberEnemySpawner::ACyberEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	/* Ability System Component */
	GASAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	GASAbilitySystemComponent->SetIsReplicated(true);

	CyberAttributeSet = CreateDefaultSubobject<UCyberAttributeSet>(TEXT("GAS Attribute Set"));
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("Combat Attribute Set"));

	bReplicates = true;
}

TArray<ACyberEnemy*> ACyberEnemySpawner::GetAliveEnemies()
{
	return Enemies;
}

void ACyberEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	ACyberEnemy::OnEnemyDied.AddDynamic(this, &ThisClass::DestroyEnemy);
	
	Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld()))->OnGameOver.AddUniqueDynamic(this, &ThisClass::ResetEnemySpawner);

	FindEnemySpawnpoints();
	WaveCount = GetWaveCount();
	
	if (UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance()))
	{
		CyberGameInstance->OnGameLoaded.AddDynamic(this, &ThisClass::OnGameLoadedEvent);
		CyberGameInstance->OnGameSaved.AddDynamic(this, &ThisClass::OnGameSavedEvent);
	}

	if (HasAuthority())
	{
		SetupGameplayAbilitySystemComponent();
	}
}

void ACyberEnemySpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, NumberOfEnemies, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bWaveChanged, COND_None, REPNOTIFY_Always);
}

int ACyberEnemySpawner::GetWaveCount()
{
	int maxWaveCount = 0;
	for (ACyberEnemySpawnpoint* Spawnpoint : Spawnpoints)
	{
		if (Spawnpoint->Waves.Num() > maxWaveCount)
		{
			maxWaveCount = Spawnpoint->Waves.Num();
		}
	}
	return maxWaveCount;
}

void ACyberEnemySpawner::DestroyEnemy(ACyberEnemy* Enemy)
{
	if (Enemy && Enemies.Contains(Enemy))
	{
		Enemies.Remove(Enemy);
		Enemy->Destroy();
		
		NumberOfEnemies = Enemies.Num();
		OnRep_EnemiesChanged();

		if (Enemies.IsEmpty())
		{
			bWaveChanged = !bWaveChanged;
			OnRep_WaveChanged();
		}
	}
}

void ACyberEnemySpawner::DestroyAllEnemies()
{
	for (ACyberEnemy* Enemy : Enemies)
	{
		if (Enemy)
		{
			Enemy->Destroy();
		}
	}
	Enemies.Empty();
}

void ACyberEnemySpawner::ResetEnemySpawner()
{
	CurrentWaveIndex = 0;
	DestroyAllEnemies();
	SetActiveState(false);
}

void ACyberEnemySpawner::SetActiveState(bool activated)
{
	IsSpawnerActive = activated;
}

void ACyberEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CanSpawnNextWave())
	{
		for (ACyberEnemySpawnpoint* CyberEnemySpawnpoint : Spawnpoints)
		{
			if (CyberEnemySpawnpoint->Waves.IsEmpty() || CyberEnemySpawnpoint->Waves.Num() < CurrentWaveIndex + 1) continue;
			
			TArray<FEnemySpawnInfo> EnemiesToSpawn = CyberEnemySpawnpoint->Waves[CurrentWaveIndex].EnemiesToSpawn;
			
			for (FEnemySpawnInfo EnemySpawnInfo : EnemiesToSpawn)
			{
				SpawnEnemy(CyberEnemySpawnpoint->GetActorLocation(), EnemySpawnInfo.EnemyType);
			}

			NumberOfEnemies = Enemies.Num();
			OnRep_EnemiesChanged();
		}

		CurrentWaveIndex++;
	}
}

bool ACyberEnemySpawner::CanSpawnNextWave()
{
	if (!IsSpawnerActive) return false;

	if (CurrentWaveIndex + 1 > WaveCount) return false;

	return Enemies.Num() == 0;
}

void ACyberEnemySpawner::SpawnEnemy(FVector location, TSubclassOf<ACyberEnemy> enemy)
{
	FHitResult hitGroundResult = LineTraceToGround(location);

	if (hitGroundResult.bBlockingHit)
	{
		FVector SpawnLocation = hitGroundResult.ImpactPoint;

		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ACyberEnemy* Enemy = GetWorld()->SpawnActor<ACyberEnemy>
		(
			enemy,
			SpawnLocation + FVector(0, 0, 50.0f),
			FRotator::ZeroRotator,
			SpawnInfo
		);

		if (Enemy)
		{
			if (HasAuthority())
			{
				if (Enemy->IsA<ACyberEnemyRanged>())
				{
					SetUpEnemyInitialEffects(Enemy, EnemyType::Ranged);
				}
				else if (Enemy->IsA<ACyberEnemyMelee>())
				{
					SetUpEnemyInitialEffects(Enemy, EnemyType::Melee);
				}

				ApplyMultiplierEffect(CurrentWaveIndex, Enemy);
			}

			Enemies.Add(Enemy);
		}
	}
}

FHitResult ACyberEnemySpawner::LineTraceToGround(FVector location)
{
	FHitResult HitResult;
	FVector TraceStart = location + FVector(0, 0, 1500);
	FVector TraceEnd = location - FVector(0, 0, 3000);
	FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), false, this);

	GetWorld()->LineTraceSingleByObjectType
	(
		HitResult,
		TraceStart,
		TraceEnd,
		FCollisionObjectQueryParams::AllStaticObjects,
		TraceParams
	);

	return HitResult;
}

void ACyberEnemySpawner::FindEnemySpawnpoints()
{
	TArray<AActor*> FoundSpawnpointActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACyberEnemySpawnpoint::StaticClass(), FoundSpawnpointActors);

	for (AActor* Actor : FoundSpawnpointActors)
	{
		Spawnpoints.Add(Cast<ACyberEnemySpawnpoint>(Actor));
	}
}

void ACyberEnemySpawner::SaveData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (saveGameRef)
	{
		// Melee Enemy
		saveGameRef->MeleeEnemiesSaveData.InitialMaxHealth = MeleeEnemySaveData.InitialMaxHealth; // Health

		saveGameRef->MeleeEnemiesSaveData.InitialDamagePower = MeleeEnemySaveData.InitialDamagePower; // Combat
		saveGameRef->MeleeEnemiesSaveData.InitialStunTime = MeleeEnemySaveData.InitialStunTime; // Combat
		saveGameRef->MeleeEnemiesSaveData.InitialKnockbackStrength = MeleeEnemySaveData.InitialKnockbackStrength; // Combat

		// Ranged Enemy
		saveGameRef->RangedEnemiesSaveData.InitialMaxHealth = RangedEnemySaveData.InitialMaxHealth; // Health

		saveGameRef->RangedEnemiesSaveData.InitialDamagePower = RangedEnemySaveData.InitialDamagePower; // Combat
		saveGameRef->RangedEnemiesSaveData.InitialStunTime = RangedEnemySaveData.InitialStunTime; // Combat
		saveGameRef->RangedEnemiesSaveData.InitialKnockbackStrength = RangedEnemySaveData.InitialKnockbackStrength; // Combat
	}
}

void ACyberEnemySpawner::LoadData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (saveGameRef)
	{
		// Melee Enemy

		MeleeEnemySaveData.InitialMaxHealth = saveGameRef->MeleeEnemiesSaveData.InitialMaxHealth; // Health
			
		MeleeEnemySaveData.InitialDamagePower = saveGameRef->MeleeEnemiesSaveData.InitialDamagePower; // Combat
		MeleeEnemySaveData.InitialStunTime = saveGameRef->MeleeEnemiesSaveData.InitialStunTime; // Combat
		MeleeEnemySaveData.InitialKnockbackStrength = saveGameRef->MeleeEnemiesSaveData.InitialKnockbackStrength; // Combat

		// Ranged Enemy

		RangedEnemySaveData.InitialMaxHealth = saveGameRef->RangedEnemiesSaveData.InitialMaxHealth; // Health

		RangedEnemySaveData.InitialDamagePower = saveGameRef->RangedEnemiesSaveData.InitialDamagePower; // Combat
		RangedEnemySaveData.InitialStunTime = saveGameRef->RangedEnemiesSaveData.InitialStunTime; // Combat
		RangedEnemySaveData.InitialKnockbackStrength = saveGameRef->RangedEnemiesSaveData.InitialKnockbackStrength; // Combat


		// Set Up Initial Effects on Enemies when game is loaded.
		for (ACyberEnemy* Enemy : Enemies)
		{
			if (Enemy->IsA<ACyberEnemyRanged>())
			{
				SetUpEnemyInitialEffects(Enemy, EnemyType::Ranged);
			}
			else if (Enemy->IsA<ACyberEnemyMelee>())
			{
				SetUpEnemyInitialEffects(Enemy, EnemyType::Melee);
			}
		}
	}
}

void ACyberEnemySpawner::OnGameLoadedEvent(UCyberSaveGame* saveGameRef)
{
	// Request Load to Game Instance.
	if (IPersistence* persistence = Cast<IPersistence>(GetGameInstance()))
	{
		persistence->RequestLoad(this);
	}

	SetUpInitialEffects();
}

void ACyberEnemySpawner::OnGameSavedEvent()
{
	// Do something on Save Game if necessary.
}

UAbilitySystemComponent* ACyberEnemySpawner::GetAbilitySystemComponent() const
{
	return GASAbilitySystemComponent;
}

int32 ACyberEnemySpawner::GetNumberOfEnemies()
{
	return NumberOfEnemies;
}

void ACyberEnemySpawner::SetupGameplayAbilitySystemComponent()
{
	if (!GASAbilitySystemComponent)
	{
		return;
	}

	GASAbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSet(); // Initialize Attributes Set

	SetUpInitialEffects();
}

void ACyberEnemySpawner::SetUpInitialEffects()
{
	InitializingAttributes = true;

	// Apply Initial Gameplay Effects
	for (const TSubclassOf<UGameplayEffect>& effect : InitialEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_MaxHealth, MeleeEnemySaveData.InitialMaxHealth);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_Health, MeleeEnemySaveData.InitialMaxHealth);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_DamagePower_SetByCaller, MeleeEnemySaveData.InitialDamagePower);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_StunTime_SetByCaller, MeleeEnemySaveData.InitialStunTime);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*Spec,
			FPredictionKey()
		);
	}

	InitializingAttributes = false;
}

void ACyberEnemySpawner::InitializeAttributeSet()
{
	if (GASAbilitySystemComponent.Get())
	{
		GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CyberAttributeSet);
		GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CombatAttributeSet);

		GASAbilitySystemComponent->ForceReplication();

		// Health
		GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CyberAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

		// Enemy Tokens
		GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetDamagePowerAttribute()).AddUObject(this, &ThisClass::OnDamagePowerChanged);

		// Cycle Tokens
		GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetStunTimeAttribute()).AddUObject(this, &ThisClass::OnStunTimeChanged);

		// Cycle Tokens
		GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(CombatAttributeSet->GetKnockbackStrengthAttribute()).AddUObject(this, &ThisClass::OnKnockbackStrenghtChanged);
	}
}

void ACyberEnemySpawner::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!InitializingAttributes)
	{
		MeleeEnemySaveData.InitialMaxHealth = MeleeEnemySaveData.InitialMaxHealth + Data.NewValue - Data.OldValue;
	}
}

void ACyberEnemySpawner::OnDamagePowerChanged(const FOnAttributeChangeData& Data)
{
	if(!InitializingAttributes)
	{
		MeleeEnemySaveData.InitialDamagePower = MeleeEnemySaveData.InitialDamagePower + Data.NewValue - Data.OldValue;
		RangedEnemySaveData.InitialDamagePower = RangedEnemySaveData.InitialDamagePower + Data.NewValue - Data.OldValue;
	}
}

void ACyberEnemySpawner::OnStunTimeChanged(const FOnAttributeChangeData& Data)
{
	if (!InitializingAttributes)
	{
		MeleeEnemySaveData.InitialStunTime = MeleeEnemySaveData.InitialStunTime + Data.NewValue - Data.OldValue;
		RangedEnemySaveData.InitialStunTime = RangedEnemySaveData.InitialStunTime + Data.NewValue - Data.OldValue;
	}

}

void ACyberEnemySpawner::OnKnockbackStrenghtChanged(const FOnAttributeChangeData& Data)
{
	if (!InitializingAttributes)
	{
		MeleeEnemySaveData.InitialKnockbackStrength = MeleeEnemySaveData.InitialKnockbackStrength + Data.NewValue - Data.OldValue;
		RangedEnemySaveData.InitialKnockbackStrength = RangedEnemySaveData.InitialKnockbackStrength + Data.NewValue - Data.OldValue;
	}
}

void ACyberEnemySpawner::SetUpEnemyInitialEffects(ACyberEnemy* enemy, EnemyType enemyType)
{
	FEnemySaveData EnemyData;

	if (enemyType == EnemyType::Melee)
	{
		EnemyData = MeleeEnemySaveData;
	}
	else if(enemyType == EnemyType::Ranged)
	{
		EnemyData = RangedEnemySaveData;
	}
	else
	{
		EnemyData = MeleeEnemySaveData;
	}

	InitialAttributesSetByCallerTags.Empty();
	InitialAttributesMagnitudes.Empty();

	for (AttributeTypes attributeType : ListOfAttributeTypes)
	{
		InitialAttributesSetByCallerTags.Add(GetAttributeTag(attributeType));
		InitialAttributesMagnitudes.Add(GetAttributeMagnitude(attributeType, EnemyData));
	}

	enemy->SetUpInitialEffectsFromSpawner(InitialAttributesSetByCallerTags, InitialAttributesMagnitudes);
}

void ACyberEnemySpawner::ApplyMultiplierEffect(int32 WaveIndex, ACyberEnemy* enemy)
{
	// Create Context Handle from characterHit Ability System Component
	FGameplayEffectContextHandle effectContext = enemy->GetAbilitySystemComponent()->MakeEffectContext();
	FGameplayEffectSpec* Spec = new FGameplayEffectSpec(MultiplierEffect.GetDefaultObject(), effectContext, 1.0f);

	// In Spec, using Set By Caller, initialize the values.
	float CurrentWaveMultiplier = 1.0f;

	if (WaveMultiplier.IsValidIndex(WaveIndex))
	{
		CurrentWaveMultiplier = WaveMultiplier[WaveIndex];
	}

	Spec->SetSetByCallerMagnitude(GameplayEffect_WaveMultiplier_SetByCaller, CurrentWaveMultiplier);

	//Apply Gameplay Effect Spec
	FActiveGameplayEffectHandle GameplayEffectHandle = enemy->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(
		*Spec,
		FPredictionKey()
	);
}

bool ACyberEnemySpawner::ApplyGameplayEffect(
	TSubclassOf<UGameplayEffect> effect,
	FGameplayTag SetByCallerTag,
	float Magnitude
)
{
	if (HasAuthority())
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*Spec,
			FPredictionKey()
		);

		return GameplayEffectHandle.WasSuccessfullyApplied();
	}
	else
	{
		ServerRPC_ApplyGameplayEffect(effect, SetByCallerTag, Magnitude);

		return false;
	}
}

void ACyberEnemySpawner::OnRep_EnemiesChanged()
{
	OnEnemiesUpdate.Broadcast(NumberOfEnemies);
}

void ACyberEnemySpawner::OnRep_WaveChanged()
{
	OnWaveCompleted.Broadcast();

	if (CurrentWaveIndex == WaveCount)
	{
		OnGameWin.Broadcast();
	}
}

FGameplayTag ACyberEnemySpawner::GetAttributeTag(AttributeTypes AttributeType)
{
	switch (AttributeType) 
	{
	case AttributeTypes::MaxHealth:

		return GameplayEffect_Initialize_MaxHealth;
		break;

	case AttributeTypes::Health:

		return GameplayEffect_Initialize_Health;
		break;

	case AttributeTypes::Damage:

		return GameplayEffect_Initialize_DamagePower_SetByCaller;
		break;

	case AttributeTypes::StunTime:

		return GameplayEffect_Initialize_StunTime_SetByCaller;
		break;

	case AttributeTypes::KnockbackStrength:

		return GameplayEffect_Initialize_KnockbackStrength_SetByCaller;
		break;

	default:

		return FGameplayTag();
	}
}

float ACyberEnemySpawner::GetAttributeMagnitude(AttributeTypes AttributeType, FEnemySaveData EnemySaveData)
{
	switch (AttributeType)
	{
	case AttributeTypes::MaxHealth:

		return EnemySaveData.InitialMaxHealth;
		break;

	case AttributeTypes::Health:

		return EnemySaveData.InitialMaxHealth;
		break;

	case AttributeTypes::Damage:

		return EnemySaveData.InitialDamagePower;
		break;

	case AttributeTypes::StunTime:

		return EnemySaveData.InitialStunTime;
		break;

	case AttributeTypes::KnockbackStrength:

		return EnemySaveData.InitialKnockbackStrength;
		break;

	default:

		return 1.0f;
	}
}

void ACyberEnemySpawner::ServerRPC_ApplyGameplayEffect_Implementation(
	TSubclassOf<UGameplayEffect> effect,
	FGameplayTag SetByCallerTag,
	float Magnitude
)
{
	// Create Context Handle from characterHit Ability System Component
	FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
	FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

	// In Spec, using Set By Caller, initialize the values.
	Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

	//Apply Gameplay Effect Spec
	FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
		*Spec,
		FPredictionKey()
	);
}
