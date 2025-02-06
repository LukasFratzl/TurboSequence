// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

using UnrealBuildTool;

public class TurboSequence_Lf : ModuleRules
{
	public TurboSequence_Lf(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[]
			{
				// ... add public include paths required here ...
			}
		);


		PrivateIncludePaths.AddRange(
			new string[]
			{
				// ... add other private include paths required here ...
			}
		);


		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"Engine",
				"TurboSequence_Shader_Lf",
				"RenderCore",
				"TurboSequence_HelperModule_Lf",
				"Niagara"
				//"AnimationBlueprintLibrary"
				// ... add other public dependencies that you statically link with here ...
			}
		);


		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Projects",
				"UMG",
				"RenderCore",
				"Renderer",
				"RHI",
				"MeshDescription"
				// ... add private dependencies that you statically link with here ...	
			}
		);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}