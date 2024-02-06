// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ACAssetActionFragment.generated.h"


UCLASS(Abstract, EditInlineNew, DefaultToInstanced, NotBlueprintType)
class ASSETCREATOREDITOR_API UACAssetActionFragment : public UObject
{
	GENERATED_BODY()

	public:

	//Is this fragment currently in use?
	UPROPERTY(EditDefaultsOnly, Category = "Asset Action Fragment", DisplayName = "Fragment Enabled?")
	bool bEnabled = true;

	public:
	
	virtual bool IsValidFragment()
	{
		return bEnabled;
	};
	
};
