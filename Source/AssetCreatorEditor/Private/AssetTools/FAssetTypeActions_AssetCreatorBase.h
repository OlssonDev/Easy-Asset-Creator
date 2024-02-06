// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "ACAssetAction.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

#define LOCTEXT_NAMESPACE "FAssetTypeActions_AssetCreatorBase"

class FAssetTypeActions_AssetCreatorBase : public FAssetTypeActions_Blueprint
{
	public:

	FAssetTypeActions_AssetCreatorBase(FAssetActionSettings InSettings, uint32 InCategory)
	{
		Settings = InSettings;
		Category = InCategory;
	};
	
	// IAssetTypeActions interface
	virtual FColor GetTypeColor() const override;
	virtual FText GetName() const override;
	virtual uint32 GetCategories() override;
	virtual UClass* GetSupportedClass() const override;
	virtual const TArray<FText>& GetSubMenus() const override;
	virtual UFactory* GetFactoryForBlueprintType(UBlueprint* InBlueprint) const override;
	// End of IAssetTypeActions interface

	protected:

	FAssetActionSettings Settings;
	uint32 Category;
	
};

#undef LOCTEXT_NAMESPACE
