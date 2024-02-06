// Copyright 2022 OlssonDev


#include "ACAssetAction.h"

#include "Fragments/ACAssetActionFragment.h"

void UACAssetAction::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	K2_PostSpawnActor(Asset, NewActor);
}

const UACAssetActionFragment* UACAssetAction::FindFragmentByClass(TSubclassOf<UACAssetActionFragment> FragmentClass) const
{
	if (IsValid(FragmentClass))
	{
		for (const UACAssetActionFragment* Fragment : Fragments)
		{
			if (IsValid(Fragment) && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

TSubclassOf<UObject> UACAssetAction::GetClassFromAsset(UObject* Asset)
{
	if (IsValid(Asset))
	{
		if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
		{
			return Blueprint->GeneratedClass;
		}
	}

	return {};
}
