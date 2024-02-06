// Copyright 2022 OlssonDev

#pragma once
#include "CoreMinimal.h"
#include "IPlacementModeModule.h"
#include "Modules/ModuleManager.h"

class UACAssetAction;
class UACActorFactory;

class FAssetCreatorEditorModule : public IModuleInterface
{
    public:
    
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    static FName GetEditorStyleSet()
    {
        return "EditorStyle";
    };
};
