// Copyright Epic Games, Inc. All Rights Reserved.

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
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FTurboSequence_LfModule::OnFilesLoaded()
{
	// const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	//
	// TArray<FAssetData> GlobalAssetData;
	// AssetRegistry.Get().GetAssetsByClass(FTopLevelAssetPath(UTurboSequence_GlobalData_Lf::StaticClass()->GetPathName()), GlobalAssetData);
	// if (GlobalAssetData.Num())
	// {
	// 	FTurboSequence_Helper_Lf::SortAssetsByPathName(GlobalAssetData);
	// 	GlobalData = Cast<UTurboSequence_GlobalData_Lf>(GlobalAssetData[GET0_NUMBER].GetAsset());
	// }
	// else
	// {
	// 	UE_LOG(LogTurboSequence_Lf, Error, TEXT("Can not find the Global Data asset -> This is really bad, without it Turbo Sequence does not work, you can recover it by creating an UTurboSequence_GlobalData_Lf Data Asset, Right click in the content browser anywhere in the Project, select Data Asset and choose UTurboSequence_GlobalData_Lf, save it and restart the editor"));
	// }

	IAssetRegistry& IAssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	IAssetRegistry.OnFilesLoaded().RemoveAll(this);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboSequence_LfModule, TurboSequence_Lf)
