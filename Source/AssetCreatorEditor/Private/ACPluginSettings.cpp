// Copyright 2022 OlssonDev


#include "ACPluginSettings.h"

#include "ACEditorSubsystem.h"

void UACPluginSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GEditor->GetEditorSubsystem<UACEditorSubsystem>()->RefreshAll();
}
