// Copyright Lukas Fratzl, 2G22-2G24. All Rights Reserved.

#include "TurboSequence_Editor_Lf.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "FileHelpers.h"
#include "LevelEditor.h"
#include "TurboSequence_ControlWidget_Lf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/ConstructorHelpers.h"

#define LOCTEXT_NAMESPACE "FTurboSequence_Editor_LfModule"

void FTurboSequence_Editor_LfModule::StartupModule()
{
	TurboSequence_MeshAssetData_AsyncComputeSwapBack.Empty();

	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	PluginAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Turbo Sequence")),
	                                                               LOCTEXT("Turbo Sequence", "Turbo Sequence"));

	TurboSequence_MeshAssetTypeActions = MakeShared<FTurboSequence_MeshAssetAction_Lf>();
	AssetTools.RegisterAssetTypeActions(TurboSequence_MeshAssetTypeActions.ToSharedRef());

	TurboSequence_AnimLibraryTypeActions = MakeShared<FTurboSequence_AnimLibraryAction_Lf>();
	AssetTools.RegisterAssetTypeActions(TurboSequence_AnimLibraryTypeActions.ToSharedRef());

	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>
		("LevelEditor");
	TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
	MenuExtender->AddMenuBarExtension(
		"Help",
		EExtensionHook::Before,
		nullptr,
		FMenuBarExtensionDelegate::CreateRaw(this, &FTurboSequence_Editor_LfModule::AddMenu)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.OnFilesLoaded().AddRaw(this, &FTurboSequence_Editor_LfModule::OnFilesLoaded);
}

void FTurboSequence_Editor_LfModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		return;
	}

	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(TurboSequence_MeshAssetTypeActions.ToSharedRef());

	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(TurboSequence_AnimLibraryTypeActions.ToSharedRef());

	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.OnFilesLoaded().RemoveAll(this);
}

void FTurboSequence_Editor_LfModule::AddMenu(FMenuBarBuilder& MenuBuilder)
{
	MenuBuilder.AddPullDownMenu(
		LOCTEXT("TurboSequence_Lf_Menu", "Turbo Sequence"),
		LOCTEXT("CrodwPlugin_Lf_MenuTooltipKey", "Opens useful tools for creating assets with the Turbo Sequence"),
		FNewMenuDelegate::CreateRaw(this, &FTurboSequence_Editor_LfModule::AddMenu_Widget),
		FName(TEXT("Turbo Sequence")),
		FName(TEXT("Turbo Sequence")));
}

void FTurboSequence_Editor_LfModule::AddMenu_Widget(class FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("TurboSequence_Lf_MenuEntry", "Control Panel"),
		LOCTEXT("CrodwPlugin_Lf_MenuEntryTooltipKey", "Opens the Control Panel for the Character Creation"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FTurboSequence_Editor_LfModule::PluginButtonClicked)),
		FName(TEXT("Control Panel")),
		EUserInterfaceActionType::Button,
		FName(TEXT("Control Panel")));
	MenuBuilder.AddMenuEntry(
		LOCTEXT("TurboSequence_Lf_MenuEntry", "Re-cache Mesh Assets"),
		LOCTEXT("CrodwPlugin_Lf_MenuEntryTooltipKey",
		        "Makes the asset see this editor as new Engine Version which makes it rebuild."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FTurboSequence_Editor_LfModule::OnInvalidMeshAssetCaches)),
		FName(TEXT("Re-cache Mesh Assets")),
		EUserInterfaceActionType::Button,
		FName(TEXT("Re-cache Mesh Assets")));
}

void FTurboSequence_Editor_LfModule::RepairMeshAssetAsync()
{
	RepairMaxIterationCounter = GET0_NUMBER;
	if (!TurboSequence_MeshAssetData_AsyncComputeSwapBack.Num())
	{
		// We reach the end
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("Finished Turbo Sequence Mesh Asset Init...."));
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=0"))
		return;
	}
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> TurboSequence_Asset = Cast<UTurboSequence_MeshAsset_Lf>(
		TurboSequence_MeshAssetData_AsyncComputeSwapBack[GET0_NUMBER].GetAsset());

	bool bAssetEdited = false;
	bool bSkinWeightEdited = false;

	if (IsValid(GlobalData) && !IsValid(TurboSequence_Asset->GlobalData))
	{
		TurboSequence_Asset->GlobalData = GlobalData;
		bAssetEdited = true;
	}

	//bool bAssetEdited = false;
	if (IsValid(TurboSequence_Asset->ReferenceMeshEdited) && TurboSequence_Asset->bNeedGeneratedNextEngineStart)
	{
		if (TurboSequence_Asset->InstancedMeshes.Num())
		{
			bool bHasAnyMesh = false;
			int32 NumLOD = TurboSequence_Asset->InstancedMeshes.Num();
			for (int32 MeshIdx = GET0_NUMBER; MeshIdx < NumLOD; ++MeshIdx)
			{
				if (IsValid(TurboSequence_Asset->InstancedMeshes[MeshIdx].StaticMesh))
				{
					bHasAnyMesh = true;
					break;
				}
			}
			if (bHasAnyMesh)
			{
				//bool bAssetEdited = false;

				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Detected Engine or Platform change ...."));
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Prepering Meshes ...."));

				TurboSequence_Asset->MeshDataOrderView.Empty();

				FString WantedMeshName = FString(FString::Format(
					TEXT("{0}_TurboSequence_Instance"), {*TurboSequence_Asset->ReferenceMeshNative->GetName()}));

				//int32 NumLOD = CPAsset->InstancedMeshes.Num();
				for (int32 MeshIdx = GET0_NUMBER; MeshIdx < NumLOD; ++MeshIdx)
				{
					if (IsValid(TurboSequence_Asset->InstancedMeshes[MeshIdx].StaticMesh))
					{
						const UPackage* Package = TurboSequence_Asset->InstancedMeshes[MeshIdx].StaticMesh->
							GetOutermost();
						const FString PackagePath = FPackageName::LongPackageNameToFilename(
							Package->GetName(), FPackageName::GetAssetPackageExtension());
						UPackageTools::LoadPackage(*PackagePath);

						TArray<int32> StaticMeshOrderIndices;

						if (TObjectPtr<UStaticMesh> StaticMesh =
							UTurboSequence_ControlWidget_Lf::GenerateStaticMeshFromSkeletalMesh(
								TurboSequence_Asset->ReferenceMeshEdited, MeshIdx, PackagePath,
								FString(FString::Format(
									TEXT("{0}_Lod_{1}"), {*WantedMeshName, *FString::FormatAsNumber(MeshIdx)})),
								StaticMeshOrderIndices, TurboSequence_Asset->MeshDataMode))
						{
							// pr #9
							// StaticMesh->NeverStream = true;
							
							//FMeshItem_Lf Item = FMeshItem_Lf();
							if (MeshIdx > GET9_NUMBER)
							{
								TurboSequence_Asset->InstancedMeshes[MeshIdx].bIsAnimated = false;
							}
							TurboSequence_Asset->InstancedMeshes[MeshIdx].StaticMesh = StaticMesh;
							//CreateVertexMaps(Main_Asset_To_Edit, i, Item.VertexData, Item.BoneIndices);
							//LevelOfDetails.Add(Item);
							//bAssetEdited = true;

							FMeshDataOrderView_Lf Order = FMeshDataOrderView_Lf();
							Order.StaticMeshIndices = StaticMeshOrderIndices;

							TurboSequence_Asset->MeshDataOrderView.Add(Order);


							bSkinWeightEdited = true;
						}
					}
				}

				if (bSkinWeightEdited)
				{
					const UPackage* Package = TurboSequence_Asset->ReferenceMeshEdited->GetOutermost();
					const FString PackagePath = FPackageName::LongPackageNameToFilename(
						Package->GetName(), FPackageName::GetAssetPackageExtension());
					UPackageTools::LoadPackage(*PackagePath);

					//TObjectPtr<UWorld> World = GEditor->GetEditorWorldContext().World();

					auto RunnerFunction = [TurboSequence_Asset](bool bSuccess)
					{
						if (!bSuccess)
						{
							UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=3"))
							RepairMeshAssetAsync();
							return;
						}

						RepairMeshAssetAsync2(TurboSequence_Asset, GEditor->GetEditorWorldContext().World(), true);
					};

					FTurboSequence_Utility_Lf::CreateRawSkinWeightTextureBuffer(
						TurboSequence_Asset, RunnerFunction, GEditor->GetEditorWorldContext().World());
				}

				// TurboSequence_Asset->Platform = FTurboSequence_Helper_Lf::GetEditorPlatformAsString();
				// TurboSequence_Asset->EngineVersion = FTurboSequence_Helper_Lf::GetEngineVersionAsString();
				TurboSequence_Asset->bNeedGeneratedNextEngineStart = false;

				// We need to cancel out of the loop cause otherwise we swap back the element before we reach
				// the async end
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=1"))
				return;
			}
		}
	}

	if (bAssetEdited)
	{
		FTurboSequence_Helper_Lf::SaveAsset(TurboSequence_Asset);
	}

	// Swap back and to the next iteration
	//UE_LOG(LogTurboSequence_Lf, Display, TEXT("Swap Back Mesh Data Asset from ID=0"))
	if (TurboSequence_MeshAssetData_AsyncComputeSwapBack.IsValidIndex(GET0_NUMBER))
	{
		//UE_LOG(LogTurboSequence_Lf, Display, TEXT("Swap Back Mesh Data Asset from ID=0"))
		TurboSequence_MeshAssetData_AsyncComputeSwapBack.RemoveAt(GET0_NUMBER);
		RepairMeshAssetAsync();
	}
}

void FTurboSequence_Editor_LfModule::RepairMeshAssetAsync2(const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
                                                           const TObjectPtr<UWorld> World, const bool bIsCollection)
{
	//TObjectPtr<UWorld> World = GEditor->GetEditorWorldContext().World();

	FTimerDelegate WaitTimerCallback;
	WaitTimerCallback.BindLambda([Asset, bIsCollection, World]
	{
		const UPackage* Package = Asset->GetOutermost();
		const FString PackagePath = FPackageName::LongPackageNameToFilename(
			Package->GetName(), FPackageName::GetAssetPackageExtension());
		UPackageTools::LoadPackage(*PackagePath);

		const FString TextureName = FString(FString::Format(TEXT("T_{0}_MeshData_Texture"), {*Asset->GetName()}));

		const FString& DirectoryPath = FPaths::GetPath(PackagePath);

		const FString TexturePath = FPaths::Combine(DirectoryPath, TextureName) +
			FPackageName::GetAssetPackageExtension();
		if (FPaths::FileExists(TexturePath))
		{
			UPackageTools::LoadPackage(*TexturePath);
		}

		bool bIsValidData = false;
		Asset->MeshDataTexture = FTurboSequence_Helper_Lf::GenerateTexture2DArrayFromRenderTarget2DArray(
			Asset->GlobalData->SkinWeightTexture, TexturePath, TextureName, false, true, GET1_NUMBER, bIsValidData);
		if (!bIsValidData)
		{
			if (RepairMaxIterationCounter > GET5_NUMBER)
			{
				UE_LOG(LogTurboSequence_Lf, Error,
				       TEXT(
					       "Limit reached on retries computing the Mesh Data Texture for asset -> %s | Might be an issue with your Input Mesh, it should have at least 1 vertex and have at least 1 Level Of Detail...., please regenerate the mesh asset with this given rules..."
				       ), *Asset->GetPathName());

				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Swap Back Mesh Data Asset from ID=2"))
				TurboSequence_MeshAssetData_AsyncComputeSwapBack.RemoveAt(GET0_NUMBER);
				RepairMeshAssetAsync();

				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=2"))
				return;
			}

			if (RepairMaxIterationCounter)
			{
				UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Mesh Data Texture Retry -> %d from -> 5 for asset -> %s"),
				       RepairMaxIterationCounter, *Asset->GetPathName());
			}
			RepairMaxIterationCounter++;

			auto RunnerFunction = [Asset, bIsCollection, World](bool bSuccess)
			{
				if (!bSuccess)
				{
					UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=4"))
					RepairMeshAssetAsync();
					return;
				}

				RepairMeshAssetAsync2(Asset, World, bIsCollection);
			};

			FTurboSequence_Utility_Lf::CreateRawSkinWeightTextureBuffer(
				Asset, RunnerFunction, GEditor->GetEditorWorldContext().World());
		}
		else
		{
			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(Asset->MeshDataTexture);

			FTurboSequence_Helper_Lf::SaveNewAsset(Asset->MeshDataTexture);
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Written Mesh Data Texture for Asset -> %s"),
			       *Asset.GetPathName());

			FTurboSequence_Helper_Lf::SaveAsset(Asset);

			if (bIsCollection)
			{
				if (TurboSequence_MeshAssetData_AsyncComputeSwapBack.IsValidIndex(GET0_NUMBER))
				{
					FTimerDelegate SwapBackCallback;
					SwapBackCallback.BindLambda([&]
					{
						UE_LOG(LogTurboSequence_Lf, Display, TEXT("Swap Back Mesh Data Asset from ID=1"))
						TurboSequence_MeshAssetData_AsyncComputeSwapBack.RemoveAt(GET0_NUMBER);
						RepairMeshAssetAsync();
					});
					World->GetTimerManager().SetTimerForNextTick(SwapBackCallback);
				}
			}
		}
	});
	World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);
}

void FTurboSequence_Editor_LfModule::PluginButtonClicked() const
{
	TArray<FAssetData> AssetData;
	const FAssetRegistryModule& AssetRegistry =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistry.Get().GetAssetsByClass(
		FTopLevelAssetPath(UEditorUtilityWidgetBlueprint::StaticClass()->GetPathName()), AssetData);

	if (AssetData.Num())
	{
		UEditorUtilityWidgetBlueprint* WidgetBP = nullptr;
		for (const FAssetData& Asset : AssetData)
		{
			if (UEditorUtilityWidgetBlueprint* Widget = Cast<UEditorUtilityWidgetBlueprint>(Asset.GetAsset()); Widget->
				ParentClass->IsChildOf(UTurboSequence_ControlWidget_Lf::StaticClass()))
			{
				WidgetBP = Widget;
				break;
			}
		}

		if (WidgetBP)
		{
			UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
		}
		else
		{
			// Widget not found
			UE_LOG(LogTemp, Warning, TEXT("Can't find widget derived from UTurboSequence_ControlWidget_Lf"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find widget ... that's pretty bad"));
	}
}

void FTurboSequence_Editor_LfModule::OnInvalidMeshAssetCaches() const
{
	TArray<FAssetData> AssetData;
	const FAssetRegistryModule& AssetRegistry =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	AssetRegistry.Get().GetAssetsByClass(FTopLevelAssetPath(UTurboSequence_MeshAsset_Lf::StaticClass()->GetPathName()),
	                                     AssetData);

	//UE_LOG(LogTemp, Warning, TEXT("%d"), AssetData.Num());
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *UTurboSequence_MeshAsset_Lf::StaticClass()->GetPathName());

	if (AssetData.Num())
	{
		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Re-Cache Turbo Sequence Mesh Assets"));
		for (const FAssetData& Asset : AssetData)
		{
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> MeshAsset = Cast<UTurboSequence_MeshAsset_Lf>(
				Asset.GetAsset());

			// MeshAsset->Platform = FString("");
			// MeshAsset->EngineVersion = FName("");

			MeshAsset->bNeedGeneratedNextEngineStart = true;


			FTurboSequence_Helper_Lf::SaveAsset(MeshAsset);
		}

		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Done ... please close the editor and open it again."));
	}
}

void FTurboSequence_Editor_LfModule::OnFilesLoaded()
{
	UE_LOG(LogTurboSequence_Lf, Display, TEXT("Initialize Turbo Sequence"));
	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");

	TArray<FAssetData> GlobalAssetData;
	AssetRegistry.Get().GetAssetsByClass(FTopLevelAssetPath(UTurboSequence_GlobalData_Lf::StaticClass()->GetPathName()),
	                                     GlobalAssetData);
	if (GlobalAssetData.Num())
	{
		FTurboSequence_Helper_Lf::SortAssetsByPathName(GlobalAssetData);
		GlobalData = Cast<UTurboSequence_GlobalData_Lf>(GlobalAssetData[GET0_NUMBER].GetAsset());

		bool bAssetEdited = false;

		FString EmitterName = FString("");
		FString MeshName = FString("");
		FString MaterialsName = FString("");
		FString IDName = FString("");
		FString PositionName = FString("");
		FString RotationName = FString("");
		FString ScaleName = FString("");
		FString LodName = FString("");
		FString CustomDataName = FString("");
		FString ParticleRemoveName = FString("");
		FTurboSequence_Helper_Lf::GetStringConfigSetting(EmitterName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraEmitter"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(MeshName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraMeshObject"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(MaterialsName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraMaterialObject"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(IDName,
														 TEXT(
															 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
														 TEXT("NameNiagaraParticleIDs"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(PositionName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraParticleLocations"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(RotationName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraParticleRotations"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(ScaleName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraParticleScales"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(LodName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraLevelOfDetailIndex"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(CustomDataName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraCustomData"));
		FTurboSequence_Helper_Lf::GetStringConfigSetting(ParticleRemoveName,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_NiagaraSettings_Lf"),
		                                                 TEXT("NameNiagaraParticleRemove"));
		if (EmitterName.IsEmpty())
		{
			EmitterName = FTurboSequence_Helper_Lf::NameNiagaraEmitter;
		}
		if (MeshName.IsEmpty())
		{
			MeshName = FTurboSequence_Helper_Lf::NameNiagaraMeshObject;
		}
		if (MaterialsName.IsEmpty())
		{
			MaterialsName = FTurboSequence_Helper_Lf::NameNiagaraMaterialObject;
		}
		if (IDName.IsEmpty())
		{
			IDName = FTurboSequence_Helper_Lf::NameNiagaraParticleIDs;
		}
		if (PositionName.IsEmpty())
		{
			PositionName = FTurboSequence_Helper_Lf::NameNiagaraParticleLocations;
		}
		if (RotationName.IsEmpty())
		{
			RotationName = FTurboSequence_Helper_Lf::NameNiagaraParticleRotations;
		}
		if (ScaleName.IsEmpty())
		{
			ScaleName = FTurboSequence_Helper_Lf::NameNiagaraParticleScales;
		}
		if (LodName.IsEmpty())
		{
			LodName = FTurboSequence_Helper_Lf::NameNiagaraLevelOfDetailIndex;
		}
		if (CustomDataName.IsEmpty())
		{
			CustomDataName = FTurboSequence_Helper_Lf::NameNiagaraCustomData;
		}
		if (ParticleRemoveName.IsEmpty())
		{
			ParticleRemoveName = FTurboSequence_Helper_Lf::NameNiagaraParticleRemove;
		}

		if (GlobalData->NameNiagaraEmitter.ToString() != EmitterName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraMeshObject != MeshName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraMaterialObject != MaterialsName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraParticleIDMap != IDName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraParticleLocations.ToString() != PositionName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraParticleRotations.ToString() != RotationName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraParticleScales.ToString() != ScaleName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraLevelOfDetailIndex.ToString() != LodName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraCustomData.ToString() != CustomDataName)
		{
			bAssetEdited = true;
		}
		if (GlobalData->NameNiagaraParticleRemove.ToString() != ParticleRemoveName)
		{
			bAssetEdited = true;
		}

		// Makes no sense at all
		if (GlobalData->bUseHighPrecisionAnimationMode != false)
		{
			GlobalData->bUseHighPrecisionAnimationMode = false;
			bAssetEdited = true;
		}

		GlobalData->NameNiagaraEmitter = FName(EmitterName);
		GlobalData->NameNiagaraMeshObject = MeshName;
		GlobalData->NameNiagaraMaterialObject = MaterialsName;
		GlobalData->NameNiagaraParticleIDMap = FName(IDName);
		GlobalData->NameNiagaraParticleLocations = FName(PositionName);
		GlobalData->NameNiagaraParticleRotations = FName(RotationName);
		GlobalData->NameNiagaraParticleScales = FName(ScaleName);
		GlobalData->NameNiagaraLevelOfDetailIndex = FName(LodName);
		GlobalData->NameNiagaraCustomData = FName(CustomDataName);
		GlobalData->NameNiagaraParticleRemove = FName(ParticleRemoveName);

		FString DefaultTransformTextureReferencePathCurrentFrame;
		FTurboSequence_Helper_Lf::GetStringConfigSetting(DefaultTransformTextureReferencePathCurrentFrame,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_RefSettings_Lf"),
		                                                 TEXT("Default_Rendering_TransformTexture_CurrentFrame"));
		if (DefaultTransformTextureReferencePathCurrentFrame.IsEmpty())
		{
			DefaultTransformTextureReferencePathCurrentFrame =
				FTurboSequence_Helper_Lf::ReferenceTurboSequenceTransformTextureCurrentFrame;
		}
		if (!IsValid(GlobalData->TransformTexture_CurrentFrame))
		{
			GlobalData->TransformTexture_CurrentFrame = FTurboSequence_Helper_Lf::LoadAssetFromReferencePath<
				UTextureRenderTarget2DArray>(DefaultTransformTextureReferencePathCurrentFrame);
			bAssetEdited = true;
		}
		if (!IsValid(GlobalData->TransformTexture_CurrentFrame))
		{
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "Can not find Transform Texture, it should at .../Plugins/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths, if it's not there please create a default Render Target 2D Array Texture and assign the reference in the TurboSequence Lf -> Reference Paths Project settings and open ../Plugins/TurboSequence_Lf/Resources/MF_TurboSequence_PositionOffset_Lf and assign it into the Texture Object with the Transform Texture Comment"
			       ));
		}
		else
		{
			bool bUse32BitTexture = GlobalData->bUseHighPrecisionAnimationMode;

			ETextureRenderTargetFormat Format = RTF_RGBA16f;
			if (bUse32BitTexture)
			{
				Format = RTF_RGBA32f;
			}

			if (GlobalData->TransformTexture_CurrentFrame->GetFormat() != GetPixelFormatFromRenderTargetFormat(Format))
			{
				bAssetEdited = true;
				const UPackage* Package = GlobalData->TransformTexture_CurrentFrame->GetOutermost();
				const FString PackagePath = FPackageName::LongPackageNameToFilename(
					Package->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*PackagePath);
				GlobalData->TransformTexture_CurrentFrame = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(
					PackagePath, GlobalData->TransformTexture_CurrentFrame->GetName(), GET2048_NUMBER, GET12_NUMBER,
					GetPixelFormatFromRenderTargetFormat(Format));
			}
		}

		FString DefaultTransformTextureReferencePathPreviousFrame;
		FTurboSequence_Helper_Lf::GetStringConfigSetting(DefaultTransformTextureReferencePathPreviousFrame,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_RefSettings_Lf"),
		                                                 TEXT("Default_Rendering_TransformTexture_PreviousFrame"));
		if (DefaultTransformTextureReferencePathPreviousFrame.IsEmpty())
		{
			DefaultTransformTextureReferencePathPreviousFrame =
				FTurboSequence_Helper_Lf::ReferenceTurboSequenceTransformTexturePreviousFrame;
		}
		if (!IsValid(GlobalData->TransformTexture_PreviousFrame))
		{
			GlobalData->TransformTexture_PreviousFrame = FTurboSequence_Helper_Lf::LoadAssetFromReferencePath<
				UTextureRenderTarget2DArray>(DefaultTransformTextureReferencePathPreviousFrame);
			bAssetEdited = true;
		}

		if (!IsValid(GlobalData->TransformTexture_PreviousFrame))
		{
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "Can not find Transform Texture, it should at .../Plugins/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths, if it's not there please create a default Render Target 2D Array Texture and assign the reference in the TurboSequence Lf -> Reference Paths Project settings and open ../Plugins/TurboSequence_Lf/Resources/MF_TurboSequence_PositionOffset_Lf and assign it into the Texture Object with the Transform Texture Comment"
			       ));
		}
		else
		{
			if (GlobalData->TransformTexture_CurrentFrame->SizeX != GlobalData->TransformTexture_PreviousFrame->SizeX ||
				GlobalData->TransformTexture_CurrentFrame->SizeY
				!= GlobalData->TransformTexture_PreviousFrame->SizeY || GlobalData->TransformTexture_PreviousFrame->
				Slices != GlobalData->TransformTexture_CurrentFrame->Slices || GlobalData->
				                                                               TransformTexture_PreviousFrame->
				                                                               GetFormat() != GlobalData->
				TransformTexture_CurrentFrame->GetFormat())
			{
				bAssetEdited = true;
				const UPackage* Package = GlobalData->TransformTexture_PreviousFrame->GetOutermost();
				const FString PackagePath = FPackageName::LongPackageNameToFilename(
					Package->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*PackagePath);
				GlobalData->TransformTexture_PreviousFrame = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(
					PackagePath, GlobalData->TransformTexture_PreviousFrame->GetName(),
					GlobalData->TransformTexture_CurrentFrame->SizeX, GlobalData->TransformTexture_CurrentFrame->Slices,
					GlobalData->TransformTexture_CurrentFrame->GetFormat());
			}
		}


		FString DefaultSkinWeightTextureReferencePath;
		FTurboSequence_Helper_Lf::GetStringConfigSetting(DefaultSkinWeightTextureReferencePath,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_RefSettings_Lf"),
		                                                 TEXT("Default_Rendering_TransformTexture"));
		if (DefaultSkinWeightTextureReferencePath.IsEmpty())
		{
			DefaultSkinWeightTextureReferencePath = FTurboSequence_Helper_Lf::ReferenceTurboSequenceSkinWeightTexture;
		}
		if (!IsValid(GlobalData->SkinWeightTexture))
		{
			bAssetEdited = true;
			GlobalData->SkinWeightTexture = FTurboSequence_Helper_Lf::LoadAssetFromReferencePath<
				UTextureRenderTarget2DArray>(DefaultSkinWeightTextureReferencePath);
		}
		if (!IsValid(GlobalData->SkinWeightTexture))
		{
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "Can not find Skin Weight Texture, it should at .../Plugins/TurboSequence_Lf/Resources/T_TurboSequence_SkinWeightTexture_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths, if it's not there please create a default Render Target 2D Array Texture and assign the reference in the TurboSequence Lf -> Reference Paths Project settings and open ../Plugins/TurboSequence_Lf/Resources/MF_TurboSequence_PositionOffset_Lf and assign it into the Texture Object with the Skin Weight Texture Comment"
			       ));
		}
		else
		{
			if (GlobalData->SkinWeightTexture->GetFormat() != GetPixelFormatFromRenderTargetFormat(RTF_RGBA16f))
			{
				bAssetEdited = true;
				const UPackage* Package = GlobalData->SkinWeightTexture->GetOutermost();
				const FString PackagePath = FPackageName::LongPackageNameToFilename(
					Package->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*PackagePath);
				GlobalData->SkinWeightTexture = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(
					PackagePath, GlobalData->SkinWeightTexture->GetName(), GET128_NUMBER, GET24_NUMBER,
					GetPixelFormatFromRenderTargetFormat(RTF_RGBA16f));
			}
		}

		FString DefaultDataTextureReferencePath;
		FTurboSequence_Helper_Lf::GetStringConfigSetting(DefaultDataTextureReferencePath,
		                                                 TEXT(
			                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_RefSettings_Lf"),
		                                                 TEXT("Default_Rendering_DataTexture"));
		if (DefaultDataTextureReferencePath.IsEmpty())
		{
			DefaultDataTextureReferencePath = FTurboSequence_Helper_Lf::ReferenceTurboSequenceDataTexture;
		}

		if (bAssetEdited)
		{
			FTurboSequence_Helper_Lf::SaveAsset(GlobalData);
		}
	}
	else
	{
		UE_LOG(LogTurboSequence_Lf, Error,
		       TEXT(
			       "Can not find the Global Data asset -> This is really bad, without it Turbo Sequence does not work, you can recover it by creating an UTurboSequence_GlobalData_Lf Data Asset, Right click in the content browser anywhere in the Project, select Data Asset and choose UTurboSequence_GlobalData_Lf, save it and restart the editor"
		       ));
		return;
	}


	AssetRegistry.Get().GetAssetsByClass(FTopLevelAssetPath(UTurboSequence_MeshAsset_Lf::StaticClass()->GetPathName()),
	                                     TurboSequence_MeshAssetData_AsyncComputeSwapBack);


	if (TurboSequence_MeshAssetData_AsyncComputeSwapBack.Num())
	{
		RepairMeshAssetAsync();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTurboSequence_Editor_LfModule, TurboSequence_Editor_Lf)
