// Copyright 2022 OlssonDev


#include "ACAssetFactory.h"

#define LOCTEXT_NAMESPACE "UAssetCreator_AssetFactory"

UACAssetFactory::UACAssetFactory()
{
	SupportedClass = UACBaseFactory::StaticClass();
	ParentClass = UACBaseFactory::StaticClass();
	UseClassPicker = false;
}

#undef LOCTEXT_NAMESPACE
