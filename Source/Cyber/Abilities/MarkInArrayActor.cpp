// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkInArrayActor.h"
#include "Components/SphereComponent.h" 
#include "Net/UnrealNetwork.h"

// Sets default values
AMarkInArrayActor::AMarkInArrayActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")));
	Collider->SetCollisionObjectType(ECC_WorldDynamic);
	Collider->SetGenerateOverlapEvents(true);
	Collider->SetSimulatePhysics(false);
	Collider->SetMobility(EComponentMobility::Movable);
	SetRootComponent(Collider);

	// Initialize and attach the Sword Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	Mesh->SetEnableGravity(false);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetupAttachment(Collider);

	// Initialize Widget
	MarkZoneWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Mark Zone"));
	MarkZoneWidget->SetupAttachment(Collider);
	MarkZoneWidget->SetRelativeLocation(FVector(0, 0, 10.0f));
	MarkZoneWidget->SetRelativeRotation(FRotator(90.0f, 90.0f, 0.0f));
	MarkZoneWidget->SetHiddenInGame(true);

	bReplicates = true;
}

// Called when the game starts or when spawned
void AMarkInArrayActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMarkInArrayActor::ChangeColor(FLinearColor NewColor)
{
	if (HasAuthority())
	{
		Color = NewColor;
		OnRep_Color();
	}
}

void AMarkInArrayActor::OnRep_Color()
{
	Mesh->SetVectorParameterValueOnMaterials(TEXT("DiffuseColor"), FVector(Color));
	MarkZoneWidget->SetTintColorAndOpacity(Color);
}

UWidgetComponent* AMarkInArrayActor::GetMarkZoneWidget()
{
	return MarkZoneWidget;
}

void AMarkInArrayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Color, COND_None, REPNOTIFY_Always);
}

// Called every frame
void AMarkInArrayActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UStaticMeshComponent* AMarkInArrayActor::GetMesh()
{
	return Mesh;
}


