// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberEnemyMelee.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\Cyber.h"
#include "..\Core\CyberCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACyberEnemyMelee::ACyberEnemyMelee()
{
	// Create the Child Actor Component
	CyberBatonComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("CyberBatonComp"));
	CyberBatonComp->SetupAttachment(GetMesh(), FName(TEXT("Weapon_R")));
}

void ACyberEnemyMelee::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bApplySwordColor, COND_None, REPNOTIFY_Always);
}

void ACyberEnemyMelee::OnRep_ApplySwordColor()
{
	CyberBaton = Cast<ACyberSword>(CyberBatonComp->GetChildActor());

	if (CyberBaton)
	{
		CyberBaton->SetOwner(this);
	}

	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberBatonComp->GetChildActor()))
	{
		SwordInterface->SetSwordColor(ColorVectorParameter, EnemyColor);
	}
}

void ACyberEnemyMelee::TurnOnSwordCollision()
{
	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberBatonComp->GetChildActor()))
	{
		SwordInterface->TurnOnSwordCollision();
	}
}

void ACyberEnemyMelee::TurnOffSwordCollision()
{
	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberBatonComp->GetChildActor()))
	{
		SwordInterface->TurnOffSwordCollision();
	}
}

void ACyberEnemyMelee::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetSwordColor();
	}
}

void ACyberEnemyMelee::SetSwordColor()
{
	bApplySwordColor = !bApplySwordColor;

	OnRep_ApplySwordColor();
}

void ACyberEnemyMelee::IncreaseComboIndex()
{
	if (ComboIndex < AnimationSections.Num() - 1)
	{
		ComboIndex++;
	}
	else if (ComboIndex == AnimationSections.Num() - 1)
	{
		ComboIndex = 0;
	}
}

// Called every frame
void ACyberEnemyMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
