// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberTraps.h"
#include "..\Cyber.h"
#include "..\AttributeSets/CombatAttributeSet.h"

// Sets default values
ACyberTraps::ACyberTraps()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GASAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("Ability System Component"));
	GASAbilitySystemComponent->SetIsReplicated(true);

	CombatAttributeSet = CreateDefaultSubobject<UCombatAttributeSet>(TEXT("Combat Attribute Set"));
}

// Called when the game starts or when spawned
void ACyberTraps::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetupGameplayAbilitySystemComponent();
	}
}

void ACyberTraps::SetupGameplayAbilitySystemComponent()
{
	if (!GASAbilitySystemComponent)
	{
		return;
	}

	GASAbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributeSet(); // Initialize Attributes Set

	SetUpInitialEffects();
}

void ACyberTraps::SetUpInitialEffects()
{	
	// Apply Initial Gameplay Effects
	for (const TSubclassOf<UGameplayEffect>& effect : InitialEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_DamagePower_SetByCaller, InitialDamagePower);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_StunTime_SetByCaller, InitialStunTime);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_KnockbackStrength_SetByCaller, InitialKnockbackStrength);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
	}
}

void ACyberTraps::InitializeAttributeSet()
{
	if (GASAbilitySystemComponent.Get())
	{
		GASAbilitySystemComponent->GetSpawnedAttributes_Mutable().AddUnique(CombatAttributeSet);

		GASAbilitySystemComponent->ForceReplication();
	}
}

UAbilitySystemComponent* ACyberTraps::GetAbilitySystemComponent() const
{
	return GASAbilitySystemComponent;
}

// Called every frame
void ACyberTraps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

