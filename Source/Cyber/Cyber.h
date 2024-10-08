// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

// Abilities
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Attack)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Attack_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Damage)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Dash)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Dash_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Dash_Cooldown_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Stun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Knockback)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_HitReact)

// Abilities (Enemy)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Enemy_Attack)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Enemy_Attack_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Shoot)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Character_Shoot_Cooldown)

// Gameplay Effects
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Basic)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Damaged)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Death)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Stun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_Tokens_Enemy)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_Tokens_Cycle)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_Health)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_Health_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_MaxHealth)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_MaxHealth_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_DamagePower_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_StunTime_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_KnockbackStrength_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_MarkedTime_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Initialize_DashCooldown_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_WaveMultiplier_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Cost_EnemyTokens_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Cost_CycleTokens_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Hit)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Slowtime)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_MarkInArray)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_MarkInArray_SetByCaller)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Combat_CanCombo)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Shield)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Shield_Hit)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_Heal_SetByCaller)

// Gameplay Event
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Combat_Combo_Enable)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Combat_Combo_Disable)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Hit)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RemoveShieldHit)

// Special Abilities
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_SelfNeutralization)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_SelfNeutralization_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Slowtime)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Slowtime_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_MarkInArray)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_MarkInArray_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Shield)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Shield_Cooldown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Heal)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(SpecialAbility_Character_Heal_Cooldown)

// Gameplay Effect for how many tokens are obtained for each enemy and cycle/wave.
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_TokensValue_Enemy)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEffect_TokensValue_Cycle)

// Gameplay Cues
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Damaged)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Dash)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Death)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_ChangeColor)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Stun)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_SelfNeutralization)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Slowtime)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_MarkInArray)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_GameWin)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayCue_Heal)

// Gameplay Tags for Color Mechanics
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Color_One)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Color_Two)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Character_Color_Neutral)