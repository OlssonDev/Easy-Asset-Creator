﻿// Copyright 2022 OlssonDev


#include "FAssetTypeActions_AssetCreatorBase.h"

#include "KismetCompilerModule.h"
#include "Factory/ACBaseFactory.h"
#include "Kismet/KismetSystemLibrary.h"

FColor FAssetTypeActions_AssetCreatorBase::GetTypeColor() const
{
	return Settings.AssetColor;
}

FText FAssetTypeActions_AssetCreatorBase::GetName() const
{
	return Settings.AssetClass->GetDisplayNameText();
}

uint32 FAssetTypeActions_AssetCreatorBase::GetCategories()
{
	return Category;
}

UClass* FAssetTypeActions_AssetCreatorBase::GetSupportedClass() const
{
	return Settings.AssetClass;
}

const TArray<FText>& FAssetTypeActions_AssetCreatorBase::GetSubMenus() const
{
	if (Settings.UseSubMenu && !Settings.SubCategoryName.IsEmpty())
	{
		static TArray<FText> NewSubMenusArray;
		NewSubMenusArray.Empty();
		NewSubMenusArray.Add(Settings.SubCategoryName);
		
		return NewSubMenusArray;
	}

	return FAssetTypeActions_Base::GetSubMenus();
}

UFactory* FAssetTypeActions_AssetCreatorBase::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UACBaseFactory* BaseFactory = NewObject<UACBaseFactory>();
	BaseFactory->ParentClass = InBlueprint->GeneratedClass;
	return BaseFactory;
}
