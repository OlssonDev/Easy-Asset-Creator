// Copyright 2022 OlssonDev

#include "Factory/ACActorFactory.h"
#include "ACAssetAction.h"

bool UACActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	UObject* Asset = AssetData.FastGetAsset(false);
	if (UBlueprint* Blueprint = Cast<UBlueprint>(Asset))
	{
		return Blueprint->GeneratedClass->IsChildOf(AssetAction->AssetActionSettings.AssetClass);
	}
	
	return Super::CanCreateActorFrom(AssetData, OutErrorMsg);
}

void UACActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	AssetAction->PostSpawnActor(Asset, NewActor);
}
