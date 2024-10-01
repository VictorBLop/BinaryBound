// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShieldActor::AShieldActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Initialize and attach the Sword Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	Mesh->SetEnableGravity(false);
	Mesh->SetSimulatePhysics(false);
	SetRootComponent(Mesh);

	// Initialize Widget
	ShieldWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Shield Widget"));
	ShieldWidget->SetupAttachment(Mesh);
	ShieldWidget->SetRelativeLocation(FVector(0, 0, 180.0f));

	bReplicates = true;
}

// Called when the game starts or when spawned
void AShieldActor::BeginPlay()
{
	Super::BeginPlay();
}

void AShieldActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, WidgetText, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Color, COND_None, REPNOTIFY_Always);
}

void AShieldActor::OnRep_bUpdateWidget()
{
	OnWidgetUpdate.Broadcast(WidgetText);
}

void AShieldActor::OnRep_bUpdateColor()
{
	OnWidgetColorUpdate.Broadcast(Color);
}


void AShieldActor::SetShieldText(FText Text)
{
	WidgetText = Text;

	OnRep_bUpdateWidget();
}

void AShieldActor::SetShieldColor(FLinearColor NewColor)
{
	Color = NewColor;

	OnRep_bUpdateColor();
}


