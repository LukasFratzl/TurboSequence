// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

using UnrealBuildTool;

public class TurboSequence_HelperModule_Lf : ModuleRules
{
    public TurboSequence_HelperModule_Lf(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "RenderCore",
                "Renderer",
                "RHI",
                // "Niagara"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "RenderCore",
                "Renderer",
                "RHI",
            }
        );
    }
}