// Copyright 2022 OlssonDev


#include "ACAssetActionFactory.h"
#include "ACAssetAction.h"

#define LOCTEXT_NAMESPACE "UACAssetActionFactory"

UACAssetActionFactory::UACAssetActionFactory()
{
	SupportedClass = UACAssetAction::StaticClass();
	ParentClass = UACAssetAction::StaticClass();
	UseClassPicker = true;
}

#undef LOCTEXT_NAMESPACE
