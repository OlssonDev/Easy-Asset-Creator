// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "ACPluginSettings.generated.h"

UCLASS(config = Editor, defaultconfig, meta=(DisplayName="Easy Asset Creator"))
class ASSETCREATOREDITOR_API UACPluginSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool SupportsAutoRegistration() const override { return false; }

	public:
	
	//If the plugin should only scan certain folders for Assets/AssetActions/AssetFactory
	UPROPERTY(EditDefaultsOnly, Category = "Asset Scanning", Config)
	bool ScanOnlyAssignedFolders = false;

	//The paths to scan
	UPROPERTY(EditDefaultsOnly, meta = (EditCondition=ScanOnlyAssignedFolders), Category = "Asset Scanning", Config)
	TArray<FString> AssetPaths;
};
