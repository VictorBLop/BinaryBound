// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CyberStructs.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialMaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 InitialEnemyTokens;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 InitialTokensPerEnemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 InitialCycleTokens;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 InitialTokensPerCycle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialDamagePower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialKnockbackStrength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialStunTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialMarkedTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialDashCooldown;
};

USTRUCT(BlueprintType)
struct FEnemySaveData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialMaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialDamagePower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialStunTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float InitialKnockbackStrength;
};

UENUM(BlueprintType)
enum class EnemyType : uint8 {
	Melee = 0 UMETA(DisplayName = "Melee"),
	Ranged = 1 UMETA(DisplayName = "Ranged")
};

UENUM(BlueprintType)
enum class AttributeTypes : uint8 {
	Health = 0 UMETA(DisplayName = "Health"),
	MaxHealth = 1 UMETA(DisplayName = "MaxHealth"),
	Damage = 2  UMETA(DisplayName = "Damage"),
	StunTime = 3 UMETA(DisplayName = "StunTime"),
	KnockbackStrength = 4 UMETA(DisplayName = "KnockbackStrength")
};

UENUM(BlueprintType)
enum class UpgradeType : uint8 {
	None = 0 UMETA(DisplayName = "None"),
	Health = 1 UMETA(DisplayName = "Health"),
	Damage = 2  UMETA(DisplayName = "Damage"),
	MarkedTime = 3 UMETA(DisplayName = "MarkedTime"),
	DashCooldown = 4 UMETA(DisplayName = "DashCooldown"),
	AttackKnockback = 5 UMETA(DisplayName = "AttackKnockback"),
	AttackStun = 6 UMETA(DisplayName = "AttackStun"),
	TokensPerEnemy = 7 UMETA(DisplayName = "TokensPerEnemy"),
	TokensPerWave = 8 UMETA(DisplayName = "TokensPerWave")
};

USTRUCT(BlueprintType)
struct FUpgradeInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	UpgradeType PlayerUpgradeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float MagnitudeForPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool ApplyToAllPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	UpgradeType EnemyUpgradeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float MagnitudeForEnemies;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool ApplyToEnemies; 

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	bool HasBeenUnlocked;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	bool HasBeenUsed;
};

USTRUCT(BlueprintType)
struct FEffectInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	TSubclassOf<UGameplayEffect> PlayerGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	TSubclassOf<UGameplayEffect> EnemyGameplayEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FGameplayTag SetByCallerTag;

};


UENUM(BlueprintType)
enum class SpecialAbilityType : uint8 {
	None = 0	UMETA(DisplayName = "None"),
	Heal = 1	UMETA(DisplayName = "Heal"),
	Shield = 2  UMETA(DisplayName = "Shield"),
	Neutral = 3		UMETA(DisplayName = "Neutral"),
	Slowtime = 4	UMETA(DisplayName = "Slowtime"),
	MarkInArray = 5	UMETA(DisplayName = "MarkInArray"),
	Dash = 6	UMETA(DisplayName = "Dash"),
	Slot1 = 7	UMETA(DisplayName = "Slot1"),
	Slot2 = 8	UMETA(DisplayName = "Slot2"),
	Slot3 = 9	UMETA(DisplayName = "Slot3")
};


USTRUCT(BlueprintType)
struct FUnlockSlotOrAbilityButton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool IsUnlocked;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	bool IsUsed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool UnlocksSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	SpecialAbilityType AbilityType;
};


USTRUCT(BlueprintType)
struct FSpecialAbilityInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	bool IsUnlocked;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	int32 Slot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FText AbilityInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	SpecialAbilityType AbilityType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float AbilityPercent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FLinearColor Color;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float Level;

	UPROPERTY(BlueprintReadWrite, Category = "Save Game")
	float CooldownTime;
};

USTRUCT(BlueprintType)
struct FSpecialAbilityActivationInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FGameplayTag SpecialAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FGameplayTag AbilityTagForUI;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FGameplayTag SpecialAbilityCooldownTag;
};

USTRUCT(BlueprintType)
struct FSpecialAbility : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	bool IsActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int Slot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float Level;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	SpecialAbilityType AbilityType;
};

// Select Gameplay Tag depending on Special Ability Type
FGameplayTag GetSpecialAbilityActivationTag(SpecialAbilityType abilityType);

// Get Ability Index depending on Slot
int32 GetIndexFromAbility(int32 Slot, TArray<FSpecialAbility> SpecialAbilities);

USTRUCT(BlueprintType)
struct FSpecialAbilityTime : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float Level1Time;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float Level2Time;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	float Level3Time;
};

UENUM(BlueprintType)
enum class PlayerWidgets : uint8 {
	PreGameTimer = 0	UMETA(DisplayName = "PreGameTimer"),
	PlayerInfo = 1  UMETA(DisplayName = "PlayerInfo"),
	Indicator = 2		UMETA(DisplayName = "Indicator"),
	GameOver = 3	UMETA(DisplayName = "GameOver"),
};


USTRUCT(BlueprintType)
struct FUpgradeAbilitySlotListEntry : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FUnlockSlotOrAbilityButton SpecialAbilityInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FUpgradeInfo Upgrade1Info;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	FUpgradeInfo Upgrade2Info;
};


USTRUCT(BlueprintType)
struct FPlayerSpecialAbilitiesInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 UnlockedSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	TArray<SpecialAbilityType> UnlockedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	TArray<FSpecialAbility> ActiveAbilities;
};


USTRUCT(BlueprintType)
struct FPlayerEndStats : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 Attempts = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 EnemiesDestroyed = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save Game")
	int32 WavesCompleted = 0;
};

UENUM(BlueprintType)
enum class MultiplayerMode : uint8 {
	TwoPlayers = 0	UMETA(DisplayName = "2Players"),
	ThreePlayers = 1  UMETA(DisplayName = "3Players"),
	FourPlayers = 2		UMETA(DisplayName = "4Players"),
};

USTRUCT(BlueprintType)
struct FLocalMultiplayerOptions : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Local Multiplayer")
	MultiplayerMode LocalMultiplayer = MultiplayerMode::TwoPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Local Multiplayer")
	int32 NumberOfPlayers = 2;
};


