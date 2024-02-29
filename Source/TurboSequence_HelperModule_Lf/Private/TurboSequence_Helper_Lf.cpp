// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.


#include "TurboSequence_Helper_Lf.h"

// #include "AssetToolsModule.h"
// #include "MeshUtilities.h"
// #include "AssetRegistry/AssetRegistryModule.h"
// #include "Dialogs/DlgPickAssetPath.h"
// #include "Internationalization/Internationalization.h"


// UStaticMesh* FTurboSequence_Helper_Lf::ConvertMeshesToStaticMesh(const TArray<UMeshComponent*>& InMeshComponents, const FTransform& InRootTransform, const FString& InPackageName)
// {
// 	UStaticMesh* StaticMesh = nullptr;
//
// 	// Build a package name to use
// 	FString PackageName = InPackageName;
// 	const FString MeshName = *FPackageName::GetLongPackageAssetName(PackageName);
//
// 	if(!PackageName.IsEmpty() && !MeshName.IsEmpty())
// 	{
// 		TArray<FRawMesh> RawMeshes;
// 		TArray<UMaterialInterface*> Materials;
//
// 		TArray<FRawMeshTracker> RawMeshTrackers;
//
// 		FMatrix WorldToRoot = InRootTransform.ToMatrixWithScale().Inverse();
//
// 		// first do a pass to determine the max LOD level we will be combining meshes into
// 		int32 OverallMaxLODs = 0;
// 		for (UMeshComponent* MeshComponent : InMeshComponents)
// 		{
// 			USkinnedMeshComponent* SkinnedMeshComponent = Cast<USkinnedMeshComponent>(MeshComponent);
// 			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent);
//
// 			if (IMeshUtilities::IsValidSkinnedMeshComponent(SkinnedMeshComponent))
// 			{
// 				OverallMaxLODs = FMath::Max(SkinnedMeshComponent->MeshObject->GetSkeletalMeshRenderData().LODRenderData.Num(), OverallMaxLODs);
// 			}
// 			else if(IsValidStaticMeshComponent(StaticMeshComponent))
// 			{
// 				OverallMaxLODs = FMath::Max(StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources.Num(), OverallMaxLODs);
// 			}
// 		}
// 		
// 		// Resize raw meshes to accommodate the number of LODs we will need
// 		RawMeshes.SetNum(OverallMaxLODs);
// 		RawMeshTrackers.SetNum(OverallMaxLODs);
//
// 		// Export all visible components
// 		for (UMeshComponent* MeshComponent : InMeshComponents)
// 		{
// 			FMatrix ComponentToWorld = MeshComponent->GetComponentTransform().ToMatrixWithScale() * WorldToRoot;
//
// 			USkinnedMeshComponent* SkinnedMeshComponent = Cast<USkinnedMeshComponent>(MeshComponent);
// 			UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent);
//
// 			if (IsValidSkinnedMeshComponent(SkinnedMeshComponent))
// 			{
// 				SkinnedMeshToRawMeshes(SkinnedMeshComponent, OverallMaxLODs, ComponentToWorld, PackageName, RawMeshTrackers, RawMeshes, Materials);
// 			}
// 			else if (IsValidStaticMeshComponent(StaticMeshComponent))
// 			{
// 				StaticMeshToRawMeshes(StaticMeshComponent, OverallMaxLODs, ComponentToWorld, PackageName, RawMeshTrackers, RawMeshes, Materials);
// 			}
// 		}
//
// 		uint32 MaxInUseTextureCoordinate = 0;
//
// 		// scrub invalid vert color & tex coord data
// 		check(RawMeshes.Num() == RawMeshTrackers.Num());
// 		for (int32 RawMeshIndex = 0; RawMeshIndex < RawMeshes.Num(); RawMeshIndex++)
// 		{
// 			if (!RawMeshTrackers[RawMeshIndex].bValidColors)
// 			{
// 				RawMeshes[RawMeshIndex].WedgeColors.Empty();
// 			}
//
// 			for (uint32 TexCoordIndex = 0; TexCoordIndex < MAX_MESH_TEXTURE_COORDS; TexCoordIndex++)
// 			{
// 				if (!RawMeshTrackers[RawMeshIndex].bValidTexCoords[TexCoordIndex])
// 				{
// 					RawMeshes[RawMeshIndex].WedgeTexCoords[TexCoordIndex].Empty();
// 				}
// 				else
// 				{
// 					// Store first texture coordinate index not in use
// 					MaxInUseTextureCoordinate = FMath::Max(MaxInUseTextureCoordinate, TexCoordIndex);
// 				}
// 			}
// 		}
//
// 		// Check if we got some valid data.
// 		bool bValidData = false;
// 		for (FRawMesh& RawMesh : RawMeshes)
// 		{
// 			if (RawMesh.IsValidOrFixable())
// 			{
// 				bValidData = true;
// 				break;
// 			}
// 		}
//
// 		if (bValidData)
// 		{
// 			// Then find/create it.
// 			UPackage* Package = CreatePackage( *PackageName);
// 			check(Package);
//
// 			// Create StaticMesh object
// 			StaticMesh = NewObject<UStaticMesh>(Package, *MeshName, RF_Public | RF_Standalone);
// 			StaticMesh->InitResources();
//
// 			StaticMesh->SetLightingGuid();
//
// 			// Determine which texture coordinate map should be used for storing/generating the lightmap UVs
// 			const uint32 LightMapIndex = FMath::Min(MaxInUseTextureCoordinate + 1, (uint32)MAX_MESH_TEXTURE_COORDS - 1);
//
// 			// Add source to new StaticMesh
// 			for (FRawMesh& RawMesh : RawMeshes)
// 			{
// 				if (RawMesh.IsValidOrFixable())
// 				{
// 					FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
// 					SrcModel.BuildSettings.bRecomputeNormals = false;
// 					SrcModel.BuildSettings.bRecomputeTangents = false;
// 					SrcModel.BuildSettings.bRemoveDegenerates = true;
// 					SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
// 					SrcModel.BuildSettings.bUseFullPrecisionUVs = false;
// 					SrcModel.BuildSettings.bGenerateLightmapUVs = true;
// 					SrcModel.BuildSettings.SrcLightmapIndex = 0;
// 					SrcModel.BuildSettings.DstLightmapIndex = LightMapIndex;
// 					SrcModel.SaveRawMesh(RawMesh);
// 				}
// 			}
//
// 			// Copy materials to new mesh 
// 			for(UMaterialInterface* Material : Materials)
// 			{
// 				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material));
// 			}
// 			
// 			//Set the Imported version before calling the build
// 			StaticMesh->ImportVersion = EImportStaticMeshVersion::LastVersion;
//
// 			// Set light map coordinate index to match DstLightmapIndex
// 			StaticMesh->SetLightMapCoordinateIndex(LightMapIndex);
//
// 			// setup section info map
// 			for (int32 RawMeshLODIndex = 0; RawMeshLODIndex < RawMeshes.Num(); RawMeshLODIndex++)
// 			{
// 				const FRawMesh& RawMesh = RawMeshes[RawMeshLODIndex];
// 				TArray<int32> UniqueMaterialIndices;
// 				for (int32 MaterialIndex : RawMesh.FaceMaterialIndices)
// 				{
// 					UniqueMaterialIndices.AddUnique(MaterialIndex);
// 				}
//
// 				int32 SectionIndex = 0;
// 				for (int32 UniqueMaterialIndex : UniqueMaterialIndices)
// 				{
// 					StaticMesh->GetSectionInfoMap().Set(RawMeshLODIndex, SectionIndex, FMeshSectionInfo(UniqueMaterialIndex));
// 					SectionIndex++;
// 				}
// 			}
// 			StaticMesh->GetOriginalSectionInfoMap().CopyFrom(StaticMesh->GetSectionInfoMap());
//
// 			// Build mesh from source
// 			StaticMesh->Build(false);
// 			StaticMesh->PostEditChange();
//
// 			StaticMesh->MarkPackageDirty();
//
// 			// Notify asset registry of new asset
// 			FAssetRegistryModule::AssetCreated(StaticMesh);
//
// 			// Display notification so users can quickly access the mesh
// 			if (GIsEditor)
// 			{
// 				FNotificationInfo Info(FText::Format(LOCTEXT("SkeletalMeshConverted", "Successfully Converted Mesh"), FText::FromString(StaticMesh->GetName())));
// 				Info.ExpireDuration = 8.0f;
// 				Info.bUseLargeFont = false;
// 				Info.Hyperlink = FSimpleDelegate::CreateLambda([=]() { GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAssets(TArray<UObject*>({ StaticMesh })); });
// 				Info.HyperlinkText = FText::Format(LOCTEXT("OpenNewAnimationHyperlink", "Open {0}"), FText::FromString(StaticMesh->GetName()));
// 				TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info);
// 				if ( Notification.IsValid() )
// 				{
// 					Notification->SetCompletionState( SNotificationItem::CS_Success );
// 				}
// 			}
// 		}
// 	}
//
// 	return StaticMesh;
// }
