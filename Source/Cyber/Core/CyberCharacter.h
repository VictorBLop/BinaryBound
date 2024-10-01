// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "..\Interfaces/Saveable.h"
#include "..\CyberStructs.h"
#include "Components/WidgetComponent.h" 
#include "..\Interfaces/ColorInterface.h"
#include "..\Interfaces/SwordInterface.h"
#include "CyberCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class ACyberPlayerController;
class UNiagaraComponent;
class ACyberEnemy;
class UCyberGameplayAbilitiesDataAsset;
class ACyberSword;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnColorTagsSetup, FGameplayTag, ActiveColorTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStunApplied, bool, ApplyStun);

UCLASS(config=Game)
class ACyberCharacter : public ACharacter, public IAbilitySystemInterface, public ISaveable, public IColorInterface, public ISwordInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Jetpack Mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Jetpack;

	/** VFX */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = VFX, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* DashVFX;

	/** Sword */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* CyberSwordComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ACyberSword* CyberSword = nullptr;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAbility1Action;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAbility2Action;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SpecialAbility3Action;

protected:

	/* Sword Interface */

	void TurnOnSwordCollision() override;

	void TurnOffSwordCollision() override;

	void SetActivateKnockback(bool ActivateKnockback) override;

public:

	/* Color Interface */

	void ActivateTrail() override;

	void DeactivateTrail() override;

	FLinearColor GetPlayerColor() override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCyberGameplayAbilitiesDataAsset> CyberGameplayAbilitiesDataAsset;

	void InitAbilitySystem();
	void OnAbilityInputPressed(int32 InputID);
	void OnAbilityInputReleased(int32 InputID);

public:

	FORCEINLINE UCyberGameplayAbilitiesDataAsset* GetCyberGameplayAbilitiesDataAsset() const { return CyberGameplayAbilitiesDataAsset; }

public:
	ACyberCharacter();

	FVector GetCyberCurrentAcceleration();

	float GetCyberMaxWalkSpeed();

	UFUNCTION(BlueprintCallable)
	void ActivateDashVFX(bool Activate);

	//////* GAS - Gameplay Ability System *//////

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PossessedBy(AController* NewController) override;

	void AddMappingContextCharacter(AController* newController);

	virtual void OnRep_PlayerState() override;

	void OnRep_Controller() override;
	
	void SetupGameplayAbilitySystemComponent();

	UFUNCTION(BlueprintCallable)
	void SetUpInitialEffects();

	UFUNCTION(BlueprintCallable)
	void SetUpInitialAttributeEffects();

	void SetupMaterialColors();

	UFUNCTION(BlueprintCallable)
	void ChangeCharacterColor(FLinearColor TintColor);

	UPROPERTY(BlueprintAssignable)
	FOnColorTagsSetup OnColorTagsSetup;

	void OnColorTagsSetupCall();

	UPROPERTY(ReplicatedUsing = OnRep_InitialColorTag)
	FGameplayTag InitialColorTag;

	UFUNCTION()
	void OnRep_InitialColorTag();
	
	UPROPERTY(ReplicatedUsing = OnRep_SelfNeutralColor)
	FLinearColor SelfNeutralColor;
	
	UFUNCTION()
	void OnRep_SelfNeutralColor();

	// Restarting Game
	UFUNCTION(BlueprintCallable)
	void MoveToStartLocation();

	// Sounds

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS - Sounds")
	TArray<USoundBase*> SpecialAbilitySounds;

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/* Dash Elements */
	void Dash();

	/* Special Abilities */
	
	void SpecialAbility1();

	void SpecialAbility2();

	void SpecialAbility3();

	void ActivateSpecialAbility(int32 Slot);

	float ClampLevel(float &CustomLevel);

public:

	UFUNCTION(BlueprintCallable)
	FLinearColor GetCharacterColor();

	UFUNCTION(BlueprintCallable)
	void SetPlayerAbilitiesInServer(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo);

	UFUNCTION(Server, Reliable)
	void ServerRPC_SetPlayerAbilitiesInServer(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo);

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetPlayerAbilitiesInClient(const FPlayerSpecialAbilitiesInfo& playerSpecialAbilitiesInfo);

	// Stun

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> StunEffect = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnStunApplied OnStunApplied;

protected:

	//////* GAS - Gameplay Ability System *//////
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> GASAbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialAttributeEffects;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialSpecificColorEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Save Data")
	FPlayerSaveData PlayerData;

	/* Special Abilities and Slots */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Data")
	FPlayerSpecialAbilitiesInfo PlayerSpecialAbilitiesInfo;

	// Hit Reaction

	void TriggerReaction(AActor* HitActor, AActor* InstigatorActor, float StunTime);

	UPROPERTY(EditAnywhere, Category = "GAS - Hit")
	TSubclassOf<UGameplayEffect> RemoveHitEffect = nullptr;

	UPROPERTY(EditAnywhere, Category = "GAS - Hit")
	TSubclassOf<UGameplayEffect> HitEffect = nullptr;

	void ApplyHit();

	void RemoveHitCount(float StunTime);

	/* Stun */

	UPROPERTY(ReplicatedUsing = OnRep_ApplyStun)
	bool ApplyStun = false;

	UFUNCTION()
	void OnRep_ApplyStun();

	UFUNCTION(BlueprintCallable)
	void StunPlayer(bool IsStunned);

	/* Generic Apply Effect Method (to be called only from server) */

	bool ApplyEffectSpec(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayEffect> Effect, AActor* InstigatorActor, AActor* EffectCauserActor, FGameplayTag SetByCallerTag, float Magnitude);

	/* Color Mechanic Elements */

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	TArray<TObjectPtr<UMaterialInterface>> CharacterColorMaterials;

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	TArray<FLinearColor> CharacterColors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Color Mechanics")
	FName ColorVectorParameter;

	UPROPERTY(BlueprintReadOnly, Replicated)
	int32 ColorsIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "GAS - Color Mechanics")
	bool bUseWholeMaterialsForCharacter = false;

	UPROPERTY(EditDefaultsOnly, Category = "GAS - Color Mechanics")
	bool bUseWholeMaterialsForDamage = false;

	// Damage Effect

	UPROPERTY(EditAnywhere, Category = "GAS - Attack")
	TSubclassOf<UGameplayEffect> DealDamage = nullptr;

	// Original Location

	UPROPERTY()
	FVector StartLocation = FVector(0.0f, 0.0f, 0.0f);

public:

	/* Damage Materials */

	UPROPERTY(BlueprintReadWrite, Category = "GAS - Color Mechanics")
	TObjectPtr<UMaterialInterface> OriginalJetpackMaterialInterface;

	UPROPERTY(BlueprintReadWrite, Category = "GAS - Color Mechanics")
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterialInterfaces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attack")
	TObjectPtr<UMaterialInterface> DamageMaterialInterface;

	UPROPERTY(ReplicatedUsing = OnRep_DamageMaterial)
	bool bReplicateApplyDamageMaterial;

	UPROPERTY(ReplicatedUsing = OnRep_OriginalMaterials)
	bool bReplicateOriginalMaterials;

	UFUNCTION()
	void OnRep_DamageMaterial();

	UFUNCTION()
	void OnRep_OriginalMaterials();

	/* Damage Colors */

	UPROPERTY(BlueprintReadWrite)
	FLinearColor OriginalColor;

	UPROPERTY(EditAnywhere, Category = "GAS - Color Mechanics")
	FLinearColor DamageColor;

	// Apply Damage - Player Feedback

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attack")
	float DelayForCharacterHitFeedback = 0.0f;

	UFUNCTION(BlueprintCallable)
	void ApplyDamagePlayerFeedback();

	UFUNCTION(BlueprintCallable)
	void EndDamagePlayerFeedback();

	/* Apply Damage Both Players */

	void PlayerGotHit(float DamageMagnitude, float StunMagnitude, AActor* InstigatorActor);

	/* Player Controller */

	UPROPERTY(BlueprintReadOnly)
	ACyberPlayerController* CyberPlayerController = nullptr;

	void SetUpPlayerControllerWidgets();

	/* Save Game */

	virtual void SaveData_Implementation(UCyberSaveGame* saveGameRef) override;

	virtual void LoadData_Implementation(UCyberSaveGame* saveGameRef) override;

	UFUNCTION()
	void OnGameLoadedEvent(UCyberSaveGame* saveGameRef);

	UFUNCTION()
	void OnGameSavedEvent();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
