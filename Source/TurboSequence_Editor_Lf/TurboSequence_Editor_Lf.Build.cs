﻿using UnrealBuildTool;

public class TurboSequence_Editor_Lf : ModuleRules
{
    public TurboSequence_Editor_Lf(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", 
                "StaticMeshDescription", 
                "SkeletalMeshUtilitiesCommon", 
                "TurboSequence_Lf",
                "Niagara",
                "TurboSequence_HelperModule_Lf",
                "DeveloperSettings",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "InputCore",
                "Engine",
                "Slate",
                "SlateCore",
                "AssetTools",
                "UnrealEd",
                "Blutility", 
                "UMGEditor",
                "ScriptableEditorWidgets",
                "Projects",
                "DesktopPlatform",
                "UMG", 
                "AssetRegistry",
                "AssetTools",
                "RawMesh",
                "MeshDescription",
                "TurboSequence_Shader_Lf",
                "RenderCore",
                "TurboSequence_Lf",
                "TurboSequence_HelperModule_Lf",
                "RHI"
            }
        );
    }
}