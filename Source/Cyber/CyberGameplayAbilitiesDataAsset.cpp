// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberGameplayAbilitiesDataAsset.h"


UCyberGameplayAbilitiesDataAsset::UCyberGameplayAbilitiesDataAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const TSet<FGameplayInputAbilityInfo>& UCyberGameplayAbilitiesDataAsset::GetInputAbilities() const
{
	return InputAbilities;
}

#if WITH_EDITOR
void UCyberGameplayAbilitiesDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FProperty* Property = PropertyChangedEvent.Property;
	if (Property && Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCyberGameplayAbilitiesDataAsset, InputAbilities) && !InputAbilities.IsEmpty())
	{
		TArray<FGameplayInputAbilityInfo> InputAbilitiesArray = InputAbilities.Array();
		InputAbilities.Reset();

		for (int32 i = 0; i < InputAbilitiesArray.Num(); ++i)
		{
			FGameplayInputAbilityInfo& Info = InputAbilitiesArray[i];
			Info.InputID = i;
			InputAbilities.Add(Info);
		}
	}
}
#endif