// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cyber.h"
#include "Modules/ModuleManager.h"

// Abilities
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Attack, "Ability.Character.Attack", "The ability allows the player to attack.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Attack_Cooldown, "Ability.Character.Attack.Cooldown", "The cooldown for the attack ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Damage, "Ability.Character.Damage", "The ability is used for player feedback when being damaged.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Dash, "Ability.Character.Dash", "The ability allows the player to dash.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Dash_Cooldown, "Ability.Character.Dash.Cooldown", "The cooldown to activate the dash again.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Dash_Cooldown_SetByCaller, "Ability.Character.Dash.Cooldown.SetByCaller", "The SetByCaller Tag to modify the dash cooldown.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Stun, "Ability.Character.Stun", "The ability to stun targets.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Knockback, "Ability.Character.Knockback", "The ability to knockback targets.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_HitReact, "Ability.Character.HitReact", "The ability to react to being hit.")

// Abilities (Enemy)
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Enemy_Attack, "Ability.Enemy.Attack", "The ability allows the enemy to attack.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Enemy_Attack_Cooldown, "Ability.Enemy.Attack.Cooldown", "The cooldown for the enemy to attack again.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Shoot, "Ability.Character.Shoot", "The ability allows the enemy to shoot.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Character_Shoot_Cooldown, "Ability.Character.Shoot.Cooldown", "The cooldown for the shoot ability.")

// Gameplay Effects
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Basic, "GameplayEffect", "All gameplay effects tags will be attached here.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Damaged, "GameplayEffect.Damaged", "Gameplay effect to apply damage.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Death, "GameplayEffect.Death", "Gameplay effect to make character die.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Stun, "GameplayEffect.Stun", "Gameplay effect to make character stunned.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_Tokens_Enemy, "GameplayEffect.Initialize.Tokens.Enemy", "Gameplay effect to initialize Enemy tokens for each player.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_Tokens_Cycle, "GameplayEffect.Initialize.Tokens.Cycle", "Gameplay effect to initialize Cycle tokens for each player.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_Health, "GameplayEffect.Initialize.Health", "Gameplay effect to initialize Health for each player.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_Health_SetByCaller, "GameplayEffect.Initialize.Health.SetByCaller", "Set By Caller magnitude for Health.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_MaxHealth, "GameplayEffect.Initialize.MaxHealth", "Gameplay effect to initialize Max Health for each player.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_MaxHealth_SetByCaller, "GameplayEffect.Initialize.MaxHealth.SetByCaller", "Set By Caller magnitude for Max Health.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_DamagePower_SetByCaller, "GameplayEffect.Initialize.DamagePower.SetByCaller", "Set By Caller magnitude for damage power.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_StunTime_SetByCaller, "GameplayEffect.Initialize.StunTime.SetByCaller", "Gameplay effect to initialize Stun Time for each player.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_KnockbackStrength_SetByCaller, "GameplayEffect.Initialize.KnockbackStrength.SetByCaller", "Set By Caller magnitude for Knockback Strength to targets.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_MarkedTime_SetByCaller, "GameplayEffect.Initialize.MarkedTime.SetByCaller", "Set By Caller magnitude for Marked Time to neutral color targets.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Initialize_DashCooldown_SetByCaller, "GameplayEffect.Initialize.DashCooldown.SetByCaller", "Set By Caller magnitude for Dash Cooldown for players.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_WaveMultiplier_SetByCaller, "GameplayEffect.WaveMultiplier.SetByCaller", "Set By Caller magnitude for Wave Multiplier value.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Cost_EnemyTokens_SetByCaller, "GameplayEffect.Cost.EnemyTokens.SetByCaller", "Set By Caller magnitude when applying the Cost in Enemy Tokens.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Cost_CycleTokens_SetByCaller, "GameplayEffect.Cost.CycleTokens.SetByCaller", "Set By Caller magnitude when applying the Cost in Cycle Tokens.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Hit, "GameplayEffect.Hit", "Tag to be added when you get hit.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Slowtime, "GameplayEffect.Slowtime", "Tag to be added when you get slowed down.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_MarkInArray, "GameplayEffect.MarkInArray", "Tag to be added when character uses Mark in Array special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_MarkInArray_SetByCaller, "GameplayEffect.MarkInArray.SetByCaller", "Set by caller tag to specify duration of the Mark in Array special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Combat_CanCombo, "GameplayEffect.Combat.CanCombo", "Gameplay tag to allow the player execute a combat combo.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_GameWin, "GameplayEffect.GameWin", "Gameplay tag for the player when they win the game.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Shield, "GameplayEffect.Shield", "Gameplay tag for the player when shield is activated.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Shield_Hit, "GameplayEffect.Shield.Hit", "Gameplay tag for the player when shield is activated (to protect from the hits).")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Heal, "GameplayEffect.Heal", "Gameplay tag for the player when heal is activated.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_Heal_SetByCaller, "GameplayEffect.Heal.SetByCaller", "Set by caller tag to specify duration of the heal special ability.")

// Gameplay Event
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Combat_Combo_Enable, "GameplayEvent.Combat.Combo.Enable", "Gameplay event to allow the player to execute a combo.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Combat_Combo_Disable, "GameplayEvent.Combat.Combo.Disable", "Gameplay event to stop allowing the player to execute a combo.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_Hit, "GameplayEvent.Hit", "Gameplay event to alert the player that it has been hit.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEvent_RemoveShieldHit, "GameplayEvent.RemoveShieldHit", "Gameplay event to alert the player that a shield hit has been removed.")

// Special Abilities
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility, "SpecialAbility", "All gameplay abilities considered as special abilities.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_SelfNeutralization, "SpecialAbility.Character.SelfNeutralization", "The Self Neutralization special ability tag.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_SelfNeutralization_Cooldown, "SpecialAbility.Character.SelfNeutralization.Cooldown", "The cooldown for the self neutralization special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Slowtime, "SpecialAbility.Character.Slowtime", "The Slowtime special ability tag.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Slowtime_Cooldown, "SpecialAbility.Character.Slowtime.Cooldown", "The cooldown for the slowtime special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_MarkInArray, "SpecialAbility.Character.MarkInArray", "The Mark In Array special ability tag.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_MarkInArray_Cooldown, "SpecialAbility.Character.MarkInArray.Cooldown", "The cooldown for the Mark In Array special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Shield, "SpecialAbility.Character.Shield", "The Shield special ability tag.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Shield_Cooldown, "SpecialAbility.Character.Shield.Cooldown", "The cooldown for the Shield special ability.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Heal, "SpecialAbility.Character.Heal", "The Heal special ability tag.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(SpecialAbility_Character_Heal_Cooldown, "SpecialAbility.Character.Heal.Cooldown", "The cooldown for the Heal special ability.")

// Gameplay Effect for how many tokens are obtained for each enemy and cycle/wave.
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_TokensValue_Enemy, "GameplayEffect.TokensValue.Enemy", "How many tokens are gained when destroying an enemy.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayEffect_TokensValue_Cycle, "GameplayEffect.TokensValue.Cycle", "How many tokens are gained when surviving a cycle.")

// Gameplay Cues
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Damaged, "GameplayCue.Damaged", "Gameplay cue when damage is applied.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Dash, "GameplayCue.Dash", "Gameplay cue when dash is executed.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Death, "GameplayCue.Death", "Gameplay cue when death is executed.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_ChangeColor, "GameplayCue.ChangeColor", "Gameplay cue when character color is changed.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Stun, "GameplayCue.Stun", "Gameplay cue when character or enemy is stunned.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_SelfNeutralization, "GameplayCue.SelfNeutralization", "Gameplay cue when Self Neutralization is used.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Slowtime, "GameplayCue.Slowtime", "Gameplay cue when Slowtime is used.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_MarkInArray, "GameplayCue.MarkInArray", "Gameplay cue when Mark In Array is used.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_GameWin, "GameplayCue.GameWin", "Gameplay cue when game is won.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayCue_Heal, "GameplayCue.Heal", "Gameplay cue when heal is used.")

// Gameplay Tags for Color Mechanics
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Color_One, "Character.Color.One", "Character has been assigned Color One.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Color_Two, "Character.Color.Two", "Character has been assigned Color Two.")
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Character_Color_Neutral, "Character.Color.Neutral", "Character has been assigned Neutral color.")


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Cyber, "Cyber" );
 