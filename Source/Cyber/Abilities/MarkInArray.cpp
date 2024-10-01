// Fill out your copyright notice in the Description page of Project Settings.

#include "MarkInArray.h"
#include "..\Cyber.h"
#include "..\Core\CyberCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayEffect.h"
#include "Components/DecalComponent.h" 
#include "MarkInArrayActor.h"

UMarkInArray::UMarkInArray()
{
	AbilityTags.AddTag(SpecialAbility_Character_MarkInArray);

	// Add Trigger Data
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SpecialAbility_Character_MarkInArray;
	AbilityTriggers.Add(TriggerData);

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UMarkInArray::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!MarkArrayActorClass || !MarkInArrayUIEffect )
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CustomAbilityLevel = TriggerEventData->EventMagnitude;

	/* Spawn Actor */

	FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	FRotator Rotation = GetAvatarActorFromActorInfo()->GetActorRotation();
	
	// Set Scale multiplier depending on the Ability Level.
	int32 index = UKismetMathLibrary::FTrunc(CustomAbilityLevel) - 1;
	float ScaleMultiplier = SphereRadiuses[index] / SphereRadiuses[0];
	FVector Scale = FVector(ScaleMultiplier, ScaleMultiplier, ScaleMultiplier);

	FActorSpawnParameters SpawnInfo;
	
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnInfo.TransformScaleMethod = ESpawnActorScaleMethod::OverrideRootScale;
	SpawnInfo.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnInfo.Owner = GetAvatarActorFromActorInfo();

	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(Rotation.Quaternion());
	Transform.SetScale3D(Scale);

	/* Spawn Actor */
	MarkingActor = GetWorld()->SpawnActor<AActor>(
		MarkArrayActorClass,
		Transform,
		SpawnInfo
	);

	/* Change Color of Actor to Player's color */
	CyberCharacter = Cast<ACyberCharacter>(GetAvatarActorFromActorInfo());

	if (AMarkInArrayActor* MarkInArrayActor = Cast<AMarkInArrayActor>(MarkingActor))
	{
		if (CyberCharacter)
		{
			MarkInArrayActor->ChangeColor(CyberCharacter->GetCharacterColor());
		}
	};

	// Override Scale after Spawning
	MarkingActor->SetActorScale3D(Scale);

	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);

	bool WasSuccessfullyAttached = MarkingActor->AttachToActor(GetAvatarActorFromActorInfo(), AttachmentRules, TEXT(""));

	if (!WasSuccessfullyAttached)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}

	/* Effects */

	// Apply Gameplay Effect on Player during Special Ability duration to keep track of the special ability.

	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetAvatarActorFromActorInfo()))
	{
		FGameplayEffectContextHandle EffectContext = ASI->GetAbilitySystemComponent()->MakeEffectContext();

		FGameplayEffectSpec* Spec;

		Spec = new FGameplayEffectSpec(MarkInArrayUIEffect.GetDefaultObject(), EffectContext, 1.0f);
		
		Spec->SetSetByCallerMagnitude(GameplayEffect_MarkInArray_SetByCaller, MarkArrayEffectDuration);

		FActiveGameplayEffectHandle EffectHandle = ASI->GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());

		if (!EffectHandle.WasSuccessfullyApplied())
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}
	}

	/* Wait Gameplay Tags to be Added - GameplayEffect_MarkInArray is added to Player to mark the start of the ability */

	WaitAddTagTask = UAbilityTask_WaitGameplayTagAdded::WaitGameplayTagAdd(this, GameplayEffect_MarkInArray, GetAvatarActorFromActorInfo(), false);
	WaitAddTagTask->Added.AddDynamic(this, &ThisClass::OnMarkInArrayEffectApplied);
	WaitAddTagTask->ReadyForActivation();

	CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, false, nullptr);

}

void UMarkInArray::OnMarkInArrayEffectApplied()
{
	/* Wait Gameplay Tags to be Removed - GameplayEffect_MarkInArray is removed from Player to mark the end of the ability */

	WaitRemoveTagTask = UAbilityTask_WaitGameplayTagRemoved::WaitGameplayTagRemove(this, GameplayEffect_MarkInArray, GetAvatarActorFromActorInfo(), false);
	WaitRemoveTagTask->Removed.AddDynamic(this, &ThisClass::OnMarkInArrayEffectRemoved);
	WaitRemoveTagTask->ReadyForActivation();
}

void UMarkInArray::OnMarkInArrayEffectRemoved()
{
	if (MarkingActor)
	{
		MarkingActor->Destroy();
	}


	WaitAddTagTask->Added.RemoveAll(this);
	WaitRemoveTagTask->Removed.RemoveAll(this);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}


void UMarkInArray::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (MarkingActor)
	{
		MarkingActor->Destroy();
	}
}

const FGameplayTagContainer* UMarkInArray::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change (moved to a different slot)
	const FGameplayTagContainer* ParentTags = Super::GetCooldownTags();
	if (ParentTags)
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);
	return MutableTags;
}

void UMarkInArray::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		// Set Level as AbilityLevel, which will be the EventMagnitude in the Payload when triggering the Ability from Gameplay Event.
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), CustomAbilityLevel);

		SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
		ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
}
