// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

using UnrealBuildTool;

public class TurboSequence_HelperModule_Lf : ModuleRules
{
	public TurboSequence_HelperModule_Lf(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"Engine",
				"RenderCore",
				"Renderer",
				"RHI"
				// "Niagara"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"Engine",
				"CoreUObject",
				"RenderCore",
				"Renderer",
				"RHI"
			}
		);
	}
}