// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

using UnrealBuildTool;

public class TurboSequence_Shader_Lf : ModuleRules
{
    public TurboSequence_Shader_Lf(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "TurboSequence_HelperModule_Lf",
                "Renderer",
                "RHI",
                "RenderCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Projects",
                "Renderer",
                "RHI",
                "RenderCore",
            }
        );
    }
}