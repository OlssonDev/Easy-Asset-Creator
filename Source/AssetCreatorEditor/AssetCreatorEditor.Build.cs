// Copyright 2022 OlssonDev

using UnrealBuildTool;

public class AssetCreatorEditor : ModuleRules
{
    public AssetCreatorEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "AssetTools",
                "UnrealEd", 
                "PlacementMode",
                "EditorSubsystem",
                "PlacementMode",
                "EditorStyle",
                "InputCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "DeveloperSettings"
            }
        );
    }
}