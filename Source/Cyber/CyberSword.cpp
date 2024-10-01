// Fill out your copyright notice in the Description page of Project Settings.

#include "CyberSword.h"
#include "AttributeSets/CombatAttributeSet.h"
#include "AttributeSets/ColorAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Core/CyberCharacter.h"
#include "Cyber.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Enemy/CyberEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
 

// Sets default values
ACyberSword::ACyberSword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize and attach the Sword Mesh
	SwordMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SwordMesh"));
	SetRootComponent(SwordMesh);

	SwordCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SwordCollider"));
	SwordCollider->SetupAttachment(SwordMesh);
	SwordCollider->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
	SwordCollider->SetCollisionObjectType(ECC_WorldStatic);
	SwordCollider->SetGenerateOverlapEvents(true);
	SwordCollider->SetSimulatePhysics(false);
	SwordCollider->SetMobility(EComponentMobility::Movable);
	SwordCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail VFX"));
	TrailVFX->AttachToComponent(SwordMesh, FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("")));


	if (HasAuthority())
	{
		SwordCollider->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSwordColliderBeginOverlap);
	}

}

void ACyberSword::TurnOnSwordCollision()
{
	TurnOnSwordCollider();
}

void ACyberSword::TurnOffSwordCollision()
{
	TurnOffSwordCollider();
}

void ACyberSword::SetActivateKnockback(bool ActivateKnockback)
{
	ApplyKnockback = ActivateKnockback;
}

void ACyberSword::SetSwordColor(FName ParameterName, FLinearColor SwordColor)
{
	SwordMesh->SetVectorParameterValueOnMaterials(ParameterName, FVector(SwordColor.R, SwordColor.G, SwordColor.B));
	
	TrailVFX->SetVariableLinearColor(TEXT("ColorTag"), SwordColor);
}

void ACyberSword::ActivateTrail()
{
	TrailVFX->Activate();
}

void ACyberSword::DeactivateTrail()
{
	TrailVFX->Deactivate();
}

// Called when the game starts or when spawned
void ACyberSword::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACyberSword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACyberSword::OnSwordColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("%s is the owner"), *GetOwner()->GetName());
	UE_LOG(LogTemp, Error, TEXT("%s is the other actor"), *OtherActor->GetName());

	if ((GetOwner()->IsA<ACyberCharacter>() && OtherActor->IsA<ACyberEnemy>()) || (GetOwner()->IsA<ACyberEnemy>() && OtherActor->IsA<ACyberCharacter>()))
	{
		// Check if the actor hit has already been hit, to avoid several hits
		// on the same Actor on only one attack.

		for (const TObjectPtr HitActorsIterator : HitActors)
		{
			if (OtherActor == HitActorsIterator)
			{
				return;
			}
		}

		// If this point has been reached, the Actor hit is unique in this attack.
		// This actor is added to the HitActors array.
		HitActors.AddUnique(OtherActor);

		ApplyDamageEffect(OtherActor);
	}
}

void ACyberSword::TurnOffSwordCollider()
{
	if (HasAuthority())
	{
		SwordCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	HitActors.Empty();

	SetActivateKnockback(false);
}

void ACyberSword::TurnOnSwordCollider()
{
	if (HasAuthority())
	{
		SwordCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void ACyberSword::ApplyDamageEffect(AActor* HitActor)
{
	// Let's apply the Gameplay Effect to the character if it exists.s
	if (HitActor->IsA<ACyberEnemy>())
	{
		if(IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(HitActor))
		{
			UAbilitySystemComponent* abilitySystem = ASI->GetAbilitySystemComponent();

			// Only if the effect is not successfully applied, call the Add Color ability, which will execute the Gameplay Cue 
			// and the Player Feedback if the Character Color is neutral.

			// Check if DealDamageEffect exists.
			if (DealDamageFromColorOne && DealDamageFromColorTwo)
			{
				if (!DealDamageOnEnemy(HitActor, abilitySystem))
				{
					if (TryChangeEnemyColor(HitActor, abilitySystem))
					{
						StunEnemy(HitActor, abilitySystem);
					}
				}
				else
				{
					if (ACyberEnemy* EnemyHit = Cast<ACyberEnemy>(HitActor))
					{
						EnemyHit->TriggerReaction(EnemyHit, GetOwner());

						StunEnemy(HitActor, abilitySystem);

						if (ApplyKnockback) // Apply Stun on attack combo.
						{
							KnockbackEnemy(HitActor, abilitySystem);
						}
					}					
				}
			}
		}
	}
	else if (HitActor->IsA<ACyberCharacter>())
	{
		if (ACyberEnemy* CyberEnemy = Cast<ACyberEnemy>(GetOwner()))
		{
			CyberEnemy->ApplyDamageEffect(HitActor);
		}
	}
}

bool ACyberSword::DealDamageOnEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem)
{
	TSubclassOf<UGameplayEffect> DealDamageToApply = nullptr;

	// Depending on the Character Color, a different Gameplay Effect is used to create the GameplayEffectSpec.
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{

		if (ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_Neutral)) // No Color.
		{
			DealDamageToApply = DealDamage;
		}
		else if (ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_One))
		{
			DealDamageToApply = DealDamageFromColorOne;
		}
		else if (ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_Two))
		{
			DealDamageToApply = DealDamageFromColorTwo;
		}
		else
		{
			return false;
		}

		const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(ASI->GetAbilitySystemComponent()->GetAttributeSet(UCombatAttributeSet::StaticClass()));
		if (!CombatAttributeSet)
		{
			return false;
		}

		return ApplyEffectSpec(enemyAbilitySystem, DealDamageToApply, GetOwner(), enemyHit, GameplayEffect_Damaged, -CombatAttributeSet->DamagePower.GetCurrentValue());
	}
	else
	{
		return false;
	}
}


bool ACyberSword::TryChangeEnemyColor(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem)
{
	// If the Character has any of these colors, ignore the rest of the function.
	if (enemyAbilitySystem->HasMatchingGameplayTag(Character_Color_One) || enemyAbilitySystem->HasMatchingGameplayTag(Character_Color_Two))
	{
		return false;
	}

	// AddColorOneEffect and AddColorTwoEffect are Gameplay Effects which add Color.One and Color.Two respectively
	// They must exist for the rest of the sequence to be executed properly.
	if (!AddColorOneEffect || !AddColorTwoEffect)
	{
		return false;
	}

	TSubclassOf<UGameplayEffect> AddColorGameplayEffect = nullptr;

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		if (ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_One))
		{
			AddColorGameplayEffect = AddColorTwoEffect; // In case of having Color.One, add Color.Two for the Enemy.
		}
		else if(ASI->GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_Two))
		{
			AddColorGameplayEffect = AddColorOneEffect; // In case of having Color.Two, add Color.One for the Enemy.
		}

		const UColorAttributeSet* ColorAttributeSet = Cast<UColorAttributeSet>(ASI->GetAbilitySystemComponent()->GetAttributeSet(UColorAttributeSet::StaticClass()));
		if (!ColorAttributeSet)
		{
			return false;
		}

		return ApplyEffectSpec(enemyAbilitySystem, AddColorGameplayEffect, GetOwner(), enemyHit, GameplayEffect_Initialize_MarkedTime_SetByCaller, ColorAttributeSet->MarkedTime.GetCurrentValue());
	}
	else
	{
		return false;
	}
}


bool ACyberSword::StunEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem)
{
	bool StunWasSuccessfullyApplied = false;

	if (StunEffect)
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
		{
			const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(ASI->GetAbilitySystemComponent()->GetAttributeSet(UCombatAttributeSet::StaticClass()));
			if (!CombatAttributeSet)
			{
				return false;
			}

			/*********** STUN ***********/

			StunWasSuccessfullyApplied = ApplyEffectSpec(enemyAbilitySystem, StunEffect, GetOwner(), enemyHit, GameplayEffect_Initialize_StunTime_SetByCaller, CombatAttributeSet->StunTime.GetCurrentValue());
		}
	}

	return StunWasSuccessfullyApplied;
}

void ACyberSword::KnockbackEnemy(AActor* enemyHit, UAbilitySystemComponent* enemyAbilitySystem)
{
	/*********** KNOCKBACK ***********/

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(ASI->GetAbilitySystemComponent()->GetAttributeSet(UCombatAttributeSet::StaticClass()));
		if (!CombatAttributeSet)
		{
			return;
		}

		FGameplayEventData KnockbackPayload;
		KnockbackPayload.Instigator = GetOwner();
		KnockbackPayload.Target = enemyHit;
		KnockbackPayload.EventMagnitude = CombatAttributeSet->KnockbackStrength.GetCurrentValue(); // EventMagnitude is the Knockback Strength.		

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(enemyHit, Ability_Character_Knockback, KnockbackPayload);
	}
}

bool ACyberSword::ApplyEffectSpec(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> Effect, AActor* InstigatorActor, AActor* EffectCauserActor, FGameplayTag SetByCallerTag, float Magnitude)
{
	if (ASC && Effect)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = ASC->MakeEffectContext();
		effectContext.AddInstigator(InstigatorActor, EffectCauserActor); // Add Character as Instigator and CharacterHit as Effect Causer

		FGameplayEffectSpec* Spec;
		Spec = new FGameplayEffectSpec(Effect.GetDefaultObject(), effectContext, 1.0f);

		// Apply Gameplay Effect with Set By Caller (Magnitude defined in float variable DamagePower)
		Spec->SetSetByCallerMagnitude(SetByCallerTag, Magnitude);

		//Apply Gameplay Effect Spec
		FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());

		return GameplayEffectHandle.WasSuccessfullyApplied();
	}
	else
	{
		return false;
	}
}