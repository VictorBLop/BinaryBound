// Fill out your copyright notice in the Description page of Project Settings.

#include "CyberEnemy.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\AttributeSets/CombatAttributeSet.h"
#include "..\Cyber.h"
#include "Kismet/GameplayStatics.h"
#include "..\Core\CyberGameState.h"
#include "..\Core\CyberCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbility.h" 
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

FOnEnemyDied ACyberEnemy::OnEnemyDied;

// Sets default values
ACyberEnemy::ACyberEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GASAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	GASAbilitySystemComponent->SetIsReplicated(true);

	CyberAttributeSet = CreateDefaultSubobject<UCyberAttributeSet>(TEXT("GAS Attribute Set"));
	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("Combat Attribute Set"));

	GetCharacterMovement()->MaxWalkSpeed = 400.f;

	DefaultEnemyStats = FEnemySaveData(100.0f, 10.0f, 0.25f, 1000.0f);

	// Set Relevancy
	bAlwaysRelevant = true;
}

// Called when the game starts or when spawned
void ACyberEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetupMaterialColors();

		SetupGameplayAbilitySystemComponent();
	}
	else
	{
		/* For CLIENT */
		// Get Original Materials from Character Mesh and Set Custom Color defined in CharacterColorMaterials array.
		for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
		{
			// Add the actual Material to the OriginalMaterialInterfaces and CurrentMaterialInterfaces arrays. This is done so that 
			// when changing the materials in player feedback, it is possible to come back to the original materials.
			OriginalMaterialInterfaces.Add(GetMesh()->GetMaterial(index));
			CurrentMaterialInterfaces.Add(GetMesh()->GetMaterial(index));
		}
	}
}

void ACyberEnemy::SetCharacterMovement(bool Activate)
{
	if(Activate)
	{
		GetCharacterMovement()->Activate();
	}
	else
	{
		GetCharacterMovement()->Deactivate();
	}
}

// Called every frame
void ACyberEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


UAbilitySystemComponent* ACyberEnemy::GetAbilitySystemComponent() const
{
	return GASAbilitySystemComponent;
}

void ACyberEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ACyberEnemy::SetupGameplayAbilitySystemComponent()
{
	if (!GASAbilitySystemComponent)
	{
		return;
	}

	GASAbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSet(); // Initialize Attributes Set

	SetUpInitialEffects();

	// Apply Initial Effect only for one specific color
	// This applies the specific color gameplay effect to the Character.
	if (InitialSpecificColorEffect != nullptr)
	{
		GASAbilitySystemComponent->ApplyGameplayEffectToSelf
		(
			InitialSpecificColorEffect.GetDefaultObject(),
			// Specific Color GameplayEffect
			1.0f,
			// Level
			GASAbilitySystemComponent->MakeEffectContext()
		); // Effect context
	}
}

void ACyberEnemy::SetupMaterialColors()
{
	// Get Original Materials from Character Mesh and Set Custom Color defined in CharacterColorMaterials array.
	for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
	{
		// For the first material of all, let's change it to a custom color.
		if (index == 0)
		{
			bApplyColors = !bApplyColors; // 0 or 1 (specific color), 2 (no pre-assigned color)

			// First material of the character, set it to Custom color in Game Mode.
			OnRep_ApplyColors(); // Sets Material index 0 to CharacterColorMaterials[index]
		}

		// Add the actual Material to the OriginalMaterialInterfaces and CurrentMaterialInterfaces arrays. This is done so that 
		// when changing the materials in player feedback, it is possible to come back to the original materials.
		OriginalMaterialInterfaces.Add(GetMesh()->GetMaterial(index));
		CurrentMaterialInterfaces.Add(GetMesh()->GetMaterial(index));
	}
}


void ACyberEnemy::SetUpInitialEffects()
{
	// Apply Initial Gameplay Effects
	for (const TSubclassOf<UGameplayEffect>& effect : InitialEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_MaxHealth, DefaultEnemyStats.InitialMaxHealth);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_Health, DefaultEnemyStats.InitialMaxHealth);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_DamagePower_SetByCaller, DefaultEnemyStats.InitialDamagePower);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_StunTime_SetByCaller, DefaultEnemyStats.InitialStunTime);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_KnockbackStrength_SetByCaller, DefaultEnemyStats.InitialKnockbackStrength);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf
			(*Spec, FPredictionKey());
	}
}

void ACyberEnemy::SetUpInitialEffectsFromSpawner(TArray<FGameplayTag> GameplayTags, TArray<float> Magnitudes)
{
	// Apply Initial Gameplay Effects
	for (const TSubclassOf<UGameplayEffect>& effect : InitialEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		if (GameplayTags.Num() != Magnitudes.Num())
		{
			return;
		}

		for (int32 index = 0; index < GameplayTags.Num(); index++)
		{
			Spec->SetSetByCallerMagnitude(GameplayTags[index], Magnitudes[index]);
		}

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf
			(*Spec, FPredictionKey());
	}
}

void ACyberEnemy::OnRep_ApplyColors()
{
	// First material of the character, set it to Custom color in Game Mode.
	if (!bUseWholeMaterials) // Change only Color of the Material.
	{
		FLinearColor Color = EnemyColor;

		GetMesh()->SetVectorParameterValueOnMaterials(ColorVectorParameter, FVector(Color.R, Color.G, Color.B));

		OriginalColor = Color;

		CurrentColor = OriginalColor;
	}
	else // Change Complete Materials
	{
		GetMesh()->SetMaterial(0, CharacterColorMaterial);
	}
}

void ACyberEnemy::InitializeAttributeSet()
{
	if (GASAbilitySystemComponent.Get())
	{
		GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CyberAttributeSet);
		GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CombatAttributeSet);

		GASAbilitySystemComponent->ForceReplication();

		GASAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate
			(CyberAttributeSet->GetHealthAttribute()).AddUObject(this, &ACyberEnemy::HealthChanged);
	}
}

void ACyberEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bIsEnemyDead, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bReplicateCurrentMaterials, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bReplicateApplyDamageMaterial, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bApplyColors, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CurrentHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, CyberTimeDilation, COND_None, REPNOTIFY_Always);
}

void ACyberEnemy::ApplyDamagePlayerFeedback()
{
	/* Sets all materials in character mesh to be the DamageMaterialInterface, 
	* which is modifiable in Blueprints. */
	OnRep_DamageMaterial();

	// Change replicated variable so that 
	bReplicateApplyDamageMaterial = !bReplicateApplyDamageMaterial;

	FTimerHandle FeedbackTimerHandle;

	// Add some delay before ending the damage player feedback (editable in Blueprints).
	if (DelayForEnemyHitFeedback >= 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer
		(
			FeedbackTimerHandle,
			// handle to cancel timer at a later time
			this,
			// the owning object
			&ThisClass::EndDamagePlayerFeedback,
			// function to call on elapsed
			DelayForEnemyHitFeedback,
			// float delay until elapsed
			false
		); // looping
	}
}

void ACyberEnemy::OnRep_DamageMaterial()
{
	if (!bUseWholeMaterials) // Change only Color of the Material.
	{
		GetMesh()->SetVectorParameterValueOnMaterials
			(ColorVectorParameter, FVector(DamageColor.R, DamageColor.G, DamageColor.B));
	}
	else // Change Complete Materials
	{
		if (!DamageMaterialInterface->IsValidLowLevel())
		{
			return;
		}

		/* Sets all materials in character mesh to be the DamageMaterialInterface,
		* which is modifiable in Blueprints. */
		for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
		{
			GetMesh()->SetMaterial(index, DamageMaterialInterface);
		}
	}
}

void ACyberEnemy::EndDamagePlayerFeedback()
{
	/* Sets all materials in character mesh to be the Original Materials,
	* which is assigned in the SetupMaterialColors() function. */

	OnRep_CurrentMaterials();

	bReplicateCurrentMaterials = !bReplicateCurrentMaterials;
}

void ACyberEnemy::OnRep_CurrentMaterials()
{
	if (!bUseWholeMaterials) // Change only Color of the Material.
	{
		GetMesh()->SetVectorParameterValueOnMaterials
			(ColorVectorParameter, FVector(CurrentColor.R, CurrentColor.G, CurrentColor.B));
	}
	else // Change Complete Materials
	{
		/* Sets all materials in character mesh to be the CurrentMaterialInterface */
		for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
		{
			if (CurrentMaterialInterfaces.IsValidIndex(index))
			{
				GetMesh()->SetMaterial(index, CurrentMaterialInterfaces[index]);
			}
		}
	}
}

void ACyberEnemy::ColorSetup(UMaterialInterface* ColorMaterialToSet, FLinearColor ColorToSet)
{
	if (!bUseWholeMaterials)
	{
		CurrentColor = ColorToSet;
	}
	else
	{
		for (int32 index = 0; index < CurrentMaterialInterfaces.Num(); index++)
		{
			CurrentMaterialInterfaces[index] = ColorMaterialToSet;
		}
	}

	EndDamagePlayerFeedback();
}

void ACyberEnemy::ResetToOriginalColors()
{
	if (!bUseWholeMaterials)
	{
		CurrentColor = OriginalColor;
	}
	else
	{
		for (int32 index = 0; index < CurrentMaterialInterfaces.Num(); index++)
		{
			CurrentMaterialInterfaces[index] = OriginalMaterialInterfaces[index];
		}
	}

	EndDamagePlayerFeedback();
}

void ACyberEnemy::ApplyDamageEffect(AActor* HitActor)
{
	// Let's apply the Gameplay Effect to the character if it exists.
	if (ACyberCharacter* characterHit = Cast<ACyberCharacter>(HitActor))
	{
		if (UAbilitySystemComponent* abilitySystem = characterHit->GetAbilitySystemComponent())
		{
			// Check if DealDamageEffect exists.
			if (DealDamage && HasAuthority())
			{
				characterHit->PlayerGotHit(-CombatAttributeSet->GetDamagePower(), CombatAttributeSet->GetStunTime(), this);
			}
		}
	}
}

void ACyberEnemy::TriggerReaction(AActor* HitActor, AActor* InstigatorActor)
{
	if (GASAbilitySystemComponent.Get())
	{
		FGameplayEventData Payload;
		Payload.Target = this;
		Payload.Instigator = InstigatorActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Ability_Character_HitReact, Payload);
	}
}

void ACyberEnemy::HealthChanged(const FOnAttributeChangeData& Data)
{
	CurrentHealth = Data.NewValue;

	OnRep_CurrentHealth();

	// New value is Data.NewValue
	if (Data.NewValue <= 0.0f)
	{
		bIsEnemyDead = true;

		OnRep_IsEnemyDead();
	}
}

void ACyberEnemy::OnRep_CurrentHealth()
{
	OnHealthUpdate.Broadcast(CurrentHealth, CyberAttributeSet->GetMaxHealth());
}

void ACyberEnemy::OnRep_IsEnemyDead()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));

	if (GASAbilitySystemComponent)
	{
		for(TSubclassOf<UGameplayEffect> DeathEffect : DeathEffects)
		{
			GASAbilitySystemComponent->ApplyGameplayEffectToSelf
			(DeathEffect.GetDefaultObject(), 1.0f, FGameplayEffectContextHandle(), FPredictionKey());
		}
	}

	FTimerHandle DestroyTimer;

	GetWorld()->GetTimerManager().SetTimer
	(
		DestroyTimer,
		// handle to cancel timer at a later time
		this,
		// the owning object
		&ThisClass::DestroyEnemy,
		// function to call on elapsed
		TimeBeforeDestroyingEnemy,
		// float delay until elapsed
		false
	); // looping
}

void ACyberEnemy::DestroyEnemy()
{
	/*
	* TODO
	* Make the CyberGameState subscribe to OnEnemyDied to avoid SRP (Single Responsibility Principle) violation for
	* CyberEnemy Class. In other words CyberEnemy should not be responsible for increasing tokens of other class.
	* By subscribing to OnEnemyDied delegate you are using Observer pattern to adhere to SRP.
	 */
	if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CyberGameState->SetEnemyTokens();
	}
	
	if (HasAuthority())
	{
		OnEnemyDied.Broadcast(this);
	}
}

void ACyberEnemy::SetCyberTimeDilation(float NewTimeDilation)
{
	CyberTimeDilation = NewTimeDilation;

	if (HasAuthority())
	{
		OnRep_CyberTimeDilation();
	}
}

float ACyberEnemy::GetCyberTimeDilation()
{
	return CyberTimeDilation;
}

void ACyberEnemy::OnRep_CyberTimeDilation()
{
	CustomTimeDilation = CyberTimeDilation;
}
