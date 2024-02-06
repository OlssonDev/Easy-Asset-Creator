// Copyright 2022 OlssonDev
#include "ACAssetAction_AssetAction.h"

#define LOCTEXT_NAMESPACE "UAssetCreatorAction_AssetCreatorAction"

UACAssetAction_AssetAction::UACAssetAction_AssetAction()
{
	AssetActionSettings.AssetClass = UACAssetAction::StaticClass();
	AssetActionSettings.CategoryName = "Easy Asset Creator";
}

#undef LOCTEXT_NAMESPACE
