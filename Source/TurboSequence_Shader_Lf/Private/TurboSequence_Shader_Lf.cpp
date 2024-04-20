// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#include "TurboSequence_Shader_Lf.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FTurboSequence_Shader_LfModule"

void FTurboSequence_Shader_LfModule::StartupModule()
{
	const FString ShaderDirectory = FPaths::Combine(
		IPluginManager::Get().FindPlugin(TEXT("TurboSequence_Lf"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping("/TurboSequence_Shaders", ShaderDirectory);
}

void FTurboSequence_Shader_LfModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboSequence_Shader_LfModule, TurboSequence_Shader_Lf)
