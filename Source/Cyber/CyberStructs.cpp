// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberStructs.h"
#include "Cyber.h"

FGameplayTag GetSpecialAbilityActivationTag(SpecialAbilityType abilityType)
{
    switch (abilityType) {

        case SpecialAbilityType::None:

            return FGameplayTag();
            break;

        case SpecialAbilityType::Heal:
        
            return SpecialAbility_Character_Heal; // Change
            break;

        case SpecialAbilityType::Shield:

            return SpecialAbility_Character_Shield; // Change
            break;

        case SpecialAbilityType::Neutral: 

            return SpecialAbility_Character_SelfNeutralization;
            break;

        case SpecialAbilityType::Slowtime:

            return SpecialAbility_Character_Slowtime;
            break;

        case SpecialAbilityType::MarkInArray:

            return SpecialAbility_Character_MarkInArray;
            break;

        case SpecialAbilityType::Dash:

            return FGameplayTag();
            break;

        default:

            return FGameplayTag();
            break;
    }
}

int32 GetIndexFromAbility(int32 Slot, TArray<FSpecialAbility> SpecialAbilities)
{
    for (int32 index = 0; index < SpecialAbilities.Num(); index++)
    {
        if(SpecialAbilities.IsValidIndex(index))
        {
            if (SpecialAbilities[index].Slot == Slot)
            {
                return index;
            }
        }
    }

    return -1;
}
