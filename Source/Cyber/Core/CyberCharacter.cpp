// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CyberPlayerState.h"
#include "..\Cyber.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CyberGameMode.h"
#include "..\Enemy\CyberEnemyMelee.h"
#include "..\Enemy\CyberEnemy.h"
#include "CyberGameState.h"
#include "CyberPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "CyberGameInstance.h"
#include "..\Interfaces/Persistence.h"
#include "..\AttributeSets/CyberAttributeSet.h"
#include "..\AttributeSets/TokensAttributeSet.h"
#include "..\AttributeSets/CombatAttributeSet.h"
#include "..\AttributeSets/ColorAttributeSet.h"
#include "..\AttributeSets/MovementAttributeSet.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Abilities/GameplayAbility.h" 
#include "..\CyberGameplayAbilitiesDataAsset.h"
#include "..\CyberSword.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);


//////////////////////////////////////////////////////////////////////////
// ACyberCharacter

ACyberCharacter::ACyberCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = 6000.f;
	GetCharacterMovement()->BrakingDecelerationWalking = GetCharacterMovement()->MaxAcceleration;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->bConstrainAspectRatio = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	// Initialize Niagara System
	DashVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dash VFX"));
	DashVFX->SetupAttachment(GetMesh());

	// Create the Child Actor Component
	CyberSwordComp = CreateDefaultSubobject<UChildActorComponent>(TEXT("CyberSwordComp"));
	CyberSwordComp->SetupAttachment(GetMesh(), FName(TEXT("hand_r_sword")));
	
	// Initialize and attach the Sword Mesh
	Jetpack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JetpackMesh"));
	Jetpack->SetupAttachment(GetMesh(), FName("Jet_Engine"));

	// Initialize AbilitySystemComponent
	GASAbilitySystemComponent = nullptr;

	// Set Relevancy
	bAlwaysRelevant = true;
}

void ACyberCharacter::TurnOnSwordCollision()
{
	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberSwordComp->GetChildActor()))
	{
		SwordInterface->TurnOnSwordCollision();
	}
}

void ACyberCharacter::TurnOffSwordCollision()
{
	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberSwordComp->GetChildActor()))
	{
		SwordInterface->TurnOffSwordCollision();
	}
}

void ACyberCharacter::SetActivateKnockback(bool ActivateKnockback)
{
	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberSwordComp->GetChildActor()))
	{
		SwordInterface->SetActivateKnockback(ActivateKnockback);
	}
}

void ACyberCharacter::ActivateTrail()
{
	if (IColorInterface* ColorInterface = Cast<IColorInterface>(CyberSwordComp->GetChildActor()))
	{
		ColorInterface->ActivateTrail();
	}
}

void ACyberCharacter::DeactivateTrail()
{
	if (IColorInterface* ColorInterface = Cast<IColorInterface>(CyberSwordComp->GetChildActor()))
	{
		ColorInterface->DeactivateTrail();
	}
}

FLinearColor ACyberCharacter::GetPlayerColor()
{
	return OriginalColor;
}

void ACyberCharacter::InitAbilitySystem()
{
	if (!HasAuthority())
	{
		return;
	}

	if (CyberGameplayAbilitiesDataAsset)
	{
		const TSet<FGameplayInputAbilityInfo>& InputAbilities = CyberGameplayAbilitiesDataAsset->GetInputAbilities();
		constexpr int32 AbilityLevel = 1;

		for (const auto& It : InputAbilities)
		{
			if (It.IsValid())
			{
				const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(It.GameplayAbilityClass, AbilityLevel, It.InputID);
				GASAbilitySystemComponent->GiveAbility(AbilitySpec);
			}
		}
	}
}

void ACyberCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (GASAbilitySystemComponent.Get())
	{
		GASAbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void ACyberCharacter::OnAbilityInputReleased(int32 InputID)
{
	if (GASAbilitySystemComponent.Get())
	{
		GASAbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}


FVector ACyberCharacter::GetCyberCurrentAcceleration()
{
	return GetCharacterMovement()->GetCurrentAcceleration();
}

float ACyberCharacter::GetCyberMaxWalkSpeed()
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void ACyberCharacter::ActivateDashVFX(bool Activate)
{
	DashVFX->ActivateSystem(Activate);
}

UAbilitySystemComponent* ACyberCharacter::GetAbilitySystemComponent() const
{
	return GASAbilitySystemComponent.Get();
}

void ACyberCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupMaterialColors();

	SetupGameplayAbilitySystemComponent();

	SetUpPlayerControllerWidgets();

	CyberPlayerController = Cast<ACyberPlayerController>(NewController);

	StartLocation = GetActorLocation();

	//Add Input Mapping Context
	AddMappingContextCharacter(NewController);
}

void ACyberCharacter::AddMappingContextCharacter(AController* newController)
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(newController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACyberCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (ACyberPlayerState* CyberPlayerState = GetPlayerState<ACyberPlayerState>())
	{
		// Cache the Ability System Component on the Clients.
		GASAbilitySystemComponent = CyberPlayerState->GetAbilitySystemComponent();

		// Initialize the Ability System Component on the Clients.
		// The Player State will be the Owner Actor of the Component since the component lives on it.
		// This Character will be the Avatar Actor of the Component since it can die and we don't want to recreate the Component every death.
		GASAbilitySystemComponent->InitAbilityActorInfo(CyberPlayerState, this);
	}
	else
	{
		// On the ocasion the Player Controller replicates after the Player State to fix the Controller reference inside the Ability System.

		GASAbilitySystemComponent->RefreshAbilityActorInfo();
	}

	SetupMaterialColors();

	// 
	if (ACyberGameState* CyberGameState = Cast<ACyberGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		CyberGameState->PlayerJoined();
	}

	SetUpPlayerControllerWidgets();

	CyberPlayerController = Cast<ACyberPlayerController>(GetController());

	StartLocation = GetActorLocation();
}

void ACyberCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (ACyberPlayerState* CyberPlayerState = GetPlayerState<ACyberPlayerState>())
	{
		// On the ocasion the Player Controller replicates after the Player State to fix the Controller reference inside the Ability System.
		CyberPlayerState->GetAbilitySystemComponent()->RefreshAbilityActorInfo();
	}
}

void ACyberCharacter::SetupGameplayAbilitySystemComponent()
{
	if (ACyberPlayerState* CyberPlayerState = GetPlayerState<ACyberPlayerState>())
	{
		GASAbilitySystemComponent = CyberPlayerState->GetAbilitySystemComponent();
		GASAbilitySystemComponent->InitAbilityActorInfo(CyberPlayerState, this);

		CyberPlayerState->InitializeAttributeSet(); // Initialize Attributes Set
				
		SetUpInitialEffects();

		SetUpInitialAttributeEffects();

		// Apply Initial Effect only for one specific color
		if(InitialSpecificColorEffects.IsValidIndex(ColorsIndex))
		{
			GASAbilitySystemComponent->ApplyGameplayEffectToSelf(
				InitialSpecificColorEffects[ColorsIndex].GetDefaultObject(), // Specific Color GameplayEffect
				1.0f, // Level
				GASAbilitySystemComponent->MakeEffectContext()); // Effect context
		}
	}

	InitAbilitySystem();

	// Broadcast when Color Tags have been setup and send the Active Color Tag.
	OnColorTagsSetupCall();
}

void ACyberCharacter::SetUpInitialEffects()
{
	for (const TSubclassOf<UGameplayEffect>& effect : InitialEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		/* Apply Gameplay Effect Spec */
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
	}
}

void ACyberCharacter::SetUpInitialAttributeEffects()
{
	for (const TSubclassOf<UGameplayEffect>& effect : InitialAttributeEffects)
	{
		// Create Context Handle from characterHit Ability System Component
		FGameplayEffectContextHandle effectContext = GASAbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpec* Spec = new FGameplayEffectSpec(effect.GetDefaultObject(), effectContext, 1.0f);

		// In Spec, using Set By Caller, initialize the values.

		// Health
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_MaxHealth, PlayerData.InitialMaxHealth);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_Health, PlayerData.InitialMaxHealth);

		// Tokens
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_Tokens_Enemy, PlayerData.InitialEnemyTokens);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_Tokens_Cycle, PlayerData.InitialCycleTokens);

		Spec->SetSetByCallerMagnitude(GameplayEffect_TokensValue_Enemy, PlayerData.InitialTokensPerEnemy);
		Spec->SetSetByCallerMagnitude(GameplayEffect_TokensValue_Cycle, PlayerData.InitialTokensPerCycle);

		// Combat
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_DamagePower_SetByCaller, PlayerData.InitialDamagePower);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_KnockbackStrength_SetByCaller, PlayerData.InitialKnockbackStrength);
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_StunTime_SetByCaller, PlayerData.InitialStunTime);

		// Color
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_MarkedTime_SetByCaller, PlayerData.InitialMarkedTime);

		// Movement
		Spec->SetSetByCallerMagnitude(GameplayEffect_Initialize_DashCooldown_SetByCaller, PlayerData.InitialDashCooldown);

		/* Apply Gameplay Effect Spec */
		FActiveGameplayEffectHandle GameplayEffectHandle = GASAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec, FPredictionKey());
	}
}

void ACyberCharacter::SetupMaterialColors()
{
	// Get Original Materials from Character Mesh and Set Custom Color defined in CharacterColorMaterials array.
	for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
	{
		// For the first material of all, let's change it to a custom color.
		if (index == 0)
		{
			// In case of being server, get the Game Mode, get the ColorsIndex and increase the index in the server.
			if (HasAuthority())
			{
				// Get Game Mode
				if (ACyberGameMode* CyberGameMode = Cast<ACyberGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
				{
					ColorsIndex = CyberGameMode->GetColorsIndex(); // Gets ColorsIndex, which will be replicated

					CyberGameMode->SetColorsIndex(CyberGameMode->GetColorsIndex() + 1); // Increase ColorsIndex in the server
				}
			}

			// First material of the character, set it to Custom color in Game Mode.
			if (!bUseWholeMaterialsForCharacter) // Change only Color of the Material.
			{
				if (CharacterColors.IsValidIndex(ColorsIndex))
				{
					FLinearColor Color = CharacterColors[ColorsIndex];
					
					GetMesh()->SetVectorParameterValueOnMaterials(ColorVectorParameter, FVector(Color.R, Color.G, Color.B));

					CyberSword = Cast<ACyberSword>(CyberSwordComp->GetChildActor());
					
					if(CyberSword)
					{
						CyberSword->SetOwner(this);
					}

					if(ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberSwordComp->GetChildActor()))
					{
						SwordInterface->SetSwordColor(ColorVectorParameter, Color);
					}

					Jetpack->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color.R, Color.G, Color.B));

					DashVFX->SetVariableLinearColor(TEXT("ColorTag"), Color);

					OriginalColor = Color;
				}
			}
			else  // Change Complete Materials
			{ 
				if (CharacterColorMaterials.IsValidIndex(ColorsIndex))
				{
					GetMesh()->SetMaterial(index, CharacterColorMaterials[ColorsIndex]);

					Jetpack->SetMaterial(0, CharacterColorMaterials[ColorsIndex]);
				}
			}
		}

		OriginalMaterialInterfaces.Add(GetMesh()->GetMaterial(index));
	}

	OriginalJetpackMaterialInterface = Jetpack->GetMaterial(0);
}

void ACyberCharacter::ChangeCharacterColor(FLinearColor TintColor)
{
	SelfNeutralColor = TintColor;

	OnRep_SelfNeutralColor();
}

void ACyberCharacter::OnColorTagsSetupCall()
{
	if (GASAbilitySystemComponent->HasMatchingGameplayTag(Character_Color_One))
	{
		InitialColorTag = Character_Color_One;
	}
	else if (GetAbilitySystemComponent()->HasMatchingGameplayTag(Character_Color_Two))
	{
		InitialColorTag = Character_Color_Two;
	}

	OnRep_InitialColorTag();
}

void ACyberCharacter::OnRep_InitialColorTag()
{
	OnColorTagsSetup.Broadcast(InitialColorTag);
}

void ACyberCharacter::OnRep_SelfNeutralColor()
{
	FLinearColor Color = SelfNeutralColor;

	GetMesh()->SetVectorParameterValueOnMaterials(ColorVectorParameter, FVector(Color.R, Color.G, Color.B));

	if (ISwordInterface* SwordInterface = Cast<ISwordInterface>(CyberSwordComp->GetChildActor()))
	{
		SwordInterface->SetSwordColor(ColorVectorParameter, Color);
	}

	Jetpack->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(Color.R, Color.G, Color.B));

	DashVFX->SetVariableLinearColor(TEXT("ColorTag"), Color);
}


void ACyberCharacter::MoveToStartLocation()
{
	SetActorLocation(StartLocation, false, nullptr, ETeleportType::ResetPhysics);
}

void ACyberCharacter::PlayerGotHit(float DamageMagnitude, float StunMagnitude, AActor* InstigatorActor)
{
	if (!DealDamage)
	{
		return;
	}

	if (ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(GetPlayerState()))
	{
		CyberPlayerState->ApplyGameplayEffectAllPlayers(DealDamage, GameplayEffect_Damaged, DamageMagnitude);
	}

	ApplyHit();

	TriggerReaction(this, InstigatorActor, StunMagnitude);
}

void ACyberCharacter::SetUpPlayerControllerWidgets()
{
	// Add Widgets in Player Controller
	if (ACyberPlayerController* playerController = Cast<ACyberPlayerController>(GetController()))
	{
		// Set Player State reference in Player Controller
		if (ACyberPlayerState* playerState = Cast<ACyberPlayerState>(GetPlayerState()))
		{
			playerController->SetPlayerStateReference(playerState);
		}
	}
}

/* Save Game */

void ACyberCharacter::SaveData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(GetPlayerState()))
	{
		int32 PlayerIndex = CyberPlayerState->CyberPlayerIndex;
	
		if (saveGameRef)
		{
			if (!saveGameRef->PlayersSaveData.IsValidIndex(PlayerIndex))
			{
				saveGameRef->PlayersSaveData.Add(FPlayerSaveData()); // Add Empty new PlayerSaveData.
			}

			// Cyber Attribute Set (Health)
			if (const UCyberAttributeSet* CyberAttributeSet = Cast<UCyberAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UCyberAttributeSet::StaticClass())))
			{
				saveGameRef->PlayersSaveData[PlayerIndex].InitialMaxHealth = CyberAttributeSet->MaxHealth.GetCurrentValue();
			}

			// Tokens Attribute Set
			if (const UTokensAttributeSet* TokensAttributeSet = Cast<UTokensAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UTokensAttributeSet::StaticClass())))
			{
				saveGameRef->PlayersSaveData[PlayerIndex].InitialEnemyTokens = TokensAttributeSet->EnemyTokens.GetCurrentValue();
				saveGameRef->PlayersSaveData[PlayerIndex].InitialTokensPerEnemy = TokensAttributeSet->TokensPerEnemy.GetCurrentValue();

				saveGameRef->PlayersSaveData[PlayerIndex].InitialCycleTokens = TokensAttributeSet->CycleTokens.GetCurrentValue();
				saveGameRef->PlayersSaveData[PlayerIndex].InitialTokensPerCycle = TokensAttributeSet->TokensPerCycle.GetCurrentValue();
			}

			// Combat Attribute Set
			if (const UCombatAttributeSet* CombatAttributeSet = Cast<UCombatAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UCombatAttributeSet::StaticClass())))
			{
				saveGameRef->PlayersSaveData[PlayerIndex].InitialDamagePower = CombatAttributeSet->DamagePower.GetCurrentValue();
				saveGameRef->PlayersSaveData[PlayerIndex].InitialKnockbackStrength = CombatAttributeSet->KnockbackStrength.GetCurrentValue();
				saveGameRef->PlayersSaveData[PlayerIndex].InitialStunTime = CombatAttributeSet->StunTime.GetCurrentValue();
			}

			// Color Attribute Set
			if (const UColorAttributeSet* ColorAttributeSet = Cast<UColorAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UColorAttributeSet::StaticClass())))
			{
				saveGameRef->PlayersSaveData[PlayerIndex].InitialMarkedTime = ColorAttributeSet->MarkedTime.GetCurrentValue();
			}

			// Movement Attribute Set
			if (const UMovementAttributeSet* MovementAttributeSet = Cast<UMovementAttributeSet>(GetAbilitySystemComponent()->GetAttributeSet(UMovementAttributeSet::StaticClass())))
			{
				saveGameRef->PlayersSaveData[PlayerIndex].InitialDashCooldown = MovementAttributeSet->DashCooldown.GetCurrentValue();
			}

			// Unlocked Slots & Abilities and Active Abilities
			if (PlayerIndex == 0)
			{
				saveGameRef->Player1SpecialAbilitiesInfo = PlayerSpecialAbilitiesInfo; // Player 1
			}
			else
			{
				saveGameRef->Player2SpecialAbilitiesInfo = PlayerSpecialAbilitiesInfo; // Player 2
			}
		}
	}
}

void ACyberCharacter::LoadData_Implementation(UCyberSaveGame* saveGameRef)
{
	if (ACyberPlayerState* CyberPlayerState = Cast<ACyberPlayerState>(GetPlayerState()))
	{
		int32 PlayerIndex = CyberPlayerState->CyberPlayerIndex;
	
		if(saveGameRef)
		{
			if(saveGameRef->PlayersSaveData.IsValidIndex(PlayerIndex))
			{
				// Health
				PlayerData.InitialMaxHealth = saveGameRef->PlayersSaveData[PlayerIndex].InitialMaxHealth;

				// Tokens
				PlayerData.InitialEnemyTokens = saveGameRef->PlayersSaveData[PlayerIndex].InitialEnemyTokens;
				PlayerData.InitialTokensPerEnemy = saveGameRef->PlayersSaveData[PlayerIndex].InitialTokensPerEnemy;

				PlayerData.InitialCycleTokens = saveGameRef->PlayersSaveData[PlayerIndex].InitialCycleTokens;
				PlayerData.InitialTokensPerCycle = saveGameRef->PlayersSaveData[PlayerIndex].InitialTokensPerCycle;
				
				// Combat
				PlayerData.InitialDamagePower = saveGameRef->PlayersSaveData[PlayerIndex].InitialDamagePower;
				PlayerData.InitialKnockbackStrength = saveGameRef->PlayersSaveData[PlayerIndex].InitialKnockbackStrength;
				PlayerData.InitialStunTime = saveGameRef->PlayersSaveData[PlayerIndex].InitialStunTime;

				// Color
				PlayerData.InitialMarkedTime = saveGameRef->PlayersSaveData[PlayerIndex].InitialMarkedTime;

				// Movement
				PlayerData.InitialDashCooldown = saveGameRef->PlayersSaveData[PlayerIndex].InitialDashCooldown;
			}

			// Unlocked Slots & Abilities and Active Abilities
			if (PlayerIndex == 0)
			{
				PlayerSpecialAbilitiesInfo = saveGameRef->Player1SpecialAbilitiesInfo; // Player 1
			}
			else
			{
				PlayerSpecialAbilitiesInfo = saveGameRef->Player2SpecialAbilitiesInfo; // Player 2

				ClientRPC_SetPlayerAbilitiesInClient(PlayerSpecialAbilitiesInfo);
			}
		}
	}
}

void ACyberCharacter::OnGameLoadedEvent(UCyberSaveGame* saveGameRef)
{
	// Request Load to Game Instance.
	if (IPersistence* persistence = Cast<IPersistence>(GetGameInstance()))
	{
		persistence->RequestLoad(this);
	}

	SetUpInitialAttributeEffects();
}

void ACyberCharacter::OnGameSavedEvent()
{
	// Event just in case it is needed.
}

void ACyberCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	AddMappingContextCharacter(Controller);

	if (UCyberGameInstance* CyberGameInstance = Cast<UCyberGameInstance>(GetGameInstance()))
	{
		CyberGameInstance->OnGameLoaded.AddDynamic(this, &ThisClass::OnGameLoadedEvent);
		CyberGameInstance->OnGameSaved.AddDynamic(this, &ThisClass::OnGameSavedEvent);
	}
}

void ACyberCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACyberCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bReplicateApplyDamageMaterial, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, bReplicateOriginalMaterials, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ColorsIndex, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, InitialColorTag, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, SelfNeutralColor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ApplyStun, COND_None, REPNOTIFY_Always);	
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACyberCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (CyberGameplayAbilitiesDataAsset)
		{
			const TSet<FGameplayInputAbilityInfo>& InputAbilities = CyberGameplayAbilitiesDataAsset->GetInputAbilities();
			for (const auto& It : InputAbilities)
			{
				if (It.IsValid())
				{
					const UInputAction* InputAction = It.InputAction;
					const int32 InputID = It.InputID;

					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Started, this, &ACyberCharacter::OnAbilityInputPressed, InputID);
					EnhancedInputComponent->BindAction(InputAction, ETriggerEvent::Completed, this, &ACyberCharacter::OnAbilityInputReleased, InputID);
				}
			}
		}

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACyberCharacter::Move);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ACyberCharacter::Dash);

		// Special Ability 1
		EnhancedInputComponent->BindAction(SpecialAbility1Action, ETriggerEvent::Started, this, &ACyberCharacter::SpecialAbility1);

		// Special Ability 2
		EnhancedInputComponent->BindAction(SpecialAbility2Action, ETriggerEvent::Started, this, &ACyberCharacter::SpecialAbility2);

		// Special Ability 3
		EnhancedInputComponent->BindAction(SpecialAbility3Action, ETriggerEvent::Started, this, &ACyberCharacter::SpecialAbility3);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACyberCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = FollowCamera->GetComponentRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACyberCharacter::Dash()
{
	if (GASAbilitySystemComponent.Get())
	{
		// Activate Gameplay Ability Dash
		GASAbilitySystemComponent->TryActivateAbilitiesByTag(Ability_Character_Dash.GetTag().GetSingleTagContainer());
	}
}

void ACyberCharacter::SpecialAbility1()
{
	ActivateSpecialAbility(1);
}

void ACyberCharacter::SpecialAbility2()
{
	ActivateSpecialAbility(2);
}

void ACyberCharacter::SpecialAbility3()
{
	ActivateSpecialAbility(3);
}

void ACyberCharacter::ActivateSpecialAbility(int32 Slot)
{
	int32 AbilityIndex = GetIndexFromAbility(Slot, PlayerSpecialAbilitiesInfo.ActiveAbilities);

	if (PlayerSpecialAbilitiesInfo.ActiveAbilities.IsValidIndex(AbilityIndex))
	{
		if (GASAbilitySystemComponent.Get() && PlayerSpecialAbilitiesInfo.ActiveAbilities[AbilityIndex].IsActive)
		{
			FGameplayEventData Payload;

			Payload.EventMagnitude = ClampLevel(PlayerSpecialAbilitiesInfo.ActiveAbilities[AbilityIndex].Level);

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GetSpecialAbilityActivationTag(PlayerSpecialAbilitiesInfo.ActiveAbilities[AbilityIndex].AbilityType), Payload);

			if (SpecialAbilitySounds[AbilityIndex])
			{
				UGameplayStatics::PlaySound2D(GetWorld(), SpecialAbilitySounds[AbilityIndex], 1.0f, 1.0f, 0.2f);
			}
		}
	}
}

float ACyberCharacter::ClampLevel(float &CustomLevel)
{
	return FMath::Clamp(CustomLevel, 1.0f, 3.0f);
}

FLinearColor ACyberCharacter::GetCharacterColor()
{
	if (CharacterColors.IsValidIndex(ColorsIndex))
	{
		return CharacterColors[ColorsIndex];
	}
	else
	{
		return FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

void ACyberCharacter::SetPlayerAbilitiesInServer(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo)
{
	if (HasAuthority())
	{
		PlayerSpecialAbilitiesInfo = playerSpecialAbilitiesInfo;
	}
	else
	{
		ServerRPC_SetPlayerAbilitiesInServer(playerSpecialAbilitiesInfo);
	}
}

void ACyberCharacter::ServerRPC_SetPlayerAbilitiesInServer_Implementation(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo)
{
	PlayerSpecialAbilitiesInfo = playerSpecialAbilitiesInfo;
}

void ACyberCharacter::ClientRPC_SetPlayerAbilitiesInClient_Implementation(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo)
{
	PlayerSpecialAbilitiesInfo = playerSpecialAbilitiesInfo;
}


//////////////////////////////////////////////////////////////////////////
// GAS Elements

void ACyberCharacter::ApplyDamagePlayerFeedback()
{
	OnRep_DamageMaterial();

	bReplicateApplyDamageMaterial = !bReplicateApplyDamageMaterial;

	FTimerHandle CharacterFeedbackTimerHandle;
	// Add some delay before ending the damage player feedback (editable in Blueprints).
	if (DelayForCharacterHitFeedback >= 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			CharacterFeedbackTimerHandle, // handle to cancel timer at a later time
			this, // the owning object
			&ACyberCharacter::EndDamagePlayerFeedback, // function to call on elapsed
			DelayForCharacterHitFeedback, // float delay until elapsed
			false); // looping
	}
}

void ACyberCharacter::OnRep_DamageMaterial()
{
	// Change Materials of Mesh
	if(bUseWholeMaterialsForDamage)
	{
		if(!DamageMaterialInterface->IsValidLowLevel())
		{
			return;
		}

		// Set Character Materials.
		for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
		{
			GetMesh()->SetMaterial(index, DamageMaterialInterface);
		}

		// Set Jetpack Materials.
		for (int32 index = 0; index < Jetpack->GetMaterials().Num(); index++)
		{
			Jetpack->SetMaterial(index, DamageMaterialInterface);
		}

		
	} // Change Colors of Mesh
	else
	{
		GetMesh()->SetVectorParameterValueOnMaterials(ColorVectorParameter, FVector(DamageColor.R, DamageColor.G, DamageColor.B));

		Jetpack->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(DamageColor.R, DamageColor.G, DamageColor.B));
	}
}

void ACyberCharacter::EndDamagePlayerFeedback()
{
	OnRep_OriginalMaterials();

	bReplicateOriginalMaterials = !bReplicateOriginalMaterials;
}

void ACyberCharacter::OnRep_OriginalMaterials()
{
	if(bUseWholeMaterialsForDamage)
	{
		// Set Character Materials.
		for (int32 index = 0; index < GetMesh()->GetMaterials().Num(); index++)
		{
			GetMesh()->SetMaterial(index, OriginalMaterialInterfaces[index]);
		}

		// Set Jetpack Materials.
		for (int32 index = 0; index < Jetpack->GetMaterials().Num(); index++)
		{
			Jetpack->SetMaterial(index, OriginalJetpackMaterialInterface);
		}
	}
	else
	{
		GetMesh()->SetVectorParameterValueOnMaterials(ColorVectorParameter, FVector(OriginalColor.R, OriginalColor.G, OriginalColor.B));

		Jetpack->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(OriginalColor.R, OriginalColor.G, OriginalColor.B));
	}
}

void ACyberCharacter::TriggerReaction(AActor* HitActor, AActor* InstigatorActor, float StunTime)
{
	if (GASAbilitySystemComponent.Get())
	{
		FGameplayEventData Payload;
		Payload.Target = this;
		Payload.Instigator = InstigatorActor;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Ability_Character_HitReact, Payload);

		RemoveHitCount(StunTime);
	}
}



void ACyberCharacter::ApplyHit()
{
	if (GetAbilitySystemComponent() && HitEffect)
	{
		GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(HitEffect.GetDefaultObject(), 1.0f, FGameplayEffectContextHandle(), FPredictionKey());
	}
}

void ACyberCharacter::RemoveHitCount(float StunTime)
{
	if (GetAbilitySystemComponent() && RemoveHitEffect)
	{
		if (GetAbilitySystemComponent()->GetTagCount(GameplayEffect_Hit) >= 3)
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(RemoveHitEffect.GetDefaultObject(), 1.0f, FGameplayEffectContextHandle(), FPredictionKey());
		
			// Apply Stun
			ApplyEffectSpec(GetAbilitySystemComponent(), StunEffect, nullptr, this, GameplayEffect_Initialize_StunTime_SetByCaller, StunTime);
		}
	}
}

void ACyberCharacter::OnRep_ApplyStun()
{
	OnStunApplied.Broadcast(ApplyStun);
}

void ACyberCharacter::StunPlayer(bool IsStunned)
{
	ApplyStun = IsStunned;

	OnRep_ApplyStun();
}

bool ACyberCharacter::ApplyEffectSpec(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> Effect, AActor* InstigatorActor, AActor* EffectCauserActor, FGameplayTag SetByCallerTag, float Magnitude)
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
