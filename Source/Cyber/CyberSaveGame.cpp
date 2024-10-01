// Fill out your copyright notice in the Description page of Project Settings.


#include "CyberSaveGame.h"

void UCyberSaveGame::SaveAllData(TArray<UObject*> SaveableObjects)
{
	for (TObjectPtr<UObject> object : SaveableObjects)
	{
		if (object->GetClass()->ImplementsInterface(USaveable::StaticClass()))
		{
			ISaveable::Execute_SaveData(object, this);
		}
	}
}

void UCyberSaveGame::LoadDataForRequester(UObject* Requester)
{
	if (Requester->GetClass()->ImplementsInterface(USaveable::StaticClass()))
	{
		ISaveable::Execute_LoadData(Requester, this);
	}
}
