// Copyright 2022 OlssonDev


#include "ACAssetAction_AssetFactory.h"

#include "Factory/ACBaseFactory.h"

#define LOCTEXT_NAMESPACE "UAssetAction_AssetFactory"

UACAssetAction_AssetFactory::UACAssetAction_AssetFactory()
{
	AssetActionSettings.AssetClass = UACBaseFactory::StaticClass();
	AssetActionSettings.CategoryName = "Easy Asset Creator";
}

#undef LOCTEXT_NAMESPACE