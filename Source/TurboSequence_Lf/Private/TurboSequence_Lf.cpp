// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#include "TurboSequence_Lf.h"

#include "TurboSequence_Helper_Lf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"

#define LOCTEXT_NAMESPACE "FTurboSequence_LfModule"

void FTurboSequence_LfModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.OnFilesLoaded().AddRaw(this, &FTurboSequence_LfModule::OnFilesLoaded);
}

void FTurboSequence_LfModule::ShutdownModule()
{
}

void FTurboSequence_LfModule::OnFilesLoaded()
{
	IAssetRegistry& IAssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	IAssetRegistry.OnFilesLoaded().RemoveAll(this);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboSequence_LfModule, TurboSequence_Lf)
