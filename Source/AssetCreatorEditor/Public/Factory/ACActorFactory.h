// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "ACActorFactory.generated.h"

UCLASS()
class UACActorFactory : public UActorFactory
{
	GENERATED_BODY()

	//UActorFactory interface implementation
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	//End of implementation

	public:

	UPROPERTY()
	class UACAssetAction* AssetAction;
};
