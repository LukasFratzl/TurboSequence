// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#include "TurboSequence_Utility_Lf.h"

#include "MeshDescription.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshRenderData.h"


uint32 FTurboSequence_Utility_Lf::CreateRenderer(FSkinnedMeshReference_Lf& Reference,
                                                 const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
                                                 const TObjectPtr<UNiagaraSystem> RenderReference,
                                                 const TMap<uint8, FSkinnedMeshReferenceLodElement_Lf>& LevelOfDetails,
                                                 const TObjectPtr<USceneComponent> InstanceSceneComponent,
                                                 TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>
                                                 & RenderComponents,
                                                 const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
                                                 const TArray<TObjectPtr<UMaterialInterface>>& Materials,
                                                 uint32 MaterialsHash)
{
	if (RenderComponents.Contains(FromAsset) && RenderComponents[FromAsset].NiagaraRenderer.Contains(MaterialsHash))
	{
		return MaterialsHash;
	}

	// Create the Render Data first

	FRenderData_Lf RenderData = FRenderData_Lf(GlobalData->NameNiagaraEmitter,
	                                           GlobalData->NameNiagaraParticleIDMap,
	                                           GlobalData->NameNiagaraParticleLocations,
	                                           GlobalData->NameNiagaraParticleRotations,
	                                           GlobalData->NameNiagaraParticleScales,
	                                           GlobalData->NameNiagaraMeshObject,
	                                           GlobalData->NameNiagaraMaterialObject,
	                                           GlobalData->NameNiagaraLevelOfDetailIndex,
	                                           GlobalData->NameNiagaraCustomData,
	                                           GlobalData->NameNiagaraParticleRemove);

	RenderData.Materials = Materials;
	Reference.RenderData.Add(MaterialsHash, RenderData);
	Reference.RenderData[MaterialsHash].RenderReference = RenderReference;


	// Spawn the Niagara system on the actor
	const TObjectPtr<UNiagaraComponent> Component = UNiagaraFunctionLibrary::SpawnSystemAttached(
		Reference.RenderData[MaterialsHash].RenderReference,
		InstanceSceneComponent,
		FName("MeshLODRenderer"),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::KeepWorldPosition,
		false
	);
	if (IsValid(Component))
	{
		Component->SetVisibleInRayTracing(true);
	}

	TArray<FRenderingMaterialKeyValue_Lf> ConvertedMaterial;
	for (const TObjectPtr<UMaterialInterface>& Material : Materials)
	{
		FRenderingMaterialKeyValue_Lf Mat = FRenderingMaterialKeyValue_Lf();
		Mat.MaterialKey = Material;

		ConvertedMaterial.Add(Mat);
	}

	if (!RenderComponents.Contains(FromAsset))
	{
		FRenderingMaterialMap_Lf Map = FRenderingMaterialMap_Lf();
		FRenderingMaterialItem_Lf Item = FRenderingMaterialItem_Lf();
		Item.NiagaraRenderer = Component;
		Item.Materials = ConvertedMaterial;
		Map.NiagaraRenderer.Add(MaterialsHash, Item);
		RenderComponents.Add(FromAsset, Map);
	}
	else
	{
		FRenderingMaterialItem_Lf Item = FRenderingMaterialItem_Lf();
		Item.NiagaraRenderer = Component;
		Item.Materials = ConvertedMaterial;
		RenderComponents[FromAsset].NiagaraRenderer.Add(MaterialsHash, Item);
	}


	// Add the mesh to the component we just created
	for (const TTuple<uint8, FSkinnedMeshReferenceLodElement_Lf>& Lod : LevelOfDetails)
	{
		const FSkinnedMeshReferenceLodElement_Lf& LodElement = Lod.Value;

		if (LodElement.bIsRenderStateValid)
		{
			if (LodElement.GPUMeshIndex < FTurboSequence_Helper_Lf::NotVisibleMeshIndex)
			{
				const FName& WantedMeshName = FName(FString::Format(
					*Reference.RenderData[MaterialsHash].GetMeshName(),
					{*FString::FormatAsNumber(LodElement.GPUMeshIndex)}));

				if (!IsValid(RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer))
				{
					continue;
				}
				RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetVariableStaticMesh(
					WantedMeshName, LodElement.Mesh);
			}
		}
	}

	// Set the Materials
	int32 NumMaterials = Materials.Num();
	for (int32 MaterialIdx = GET0_NUMBER; MaterialIdx < NumMaterials; ++MaterialIdx)
	{
		const FName& WantedMaterialName = FName(FString::Format(
			*Reference.RenderData[MaterialsHash].GetMaterialsName(), {*FString::FormatAsNumber(MaterialIdx)}));


		TObjectPtr<UMaterialInstanceDynamic> MaterialInstance = UMaterialInstanceDynamic::Create(
			Materials[MaterialIdx], nullptr);

		// SkinWeight_Texture2DArray
		MaterialInstance->SetTextureParameterValue(FTurboSequence_Helper_Lf::NameMaterialParameterMeshDataTexture,
		                                           FromAsset->MeshDataTexture);

		// SkinWeight_Tex
		MaterialInstance->SetScalarParameterValue(
			FTurboSequence_Helper_Lf::NameMaterialParameterMeshDataTextureSizeX,
			FromAsset->MeshDataTexture->GetSizeX());
		MaterialInstance->SetScalarParameterValue(
			FTurboSequence_Helper_Lf::NameMaterialParameterMeshDataTextureSizeY,
			FromAsset->MeshDataTexture->GetSizeY());

		float DataMode = 0.0f;
		if (FromAsset->MeshDataMode == ETurboSequence_MeshDataMode_Lf::VertexColor)
		{
			DataMode = 1.0f;
		}
		MaterialInstance->SetScalarParameterValue(
			FTurboSequence_Helper_Lf::NameMaterialParameterMeshDataMode,
			DataMode);

		if (!IsValid(RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer))
		{
			continue;
		}
		RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetVariableMaterial(
			WantedMaterialName, MaterialInstance);

		// For Debugging nice...
		RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].Materials[MaterialIdx].MaterialValue =
			MaterialInstance;
	}

	// Set Bounds
	uint8 NumLevelOfDetails = LevelOfDetails.Num();
	for (int16 i = NumLevelOfDetails - GET1_NUMBER; i >= GET0_NUMBER; --i)
	{
		if (LevelOfDetails[i].bIsRenderStateValid)
		{
			// Basically 5000-Meter Radius
			constexpr float BoundsExtend = GET1000_NUMBER * GET1000_NUMBER / GET2_NUMBER;
			const FBox& Bounds = FBox(FVector::OneVector * -BoundsExtend, FVector::OneVector * BoundsExtend);

			if (!IsValid(RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer))
			{
				continue;
			}

			RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetEmitterFixedBounds(
				Reference.RenderData[MaterialsHash].GetEmitterName(), Bounds);
			break;
		}
	}

	return MaterialsHash;
}

void FTurboSequence_Utility_Lf::UpdateCameras(TArray<FCameraView_Lf>& OutView, const UWorld* InWorld,
                                              const TArray<FTurboSequence_CameraInfo_Lf>& CustomCameraInfo)
{
	const bool bHasCustomCameraData = static_cast<bool>(CustomCameraInfo.Num());
	const bool bIsSinglePlayer = UGameplayStatics::GetNumPlayerControllers(InWorld) < GET2_NUMBER &&
		CustomCameraInfo.Num() < GET2_NUMBER;
	int32 MaxNumberPlayerControllers = CustomCameraInfo.Num();
	if (!bHasCustomCameraData)
	{
		MaxNumberPlayerControllers = UGameplayStatics::GetNumPlayerControllers(InWorld);
	}
	OutView.SetNum(MaxNumberPlayerControllers);
	for (int32 ViewIdx = GET0_NUMBER; ViewIdx < MaxNumberPlayerControllers; ++ViewIdx)
	{
		FMinimalViewInfo PlayerMinimalViewInfo = FMinimalViewInfo();
		FVector2D LocalPlayerSize = FVector2D(0,0);
		FVector2D ViewportSize = FVector2D(0,0);
		float Fov = 90;
		TEnumAsByte<enum EAspectRatioAxisConstraint> ConstraintType;
		if (!bHasCustomCameraData)
		{
			const TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(
				InWorld, ViewIdx);
			if (!IsValid(PlayerController))
			{
				continue;
			}
			const TObjectPtr<ULocalPlayer> LocalPlayer = PlayerController->GetLocalPlayer();
			if (!IsValid(LocalPlayer))
			{
				continue;
			}

			PlayerController->PlayerCameraManager->UpdateCamera(GET0_NUMBER);

			PlayerMinimalViewInfo = PlayerController->PlayerCameraManager->ViewTarget.POV;

			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
			LocalPlayerSize = FVector2D(LocalPlayer->Size.X, LocalPlayer->Size.Y);
			Fov = PlayerController->PlayerCameraManager->GetFOVAngle();

			ConstraintType = LocalPlayer->AspectRatioAxisConstraint;
		}
		else
		{
			PlayerMinimalViewInfo = CustomCameraInfo[ViewIdx].ViewInfo;
			LocalPlayerSize = CustomCameraInfo[ViewIdx].LocalPlayerSize;
			Fov = CustomCameraInfo[ViewIdx].Fov;

			ConstraintType = CustomCameraInfo[ViewIdx].AspectRatioAxisConstraint;
		}

		FVector2f SplitScreenPlayerViewSize = FVector2f(LocalPlayerSize.X * ViewportSize.X,
		                                                LocalPlayerSize.Y * ViewportSize.Y);
		FVector2f ViewportDimensions = FVector2f(ViewportSize.X, ViewportSize.Y);

		if (!bIsSinglePlayer)
		{
			ViewportDimensions = FVector2f(SplitScreenPlayerViewSize.X / SplitScreenPlayerViewSize.Y);


			float AspectRatio;
			if (((ViewportSize.X > ViewportSize.Y) && (ConstraintType ==
				AspectRatio_MajorAxisFOV)) || (ConstraintType == AspectRatio_MaintainXFOV))
			{
				//if the viewport is wider than it is tall
				AspectRatio = ViewportDimensions.X / ViewportDimensions.Y;
			}
			else
			{
				//if the viewport is taller than it is wide
				AspectRatio = ViewportDimensions.Y / ViewportDimensions.X;
			}

			Fov = FMath::RadiansToDegrees(GET2_NUMBER * FMath::Atan(
				FMath::Tan(FMath::DegreesToRadians(Fov) * 0.5f) * AspectRatio));
		}

		FCameraView_Lf View;
		View.Fov = Fov; // * 1.1f;
		View.ViewportSize = ViewportDimensions;
		View.AspectRatioAxisConstraint = ConstraintType;
		View.bIsPerspective = PlayerMinimalViewInfo.ProjectionMode == ECameraProjectionMode::Perspective;
		View.OrthoWidth = PlayerMinimalViewInfo.OrthoWidth;
		View.FarClipPlane = PlayerMinimalViewInfo.OrthoFarClipPlane;
		// Near clipping plane need to have an offset backwards because otherwise characters disappear
		// in front of the camera
		View.NearClipPlane = GET100_NEGATIVE_NUMBER;
		View.CameraTransform = FTransform(PlayerMinimalViewInfo.Rotation, PlayerMinimalViewInfo.Location,
		                                  FVector::OneVector);

		OutView[ViewIdx] = View;
	}
}

void FTurboSequence_Utility_Lf::UpdateCameras_1(TArray<FCameraView_Lf>& OutViews,
                                                const TMap<uint8, FTransform>& LastFrameCameraTransforms,
                                                const UWorld* InWorld, float DeltaTime,
                                                const TArray<FTurboSequence_CameraInfo_Lf>& CustomCameraInfo)
{
	UpdateCameras(OutViews, InWorld, CustomCameraInfo);
	float Interpolation = GET1_NUMBER; //GET2_NUMBER + DeltaTime * GET4_NUMBER;
	uint8 NumCameraViews = OutViews.Num();
	for (uint8 i = GET0_NUMBER; i < NumCameraViews; ++i)
	{
		FCameraView_Lf& View = OutViews[i];

		float InterpolatedFov = View.Fov; // + GET5_NUMBER * DeltaTime;
		FTurboSequence_Helper_Lf::GetCameraFrustumPlanes_ObjectSpace(
			View.Planes_Internal, InterpolatedFov, View.ViewportSize, View.AspectRatioAxisConstraint,
			View.NearClipPlane, View.FarClipPlane,
			!View.bIsPerspective, View.OrthoWidth);

		if (LastFrameCameraTransforms.Contains(i))
		{
			const FVector InterpolatedCameraLocation = View.CameraTransform.GetLocation();
			const FQuat InterpolatedCameraRotation = FQuat::SlerpFullPath(
				LastFrameCameraTransforms[i].GetRotation(), View.CameraTransform.GetRotation(), Interpolation);

			View.InterpolatedCameraTransform_Internal = FTransform(InterpolatedCameraRotation,
			                                                       InterpolatedCameraLocation, FVector::OneVector);
		}
		else
		{
			View.InterpolatedCameraTransform_Internal = View.CameraTransform;
		}
	}
}

void FTurboSequence_Utility_Lf::UpdateCameras_2(TMap<uint8, FTransform>& OutLastFrameCameraTransforms,
                                                const TArray<FCameraView_Lf>& CameraViews)
{
	uint8 MaxNumberLastFrameCameras = FMath::Max(OutLastFrameCameraTransforms.Num(), CameraViews.Num());
	for (uint8 i = GET0_NUMBER; i < MaxNumberLastFrameCameras; ++i)
	{
		if (OutLastFrameCameraTransforms.Contains(i))
		{
			if (CameraViews.Num() <= i)
			{
				OutLastFrameCameraTransforms.Remove(i);
			}
			else
			{
				OutLastFrameCameraTransforms[i] = CameraViews[i].CameraTransform;
			}
		}
		else
		{
			OutLastFrameCameraTransforms.Add(i, CameraViews[i].CameraTransform);
		}
	}
}

int32 FTurboSequence_Utility_Lf::GetBoneMapIndex_CPU(const TArray<FBoneIndexType>& FromSection, int32 RawIndex)
{
	if (!FromSection.IsValidIndex(RawIndex))
	{
		return INDEX_NONE;
	}

	int32 MeshIndex = FromSection[RawIndex];

	return MeshIndex;
}

int32 FTurboSequence_Utility_Lf::GetBoneMapIndex_GPU(const TMap<uint16, uint16>& FromMap,
                                                     const TArray<FBoneIndexType>& FromSection, int32 RawIndex,
                                                     uint8 Weight)
{
	if (int32 ReferenceIndex = GetBoneMapIndex_CPU(FromSection, RawIndex); Weight && FromMap.Contains(
		ReferenceIndex))
	{
		return FromMap[ReferenceIndex];
	}
	return GET0_NUMBER;
}

int32 FTurboSequence_Utility_Lf::GetValidBoneData(int32 FromData, uint8 CurrentInfluenceIndex, uint8 MaxInfluenceIndex)
{
	if (CurrentInfluenceIndex <= MaxInfluenceIndex)
	{
		return FromData;
	}
	return INDEX_NONE;
}

// void FTurboSequence_Utility_Lf::CacheTurboSequenceAssets(FSkinnedMeshGlobalLibrary_Lf& Library,
//                                                          FSkinnedMeshGlobalLibrary_RenderThread_Lf&
//                                                          Library_RenderThread, FCriticalSection& CriticalSection)
// {
// 	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
// 		"AssetRegistry");
// 	TArray<FAssetData> AssetClasses;
// 	AssetRegistry.Get().GetAssetsByClass(
// 		FTopLevelAssetPath(UTurboSequence_MeshAsset_Lf::StaticClass()->GetPathName()), AssetClasses);
//
// 	int32 NumAssets = AssetClasses.Num();
// 	FTurboSequence_Helper_Lf::SortAssetsByPathName(AssetClasses);
//
// 	for (int32 AssetIdx = GET0_NUMBER; AssetIdx < NumAssets; ++AssetIdx)
// 	{
// 		const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Cast<UTurboSequence_MeshAsset_Lf>(
// 			AssetClasses[AssetIdx].GetAsset());
// 		if (!Library.PerReferenceData.Contains(Asset))
// 		{
// 			CreateTurboSequenceReference(Library, Library_RenderThread, CriticalSection, Asset);
// 		}
// 	}
// }

void FTurboSequence_Utility_Lf::CreateAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
                                                           const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
                                                           FSkinnedMeshGlobalLibrary_Lf& Library,
                                                           FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                           Library_RenderThread, FCriticalSection& CriticalSection,
                                                           const TObjectPtr<UWorld> InWorld)
{
	CreateTurboSequenceReference(Library, Library_RenderThread, CriticalSection, FromAsset, InWorld);
	if (Library.PerReferenceData.Contains(FromAsset))
	{
		RefreshAsyncChunkedMeshData(GlobalData, Library, Library_RenderThread, CriticalSection);
	}
}

void FTurboSequence_Utility_Lf::RefreshAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
                                                            FSkinnedMeshGlobalLibrary_Lf& Library,
                                                            FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                            Library_RenderThread, FCriticalSection& CriticalSection)
{
	CreateBoneMaps(Library, Library_RenderThread, CriticalSection);
	CreateInverseReferencePose(Library, Library_RenderThread, CriticalSection);
}

void FTurboSequence_Utility_Lf::CreateTurboSequenceReference(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                             FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                             Library_RenderThread, FCriticalSection& CriticalSection,
                                                             const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
                                                             const TObjectPtr<UWorld> InWorld)
{
	if (!IsValid(FromAsset))
	{
		return;
	}

	if (!IsValid(FromAsset->GetSkeleton()))
	{
		return;
	}

	if (!FromAsset->MeshData.Num())
	{
		return;
	}

	if (!FromAsset->InstancedMeshes.Num())
	{
		return;
	}

	if (FromAsset->bExcludeFromSystem)
	{
		return;
	}

	FSkinnedMeshReference_Lf Reference = FSkinnedMeshReference_Lf(FromAsset);
	Reference.FromWorld = InWorld;
	FSkinnedMeshReference_RenderThread_Lf Reference_RenderThread = FSkinnedMeshReference_RenderThread_Lf();


	CreateLevelOfDetails(Reference, Reference_RenderThread, CriticalSection, FromAsset);

	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(FromAsset);
	const TArray<FTransform>& ReferencePose = GetSkeletonRefPose(ReferenceSkeleton);
	Reference.FirstReferenceBone = ReferencePose[GET0_NUMBER];

	CriticalSection.Lock();
	Library.PerReferenceDataKeys.Add(FromAsset);
	Reference_RenderThread.DataAsset = FromAsset;
	Library.PerReferenceData.Add(FromAsset, Reference);
	CriticalSection.Unlock();

	ENQUEUE_RENDER_COMMAND(TurboSequence_AddRenderThreadReference_Lf)(
		[&Library_RenderThread, Reference_RenderThread](FRHICommandListImmediate& RHICmdList)
		{
			Library_RenderThread.PerReferenceDataKeys.Add(Reference_RenderThread.DataAsset);
			Library_RenderThread.PerReferenceData.Add(Reference_RenderThread.DataAsset, Reference_RenderThread);

			const int16 NumReferences = Library_RenderThread.PerReferenceData.Num();
			for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
			{
				FSkinnedMeshReference_RenderThread_Lf& ReferenceOther = Library_RenderThread.PerReferenceData[
					Library_RenderThread.PerReferenceDataKeys[i]];

				ReferenceOther.ReferenceCollectionIndex = i;
			}
		});

	const int16 NumReferences = Library.PerReferenceData.Num();
	for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
	{
		FSkinnedMeshReference_Lf& ReferenceOther = Library.PerReferenceData[Library.PerReferenceDataKeys[i]];

		ReferenceOther.ReferenceCollectionIndex = i;
	}

	UE_LOG(LogTurboSequence_Lf, Display, TEXT("Adding Mesh Asset to Library at Path | -> %s"),
	       *FromAsset->GetPathName());
}

void FTurboSequence_Utility_Lf::CreateLevelOfDetails(FSkinnedMeshReference_Lf& Reference,
                                                     FSkinnedMeshReference_RenderThread_Lf& Reference_RenderThread,
                                                     FCriticalSection& CriticalSection,
                                                     const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
                                                     bool bIsMeshDataEvaluationFunction)
{
	if (bIsMeshDataEvaluationFunction)
	{
		FromAsset->MeshData.Empty();
	}

	uint8 MaxNumLevelOfDetails = FromAsset->InstancedMeshes.Num();
	int32 MaxLevelOfDetailRange = FromAsset->AutoLodRatio;
	uint32 SkinWightOffsetIndex = GET0_NUMBER; //FTurboSequence_Helper_Lf::NumCustomStates; Refactor to Manager
	//uint8 GPUIndex = GET0_NUMBER;
	uint8 MeshIndex = GET0_NUMBER;
	for (uint8 i = GET0_NUMBER; i < MaxNumLevelOfDetails; ++i)
	{
		FSkinnedMeshReferenceLodElement_Lf LodElement = FSkinnedMeshReferenceLodElement_Lf();
		LodElement.CollectionIndex = i;

		FMeshData_Lf MeshData = FMeshData_Lf();

		/*
		    < - - - - - - - - - - - - - - - - - - - - >
		                    LOD LOGIC
		    < - - - - - - - - - - - - - - - - - - - - >
		*/

		const float PreviousLinearPercentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
			MeshIndex, GET0_NUMBER, MaxNumLevelOfDetails - GET1_NUMBER);
		const float LinearPercentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
			MeshIndex + GET1_NUMBER, GET0_NUMBER, MaxNumLevelOfDetails - GET1_NUMBER);
		LodElement.MinLodDistance = (!MeshIndex ? GET0_NUMBER : FromAsset->HighestDetailDrawDistance) +
			MaxLevelOfDetailRange * MeshIndex * PreviousLinearPercentage;
		LodElement.MaxLodDistance = FromAsset->HighestDetailDrawDistance + MaxLevelOfDetailRange * (MeshIndex +
			GET1_NUMBER) * LinearPercentage;

		/*
		    < - - - - - - - - - - - - - - - - - - - - >
		                    RENDER LOGIC
		    < - - - - - - - - - - - - - - - - - - - - >
		*/

		LodElement.bIsFrustumCullingEnabled = FromAsset->InstancedMeshes[i].bIsFrustumCullingEnabled;

		const bool bIsIncluded = !FromAsset->InstancedMeshes[i].bExcludeLodFromSystem ||
			bIsMeshDataEvaluationFunction;

		const bool bIsMeshVisible = IsValid(FromAsset->InstancedMeshes[i].StaticMesh) && FromAsset->InstancedMeshes
			[i].bShowMesh;

		if (bIsMeshVisible && bIsIncluded)
		{
			// Get the Mesh Index so the Skin FinalAnimationWeight AnimationOutputTexture can adapt the data
			uint8 NumLevelOfDetails = FromAsset->MeshData.Num();
			if (bIsMeshDataEvaluationFunction)
			{
				NumLevelOfDetails = FromAsset->ReferenceMeshEdited->GetLODNum();
			}
			uint32 NumVerticesInstancedMesh = FromAsset->InstancedMeshes[i].StaticMesh->GetNumVertices(
				GET0_NUMBER);
			bool bLodValid = false;
			//uint32 GPUSkinWeightOffset = GET0_NUMBER;
			for (int16 LodIdx = GET0_NUMBER; LodIdx < NumLevelOfDetails; ++LodIdx)
			{
				uint32 NumVertices = GET0_NUMBER;
				if (bIsMeshDataEvaluationFunction)
				{
					if (FromAsset->ReferenceMeshEdited->IsValidLODIndex(LodIdx))
					{
						NumVertices = FromAsset->ReferenceMeshEdited->GetResourceForRendering()->LODRenderData[
								LodIdx].
							GetNumVertices();

						MeshData.NumVertices = NumVertices;
					}
					else
					{
						MeshData.NumVertices = GET0_NUMBER;
					}
				}
				else
				{
					NumVertices = FromAsset->MeshData[LodIdx].NumVertices;
				}
				if (NumVerticesInstancedMesh == NumVertices)
				{
					LodElement.MeshIndex = LodIdx;
					LodElement.SkinWeightOffset = SkinWightOffsetIndex;
					// Per Lod
					//GPUSkinWeightOffset = SkinWightOffsetIndex;
					SkinWightOffsetIndex += NumVertices * FTurboSequence_Helper_Lf::NumSkinWeightPixels;
					bLodValid = true;
					if (!IsValid(Reference.FirstValidMeshLevelOfDetail))
					{
						Reference.FirstValidMeshLevelOfDetail = FromAsset->InstancedMeshes[i].StaticMesh;
					}
					LodElement.Mesh = FromAsset->InstancedMeshes[i].StaticMesh;
					break;
				}
			}
			LodElement.bIsRenderStateValid = bLodValid;
			if (LodElement.bIsRenderStateValid && bIsIncluded)
			{
				LodElement.GPUMeshIndex = Reference.NumLevelOfDetailsWithMesh;
				Reference.NumLevelOfDetailsWithMesh++;
			}
		}
		if ((!bIsMeshVisible || !bIsIncluded) && FromAsset->MeshData.IsValidIndex(LodElement.CollectionIndex))
		{
			SkinWightOffsetIndex += FromAsset->MeshData[LodElement.CollectionIndex].NumVertices *
				FTurboSequence_Helper_Lf::NumSkinWeightPixels;
		}

		LodElement.bIsAnimated = FromAsset->InstancedMeshes[i].bIsAnimated;
		if (bIsIncluded)
		{
			CriticalSection.Lock();
			Reference.LevelOfDetails.Add(MeshIndex, LodElement);
			if (bIsMeshDataEvaluationFunction)
			{
				FromAsset->MeshData.Add(MeshData);
			}
			CriticalSection.Unlock();

			MeshIndex++;
		}
	}
}

void FTurboSequence_Utility_Lf::CreateMaxLodAndCPUBones(FSkinnedMeshGlobalLibrary_Lf& Library)
{
	Library.MaxNumCPUBones = GET0_NUMBER;
	Library.MaxNumLevelOfDetailsWithMesh = GET0_NUMBER;
	for (const TTuple<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_Lf>& ReferenceOther : Library.
	     PerReferenceData)
	{
		if (int32 NumBones = GetSkeletonNumBones(GetReferenceSkeleton(ReferenceOther.Key)); Library.
			MaxNumCPUBones < NumBones)
		{
			Library.MaxNumCPUBones = NumBones;
		}
		if (int32 NumLevelOfDetails = ReferenceOther.Value.NumLevelOfDetailsWithMesh; Library.
			MaxNumLevelOfDetailsWithMesh < NumLevelOfDetails)
		{
			Library.MaxNumLevelOfDetailsWithMesh = NumLevelOfDetails;
		}
	}
}

void FTurboSequence_Utility_Lf::CreateGPUBones(FSkinnedMeshReferenceLodElement_Lf& LodElement,
                                               FCriticalSection& CriticalSection,
                                               const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
                                               bool bIsMeshDataEvaluation)
{
	if (!LodElement.CPUBoneToGPUBoneIndicesMap.Num())
	{
		if (bIsMeshDataEvaluation)
		{
			const FSkeletalMeshLODRenderData& RenderData = Asset->ReferenceMeshEdited->GetResourceForRendering()->
			                                                      LODRenderData[LodElement.MeshIndex];
			const FSkinWeightVertexBuffer* SkinWeightBuffer = RenderData.GetSkinWeightVertexBuffer();
			uint32 NumVertices = RenderData.GetNumVertices();
			for (uint32 i = GET0_NUMBER; i < NumVertices; ++i)
			{
				int32 VertexIndexSection;
				int32 SectionIndex;
				RenderData.GetSectionFromVertexIndex(i, SectionIndex, VertexIndexSection);

				const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];
				uint32 VertIdx = Section.BaseVertexIndex + VertexIndexSection;

				for (uint16 InfluenceIdx = GET0_NUMBER; InfluenceIdx < GET12_NUMBER; ++InfluenceIdx)
				{
					if (int32 BoneIndex =
							GetBoneMapIndex_CPU(Section.BoneMap, SkinWeightBuffer->GetBoneIndex(VertIdx, InfluenceIdx));
						SkinWeightBuffer->GetBoneWeight(VertIdx, InfluenceIdx) && BoneIndex > INDEX_NONE && !LodElement.
						CPUBoneToGPUBoneIndicesMap.Contains(BoneIndex))
					{
						CriticalSection.Lock();
						LodElement.CPUBoneToGPUBoneIndicesMap.Add(BoneIndex, GET0_NUMBER);
						CriticalSection.Unlock();
					}
				}
			}

			CriticalSection.Lock();
			// This actually generates the bones in BoneTree formation, I hope it's robust enough ...
			FTurboSequence_Helper_Lf::SortKeyFromStartLowToEndHigh(LodElement.CPUBoneToGPUBoneIndicesMap);

			Asset->MeshData[LodElement.CollectionIndex].CPUBoneToGPUBoneIndicesMap.Empty();

			uint16 BoneIndex = GET0_NUMBER;
			for (TTuple<uint16, uint16>& BoneIndicesMap : LodElement.CPUBoneToGPUBoneIndicesMap)
			{
				BoneIndicesMap.Value = BoneIndex;
				BoneIndex++;

				Asset->MeshData[LodElement.CollectionIndex].CPUBoneToGPUBoneIndicesMap.Add(
					BoneIndicesMap.Key, BoneIndicesMap.Value);
			}
			CriticalSection.Unlock();
		}
		else
		{
			if (Asset->MeshData.Num())
			{
				CriticalSection.Lock();
				for (const TTuple<int32, int32>& BoneIndicesMap : Asset->MeshData[LodElement.CollectionIndex].
				     CPUBoneToGPUBoneIndicesMap)
				{
					LodElement.CPUBoneToGPUBoneIndicesMap.Add(BoneIndicesMap.Key, BoneIndicesMap.Value);
				}
				CriticalSection.Unlock();
			}
			else
			{
				UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Not Valid Mesh Data..."));
			}
		}
	}
}

void FTurboSequence_Utility_Lf::CreateBoneMaps(FSkinnedMeshGlobalLibrary_Lf& Library,
                                               FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                               FCriticalSection& CriticalSection)
{
	if (Library.PerReferenceData.Num())
	{
		const int16 PreviousNumCPUBones = Library.MaxNumCPUBones;
		CreateMaxLodAndCPUBones(Library);

		if (PreviousNumCPUBones != Library.MaxNumCPUBones)
		{
			Library.bMaxNumCPUBonesChanged = true;
		}

		int16 MaxNumCPUBones = Library.MaxNumCPUBones;
		int16 MaxNumMeshLevelOfDetails = Library.MaxNumLevelOfDetailsWithMesh;
		int32 MaxNumGPUBones = GET0_NUMBER;

		ENQUEUE_RENDER_COMMAND(TurboSequence_UpdateBoneCollection_Lf)(
			[&Library_RenderThread, MaxNumCPUBones, MaxNumMeshLevelOfDetails](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.BoneTransformParams.NumMaxCPUBones = MaxNumCPUBones;
				Library_RenderThread.BoneTransformParams.NumMaxLevelOfDetails = MaxNumMeshLevelOfDetails;

				int32 NumReferences = Library_RenderThread.PerReferenceData.Num();
				Library_RenderThread.BoneTransformParams.ReferenceNumCPUBones.SetNum(NumReferences);
				for (int32 i = GET0_NUMBER; i < NumReferences; ++i)
				{
					FSkinnedMeshReference_RenderThread_Lf& Reference = Library_RenderThread.PerReferenceData[
						Library_RenderThread.PerReferenceDataKeys[i]];

					int32 NumCpuBones = GetSkeletonNumBones(GetReferenceSkeleton(Reference.DataAsset));
					Library_RenderThread.BoneTransformParams.ReferenceNumCPUBones[i] = NumCpuBones;
				}
			});

		int16 NumReferences = Library.PerReferenceData.Num();

		TArray<FVector4f> CachedIndices;
		int32 NumBoneIndices = NumReferences * MaxNumCPUBones * MaxNumMeshLevelOfDetails;
		CachedIndices.AddUninitialized(NumBoneIndices);
		for (int32 RefIdx = GET0_NUMBER; RefIdx < NumReferences; ++RefIdx)
		{
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Library.PerReferenceDataKeys[RefIdx];
			FSkinnedMeshReference_Lf& Reference = Library.PerReferenceData[Asset];
			Reference.NumCPUBones = GetSkeletonNumBones(GetReferenceSkeleton(Reference.DataAsset));

			const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Asset);
			//FromAsset->ReferenceSkeleton->GetMeshBoneIndexFromSkeletonBoneIndex()
			uint8 NumLevelOfDetails = Reference.LevelOfDetails.Num();
			int16 FirstGPULodCollectionIndex = NumLevelOfDetails - GET1_NUMBER;
			bool bHasRenderState = false;
			ParallelFor(NumLevelOfDetails, [&](int32 LodIdx)
			{
				FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[LodIdx];
				if (!LodElement.bIsRenderStateValid)
				{
					return;
				}

				CriticalSection.Lock();
				if (LodIdx < FirstGPULodCollectionIndex)
				{
					FirstGPULodCollectionIndex = LodIdx;
				}
				bHasRenderState = true;
				CriticalSection.Unlock();

				CreateGPUBones(LodElement, CriticalSection, Asset, false);

				int32 BaseIndex = RefIdx * MaxNumMeshLevelOfDetails * MaxNumCPUBones + LodElement.GPUMeshIndex *
					MaxNumCPUBones;
				for (uint16 i = GET0_NUMBER; i < MaxNumCPUBones; ++i)
				{
					FVector4f Data;
					if (GetSkeletonIsValidIndex(ReferenceSkeleton, i))
					{
						Data.X = i;
						if (LodElement.CPUBoneToGPUBoneIndicesMap.Contains(i))
						{
							Data.Y = LodElement.CPUBoneToGPUBoneIndicesMap[i];
						}
						else
						{
							Data.Y = INDEX_NONE;
						}
						Data.Z = GetSkeletonParentIndex(ReferenceSkeleton, i);
						Data.W = GET0_NUMBER; // Reserved
					}
					else // It's not part of the rig anymore, so tell the Compute Shader to not compute it
					{
						Data.X = INDEX_NONE;
						Data.Y = INDEX_NONE;
						Data.Z = INDEX_NONE;
						Data.W = GET0_NUMBER;
					}
					CachedIndices[BaseIndex + i] = Data;
				}
			}, EParallelForFlags::BackgroundPriority);


			if (!Reference.FirstLodGPUBonesCollection.Num() && bHasRenderState)
			{
				const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[
					FirstGPULodCollectionIndex];

				Reference.NumFirstLodGPUBones = LodElement.CPUBoneToGPUBoneIndicesMap.Num();
				Reference.FirstLodGPUBonesCollection = LodElement.CPUBoneToGPUBoneIndicesMap;

				TMap<uint16, uint16> GPUBoneCollection = LodElement.CPUBoneToGPUBoneIndicesMap;
				ENQUEUE_RENDER_COMMAND(TurboSequence_UpdateGPUBoneCollection_Lf)(
					[&Library_RenderThread, Asset, GPUBoneCollection](FRHICommandListImmediate& RHICmdList)
					{
						Library_RenderThread.PerReferenceData[Asset].FirstLodGPUBonesCollection = GPUBoneCollection;
					});
			}

			if (int32 NumBones = Reference.NumFirstLodGPUBones; MaxNumGPUBones <= NumBones)
			{
				MaxNumGPUBones = NumBones;
			}
		}

		Library.MaxNumGPUBones = MaxNumGPUBones;
		ENQUEUE_RENDER_COMMAND(TurboSequence_FillBoneCollection_Lf)(
			[&Library_RenderThread, CachedIndices, MaxNumGPUBones](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.BoneTransformParams.NumMaxGPUBones = MaxNumGPUBones;
				Library_RenderThread.BoneTransformParams.Indices = CachedIndices;
			});
	}
}

void FTurboSequence_Utility_Lf::CreateRawSkinWeightTextureBuffer(
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset, const TFunction<void(bool bSuccess)>& PostCall,
	const TObjectPtr<UWorld> World)
{
#if WITH_EDITOR
	if (!IsValid(FromAsset->GlobalData))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Global data is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	if (!IsValid(FromAsset->GlobalData->SkinWeightTexture))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Skin Weight Texture is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	if (!IsValid(FromAsset->ReferenceMeshEdited))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Reference Mesh is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams = FSettingsComputeShader_Params_Lf();
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.bUse32BitTexture = false;
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.ShaderID = GetTypeHash(FromAsset);
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.Empty();

	FCriticalSection CriticalSection;

	FSkinnedMeshReference_Lf Reference = FSkinnedMeshReference_Lf(FromAsset);
	FSkinnedMeshReference_RenderThread_Lf Reference_RenderThread = FSkinnedMeshReference_RenderThread_Lf();

	CreateLevelOfDetails(Reference, Reference_RenderThread, CriticalSection, FromAsset, true);

	int32 NumPixels = GET0_NUMBER;
	for (const TTuple<uint8, FSkinnedMeshReferenceLodElement_Lf>& LevelOfDetail : Reference.LevelOfDetails)
	{
		if (!LevelOfDetail.Value.bIsRenderStateValid)
		{
			continue;
		}

		const FSkeletalMeshLODRenderData& RenderData = FromAsset->ReferenceMeshEdited->GetResourceForRendering()->
		                                                          LODRenderData[LevelOfDetail.Value.MeshIndex];
		uint32 NumVertices = RenderData.GetNumVertices();

		NumPixels += NumVertices * FTurboSequence_Helper_Lf::NumSkinWeightPixels;
	}
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.AddUninitialized(NumPixels);

	uint8 NumLevelOfDetails = Reference.LevelOfDetails.Num();
	ParallelFor(NumLevelOfDetails, [&](int32 LodIdx)
	{
		FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[LodIdx];
		if (!LodElement.bIsRenderStateValid)
		{
			return;
		}

		if (!FromAsset->MeshDataOrderView.IsValidIndex(LodElement.MeshIndex))
		{
			return;
		}

		CreateGPUBones(LodElement, CriticalSection, FromAsset, true);

		const FSkeletalMeshLODRenderData& RenderData = FromAsset->ReferenceMeshEdited->GetResourceForRendering()->
		                                                          LODRenderData[LodElement.MeshIndex];
		const FSkinWeightVertexBuffer* SkinWeightBuffer = RenderData.GetSkinWeightVertexBuffer();


		const TArray<int32>& VertexIndices = FromAsset->MeshDataOrderView[LodElement.MeshIndex].StaticMeshIndices;

		int32 NumVertices = VertexIndices.Num();
		for (int32 VertIdx = GET0_NUMBER; VertIdx < NumVertices; ++VertIdx)
		{
			int32 RealVertexIndex = VertexIndices[VertIdx];

			int32 VertexIndexSection;
			int32 SectionIndex;
			RenderData.GetSectionFromVertexIndex(RealVertexIndex, SectionIndex, VertexIndexSection);


			const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];

			//uint8 MaxSkinWeightVertex = GET0_NUMBER;
			for (uint16 InfluenceChunkIdx = GET0_NUMBER; InfluenceChunkIdx < GET3_NUMBER; ++InfluenceChunkIdx)
			{
				uint8 ChunkIndex = InfluenceChunkIdx * GET4_NUMBER; // 12 SkinData

				FVector4f Weights;
				FVector4f Indices;
				for (int32 WeightIdx = GET0_NUMBER; WeightIdx < GET4_NUMBER; ++WeightIdx)
				{
					uint16 RawWeight = SkinWeightBuffer->GetBoneWeight(RealVertexIndex, ChunkIndex + WeightIdx);

					float WeightFloat = static_cast<float>(RawWeight) / static_cast<float>(0xFFFF);

					uint8 Weight = WeightFloat * 0xFF;

					Weights[WeightIdx] = Weight;

					int32 RawBoneIndex = SkinWeightBuffer->GetBoneIndex(RealVertexIndex, ChunkIndex + WeightIdx);

					int32 IndexedBoneIndex = GetBoneMapIndex_GPU(LodElement.CPUBoneToGPUBoneIndicesMap,
					                                             Section.BoneMap, RawBoneIndex, Weight);


					Indices[WeightIdx] = IndexedBoneIndex;

					// if (Weight)
					// {
					// 	MaxSkinWeightVertex = ChunkIndex + WeightIdx;
					// }
				}


				FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
						RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels + InfluenceChunkIdx *
						GET2_NUMBER] =
					Indices;
				FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
					RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels + InfluenceChunkIdx * GET2_NUMBER +
					GET1_NUMBER] = Weights;
			}

			FVector4f PerVertexCustomData_0;
			PerVertexCustomData_0.X = GET12_NUMBER;
			PerVertexCustomData_0.Y = GET0_NUMBER;
			PerVertexCustomData_0.Z = GET0_NUMBER;
			PerVertexCustomData_0.W = GET0_NUMBER;

			FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
				RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels +
				FTurboSequence_Helper_Lf::NumSkinWeightPixels - GET1_NUMBER] = PerVertexCustomData_0;
		}
	}, EParallelForFlags::BackgroundPriority);

	const int32 Slice = FMath::Min(
		FMath::CeilToInt(
			static_cast<float>(FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.Num()) /
			static_cast<float>(GET128_NUMBER * GET128_NUMBER)), 1023);

	FromAsset->GlobalData->SkinWeightTexture->Init(GET128_NUMBER, GET128_NUMBER, Slice, PF_FloatRGBA);
	FromAsset->GlobalData->SkinWeightTexture->UpdateResourceImmediate(true);

	FromAsset->MeshDataCustomData = GET24_NUMBER;

	FTimerDelegate WaitTimerCallback;
	WaitTimerCallback.BindLambda([FromAsset, PostCall, World, WaitTimerCallback]
	{
		FromAsset->MeshDataCustomData--;
		if (FromAsset->MeshDataCustomData < GET0_NUMBER)
		{
			PostCall(false);
			return;
		}

		if (!FromAsset->GlobalData->SkinWeightTexture->HasPendingInitOrStreaming(true) && !FromAsset->GlobalData->
			SkinWeightTexture->HasPendingRenderResourceInitialization())
		{
			bool bValidPixels = true;
			if (bValidPixels)
			{
				ENQUEUE_RENDER_COMMAND(TurboSequence_FillRenderThreadSkinWeightData_Lf)(
					[FromAsset](FRHICommandListImmediate& RHICmdList)
					{
						FSettingsCompute_Shader_Execute_Lf::DispatchRenderThread(
							RHICmdList, FromAsset->GlobalData->CachedMeshDataCreationSettingsParams,
							FromAsset->GlobalData->SkinWeightTexture);
					});

				ENQUEUE_RENDER_COMMAND(TurboSequence_FillRenderThreadSkinWeightData_Iteration2_Lf)(
					[PostCall, World, FromAsset](FRHICommandListImmediate& RHICmdList)
					{
						AsyncTask(ENamedThreads::GameThread, [PostCall, World, FromAsset]
						{
							FTimerDelegate WaitTimer2Callback;
							WaitTimer2Callback.BindLambda([FromAsset, PostCall, World, WaitTimer2Callback]
							{
								FromAsset->MeshDataCustomData--;
								if (FromAsset->MeshDataCustomData < GET0_NUMBER)
								{
									FromAsset->GlobalData->CachedMeshDataCreationSettingsParams =
										FSettingsComputeShader_Params_Lf();
									PostCall(false);
									return;
								}

								if (!FromAsset->GlobalData->SkinWeightTexture->HasPendingInitOrStreaming(true) && !
									FromAsset->GlobalData->SkinWeightTexture->HasPendingRenderResourceInitialization())
								{
									FromAsset->GlobalData->CachedMeshDataCreationSettingsParams =
										FSettingsComputeShader_Params_Lf();
									PostCall(true);
								}
								else
								{
									World->GetTimerManager().SetTimerForNextTick(WaitTimer2Callback);
								}
							});
							World->GetTimerManager().SetTimerForNextTick(WaitTimer2Callback);
						});
					});
			}
			else
			{
				World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);
			}
		}
		else
		{
			World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);
		}
	});
	World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);

#endif
}

void FTurboSequence_Utility_Lf::CreateInverseReferencePose(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                           FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                           Library_RenderThread, FCriticalSection& CriticalSection)
{
	int32 NumReferences = Library.PerReferenceData.Num();
	TArray<FVector4f> InvRefPoseData;

	if (int32 RestPoseBufferAddition = NumReferences * Library.MaxNumCPUBones * GET3_NUMBER)
	{
		CriticalSection.Lock();
		InvRefPoseData.AddUninitialized(RestPoseBufferAddition);
		CriticalSection.Unlock();

		//FCriticalSection Mutex;
		ParallelFor(NumReferences, [&](int32 Index)
		{
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Library.PerReferenceDataKeys[Index];
			FSkinnedMeshReference_Lf& Reference = Library.PerReferenceData[Asset];

			uint16 NumBones = Reference.NumCPUBones;
			CriticalSection.Lock();
			Reference.ComponentSpaceRestPose.AddUninitialized(NumBones);
			CriticalSection.Unlock();
			const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Asset);
			const TArray<FTransform>& ReferencePose = GetSkeletonRefPose(ReferenceSkeleton);
			int32 InvRestPoseBaseIndex = Index * Library.MaxNumCPUBones * GET3_NUMBER;
			for (uint16 BoneIdx = GET0_NUMBER; BoneIdx < NumBones; ++BoneIdx)
			{
				int32 RuntimeIndex = BoneIdx;
				FTransform RuntimeTransform = FTransform::Identity;
				while (RuntimeIndex != INDEX_NONE)
				{
					RuntimeTransform *= ReferencePose[RuntimeIndex];
					RuntimeIndex = GetSkeletonParentIndex(ReferenceSkeleton, RuntimeIndex);
				}
				Reference.ComponentSpaceRestPose[BoneIdx] = RuntimeTransform;
				const FMatrix InvBoneMatrix = RuntimeTransform.Inverse().ToMatrixWithScale();
				for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
				{
					FVector4f Row;
					Row.X = InvBoneMatrix.M[GET0_NUMBER][M];
					Row.Y = InvBoneMatrix.M[GET1_NUMBER][M];
					Row.Z = InvBoneMatrix.M[GET2_NUMBER][M];
					Row.W = InvBoneMatrix.M[GET3_NUMBER][M];

					InvRefPoseData[InvRestPoseBaseIndex + BoneIdx * GET3_NUMBER + M] = Row;
				}
			}
		}, EParallelForFlags::BackgroundPriority);
	}

	ENQUEUE_RENDER_COMMAND(TurboSequence_AddInverseReferencePoses_Lf)(
		[&Library_RenderThread, InvRefPoseData](FRHICommandListImmediate& RHICmdList)
		{
			Library_RenderThread.BoneTransformParams.CPUInverseReferencePose = InvRefPoseData;
		});
}

void FTurboSequence_Utility_Lf::ResizeBuffers(FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library, int32 Num)
{
	// Use this fast first for the bounce check on the render thread
	Library.BoneTransformParams.PerMeshCustomDataIndex_RenderThread.SetNum(Num);

	// Use this for the last one as check in the Render Thread
	Library.BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread.SetNum(Num);
}

bool FTurboSequence_Utility_Lf::IsValidBufferIndex(const FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
                                                   int32 Index)
{
	if (!(Library.BoneTransformParams.PerMeshCustomDataIndex_RenderThread.IsValidIndex(Index) && Library.
		BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread.IsValidIndex(Index)))
	{
		return false;
	}

	return true;
}


int32 FTurboSequence_Utility_Lf::AddAnimationToLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                              FCriticalSection& CriticalSection,
                                                              int32& CPUIndices,
                                                              const FSkinnedMeshRuntime_Lf& Runtime,
                                                              const FAnimationMetaData_Lf& Animation)
{
	// Slow...
	FScopeLock Lock(&CriticalSection);

	int32 PoseIndices = GET0_NUMBER;
	CPUIndices = GET0_NUMBER;

	if (!Library.AnimationLibraryData.Contains(Animation.AnimationLibraryHash))
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Contains Animation Library Data"));
		return PoseIndices;
	}


	FAnimationLibraryData_Lf& LibraryAnimData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

	if (IsValid(Animation.Animation))
	{
		if (!LibraryAnimData.KeyframesFilled.Num())
		{
			LibraryAnimData.MaxFrames = Animation.Animation->GetPlayLength() / Runtime.DataAsset->
				TimeBetweenAnimationLibraryFrames - GET1_NUMBER;


			LibraryAnimData.KeyframesFilled.Init(INDEX_NONE, LibraryAnimData.MaxFrames);
			if (!LibraryAnimData.KeyframesFilled.Num())
			{
				return PoseIndices;
			}
		}

		int32 Pose0;
		AnimationCodecTimeToIndex(Animation.AnimationNormalizedTime, LibraryAnimData.MaxFrames,
		                          Animation.Animation->Interpolation, Pose0);

		if (!LibraryAnimData.KeyframesFilled.IsValidIndex(Pose0))
		{
			return PoseIndices;
		}

		PoseIndices = LibraryAnimData.KeyframesFilled[Pose0];
		CPUIndices = Pose0;

		if (LibraryAnimData.KeyframesFilled[Pose0] > INDEX_NONE)
		{
			return PoseIndices;
		}

		LibraryAnimData.KeyframesFilled[Pose0] = Library.AnimationLibraryMaxNum;

		PoseIndices = LibraryAnimData.KeyframesFilled[Pose0];

		const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);

		const FReferenceSkeleton& AnimationSkeleton = Animation.Animation->GetSkeleton()->GetReferenceSkeleton();

		if (!LibraryAnimData.bHasPoseData)
		{
			LibraryAnimData.bHasPoseData = true;

			LibraryAnimData.PoseOptions = FAnimPoseEvaluationOptions_Lf();
			LibraryAnimData.PoseOptions.bEvaluateCurves = true;
			LibraryAnimData.PoseOptions.bShouldRetarget = true;
			LibraryAnimData.PoseOptions.bExtractRootMotion = false;
			LibraryAnimData.PoseOptions.bRetrieveAdditiveAsFullPose = true;
			bool bNeedOptionalSkeletonMesh = true;
			const uint16 NumAnimationBones = AnimationSkeleton.GetNum();
			for (uint16 b = GET0_NUMBER; b < NumAnimationBones; ++b)
			{
				if (const FName& BoneName = GetSkeletonBoneName(AnimationSkeleton, b); GetSkeletonBoneIndex(
					ReferenceSkeleton, BoneName) < GET0_NUMBER)
				{
					bNeedOptionalSkeletonMesh = false;
					break;
				}
			}
			if (bNeedOptionalSkeletonMesh)
			{
				LibraryAnimData.PoseOptions.OptionalSkeletalMesh = Runtime.DataAsset->ReferenceMeshNative;
			}
			if (Animation.Animation->IsCompressedDataValid())
			{
				LibraryAnimData.PoseOptions.EvaluationType = EAnimDataEvalType_Lf::Compressed;
			}

			FAnimPose_Lf AlphaPose;
			FTurboSequence_Helper_Lf::GetPoseInfo(GET0_NUMBER, Animation.Animation, LibraryAnimData.PoseOptions,
			                                      AlphaPose, CriticalSection);

			for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
			{
				const FName& BoneName = GetSkeletonBoneName(ReferenceSkeleton, b);
				if (int32 BoneIndex = FTurboSequence_Helper_Lf::GetAnimationBonePoseIndex(AlphaPose, BoneName);
					BoneIndex > INDEX_NONE)
				{
					LibraryAnimData.BoneNameToAnimationBoneIndex.FindOrAdd(BoneName, BoneIndex);
				}
			}
		}


		if (!LibraryAnimData.AnimPoses.Contains(Pose0))
		{
			float FrameTime0 = static_cast<float>(Pose0) / static_cast<float>(LibraryAnimData.MaxFrames -
				GET1_NUMBER) * Animation.Animation->GetPlayLength();

			FAnimPose_Lf PoseData_0;
			FTurboSequence_Helper_Lf::GetPoseInfo(FrameTime0, Animation.Animation, LibraryAnimData.PoseOptions,
			                                      PoseData_0, CriticalSection);

			FCPUAnimationPose_Lf CPUPose_0;
			CPUPose_0.Pose = PoseData_0;
			CPUPose_0.BelongsToKeyframe = Pose0;
			LibraryAnimData.AnimPoses.Add(Pose0, CPUPose_0);
		}

		uint32 AnimationDataIndex = Library.AnimationLibraryDataAllocatedThisFrame.Num();

		int32 NumAllocations = LibraryAnimData.NumBones * GET3_NUMBER;
		Library.AnimationLibraryMaxNum += NumAllocations;

		Library.AnimationLibraryDataAllocatedThisFrame.AddUninitialized(NumAllocations);
		//LibraryAnimData.AnimPoses[Pose0].RawData.AddUninitialized(NumAllocations);
		for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
		{
			const FTurboSequence_TransposeMatrix_Lf& BoneSpaceTransform = GetBoneTransformFromLocalPoses(
				b, LibraryAnimData, ReferenceSkeleton, AnimationSkeleton, LibraryAnimData.AnimPoses[Pose0].Pose,
				Animation.Animation);

			// NOTE: Keep in mind this matrix is not in correct order after uploading it
			//		 for performance reason we are using matrix calculations which match the order
			//		 in the Vertex Shader
			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				Library.AnimationLibraryDataAllocatedThisFrame[AnimationDataIndex] = BoneSpaceTransform.Colum[M];
				//LibraryAnimData.AnimPoses[Pose0].RawData[b * GET3_NUMBER + M] = BoneData;

				AnimationDataIndex++;
			}
		}
	}
	else // Is Rest Pose
	{
		CPUIndices = GET0_NUMBER;

		if (LibraryAnimData.KeyframesFilled.Num() && LibraryAnimData.KeyframesFilled[GET0_NUMBER] > INDEX_NONE)
		{
			PoseIndices = LibraryAnimData.KeyframesFilled[GET0_NUMBER];
			return PoseIndices;
		}

		if (!LibraryAnimData.KeyframesFilled.Num())
		{
			LibraryAnimData.KeyframesFilled.Init(Library.AnimationLibraryMaxNum, GET1_NUMBER);
			LibraryAnimData.MaxFrames = GET1_NUMBER;

			//const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
			//LibraryAnimData.NumBones = GetSkeletonNumBones(ReferenceSkeleton);
		}
		else
		{
			LibraryAnimData.KeyframesFilled[GET0_NUMBER] = Library.AnimationLibraryMaxNum;
		}
		PoseIndices = LibraryAnimData.KeyframesFilled[GET0_NUMBER];

		if (!LibraryAnimData.AnimPoses.Contains(GET0_NUMBER))
		{
			FCPUAnimationPose_Lf CPUPose_0;
			CPUPose_0.BelongsToKeyframe = GET0_NUMBER;
			LibraryAnimData.AnimPoses.Add(GET0_NUMBER, CPUPose_0);
		}

		int32 NumAllocations = LibraryAnimData.NumBones * GET3_NUMBER;
		Library.AnimationLibraryMaxNum += NumAllocations;

		uint32 AnimationDataIndex = Library.AnimationLibraryDataAllocatedThisFrame.Num();

		const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
		Library.AnimationLibraryDataAllocatedThisFrame.AddUninitialized(NumAllocations);
		for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
		{
			const FMatrix& BoneMatrix = GetSkeletonRefPose(ReferenceSkeleton)[b].ToMatrixWithScale();
			// NOTE: Keep in mind this matrix is not in correct order after uploading it
			//		 for performance reason we are using matrix calculations which match the order
			//		 in the Vertex Shader
			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				FVector4f BoneData;
				BoneData.X = BoneMatrix.M[GET0_NUMBER][M];
				BoneData.Y = BoneMatrix.M[GET1_NUMBER][M];
				BoneData.Z = BoneMatrix.M[GET2_NUMBER][M];
				BoneData.W = BoneMatrix.M[GET3_NUMBER][M];

				Library.AnimationLibraryDataAllocatedThisFrame[AnimationDataIndex] = BoneData;
				//LibraryAnimData.AnimPoses[GET0_NUMBER].RawData[b * GET3_NUMBER + M] = BoneData;

				AnimationDataIndex++;
			}
		}
	}

	return PoseIndices;
}

// int32 FTurboSequence_Utility_Lf::CustomizeMesh(FSkinnedMeshRuntime_Lf& Runtime,
//                                               const TObjectPtr<UTurboSequence_MeshAsset_Lf> TargetMesh,
//                                               const TArray<TObjectPtr<UMaterialInterface>>& TargetMaterials,
//                                               const TObjectPtr<USceneComponent> RootComponent,
//                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
// {
// 	const FTransform MeshTransform = Runtime.WorldSpaceTransform;
// 	TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset = Runtime.FootprintAsset;
//
// 	ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread(Runtime.GetMeshID(), RootComponent->GetWorld());
//
// 	return ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(TargetMesh, MeshTransform, RootComponent->GetWorld(),TargetMaterials, FootprintAsset);
// }

void FTurboSequence_Utility_Lf::UpdateInstanceTransform_Internal(FSkinnedMeshReference_Lf& Reference,
                                                                 const FSkinnedMeshRuntime_Lf& Runtime,
                                                                 const TArray<FCameraView_Lf>& PlayerViews)
{
	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

	const FVector3f PreviousScale = RenderData.ParticleScales[InstanceIndex];

	RenderData.ParticleScales[InstanceIndex] *= Runtime.EIsVisibleOverride !=
		ETurboSequence_IsVisibleOverride_Lf::ScaleToZero;

	if (!PreviousScale.Equals(RenderData.ParticleScales[InstanceIndex]))
	{
		RenderData.bChangedScaleCollectionThisFrame = true;
	}
}

void FTurboSequence_Utility_Lf::UpdateInstanceTransform_Concurrent(FSkinnedMeshReference_Lf& Reference,
                                                                   const FSkinnedMeshRuntime_Lf& Runtime,
                                                                   const FTransform& WorldSpaceTransform,
                                                                   const bool bForce)
{
	if (bForce || Runtime.EIsVisibleOverride !=
		ETurboSequence_IsVisibleOverride_Lf::ScaleToZero)
	{
		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

		const FVector& Position = WorldSpaceTransform.GetLocation();
		if (!Position.Equals(RenderData.ParticlePositions[InstanceIndex]))
		{
			RenderData.bChangedPositionCollectionThisFrame = true;
		}
		RenderData.ParticlePositions[InstanceIndex] = Position;

		const FVector4f& Rotation = FTurboSequence_Helper_Lf::ConvertQuaternionToVector4F(
			WorldSpaceTransform.GetRotation());;
		if (!Rotation.Equals(RenderData.ParticleRotations[InstanceIndex]))
		{
			RenderData.bChangedRotationCollectionThisFrame = true;
		}
		RenderData.ParticleRotations[InstanceIndex] = Rotation;

		const FVector3f& Scale = FTurboSequence_Helper_Lf::ConvertVectorToVector3F(WorldSpaceTransform.GetScale3D());
		if (!Scale.Equals(RenderData.ParticleScales[InstanceIndex]))
		{
			RenderData.bChangedScaleCollectionThisFrame = true;
		}
		RenderData.ParticleScales[InstanceIndex] = Scale;
	}
}

void FTurboSequence_Utility_Lf::AddRenderInstance(FSkinnedMeshReference_Lf& Reference,
                                                  const FSkinnedMeshRuntime_Lf& Runtime,
                                                  FCriticalSection& CriticalSection,
                                                  const FTransform& WorldSpaceTransform)
{
	FScopeLock Lock(&CriticalSection);

	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	const int32 InstanceIndex = RenderData.InstanceMap.Num();
	RenderData.InstanceMap.Add(Runtime.GetMeshID(), InstanceIndex);
	// const int32 InstanceCopyIndex = RenderData.InstanceMapCopy.Num();
	// RenderData.InstanceMapCopy.Add(Runtime.GetMeshID(), InstanceCopyIndex);
	//RenderData.ParticleRemoveIDs.Add(RenderData.GetUniqueID());
	// RenderData.ParticleIDMap.Add(Runtime.GetMeshID(), RenderData.GetUniqueID());
	// RenderData.ParticleIDs.Add(RenderData.GetUniqueID());

	RenderData.ParticlePositions.Add(WorldSpaceTransform.GetLocation());
	RenderData.ParticleRotations.Add(
		FTurboSequence_Helper_Lf::ConvertQuaternionToVector4F(WorldSpaceTransform.GetRotation()));
	RenderData.ParticleScales.Add(
		FTurboSequence_Helper_Lf::ConvertVectorToVector3F(WorldSpaceTransform.GetScale3D()));

	RenderData.ParticleLevelOfDetails.Add(GET0_NUMBER);

	RenderData.ParticleCustomData.AddDefaulted(FTurboSequence_Helper_Lf::NumInstanceCustomData);

	//RenderData.IncrementUniqueID(); // Use the same ID as Niagara
	//RenderData.ParticlesToRemove.Add(false);


	RenderData.bChangedCollectionSizeThisFrame = true;
}

void FTurboSequence_Utility_Lf::CleanNiagaraRenderer(
	TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>& NiagaraComponents,
	FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime)
{
	if (IsValid(NiagaraComponents[Runtime.DataAsset].NiagaraRenderer[Runtime.MaterialsHash].NiagaraRenderer))
	{
		NiagaraComponents[Runtime.DataAsset].NiagaraRenderer[Runtime.MaterialsHash].NiagaraRenderer->DestroyComponent();
	}
	NiagaraComponents[Runtime.DataAsset].NiagaraRenderer.Remove(Runtime.MaterialsHash);
	if (!NiagaraComponents[Runtime.DataAsset].NiagaraRenderer.Num())
	{
		NiagaraComponents.Remove(Runtime.DataAsset);
	}
	Reference.RenderData.Remove(Runtime.MaterialsHash);
}

void FTurboSequence_Utility_Lf::RemoveRenderInstance(FSkinnedMeshReference_Lf& Reference,
                                                     const FSkinnedMeshRuntime_Lf& Runtime,
                                                     FCriticalSection& CriticalSection,
                                                     FSkinnedMeshGlobalLibrary_Lf& Library)
{
	FScopeLock Lock(&CriticalSection);

	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	//RenderData.MeshIDsToRemove.Add(Runtime.GetMeshID());
	Library.BlackListedMeshIDs.Add(Runtime.GetMeshID(), false);

	const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];
	//RenderData.ParticlesToRemove[InstanceIndex] = true;
	//RenderData.ParticlePositions[InstanceIndex] = FVector(-100000, -100000, -100000); // Remove Point

	RenderData.InstanceMap.Remove(Runtime.GetMeshID());
	for (TTuple<int32, int32>& Item : RenderData.InstanceMap)
	{
		if (Item.Value > InstanceIndex)
		{
			Item.Value--;
		}
	}
	//
	//
	RenderData.ParticlePositions.RemoveAt(InstanceIndex);
	RenderData.ParticleRotations.RemoveAt(InstanceIndex);
	RenderData.ParticleScales.RemoveAt(InstanceIndex);
	RenderData.ParticleLevelOfDetails.RemoveAt(InstanceIndex);
	//RenderData.ParticleIDs.RemoveAt(InstanceIndex);

	// Removing IDs happens on the removal of this Array in Tick()
	//RenderData.ParticleIDMap.Remove(Runtime.GetMeshID());
	// RenderData.ParticlesToRemove[InstanceIndex] = true;
	// RenderData.ParticlesToRemoveIndices.Add(InstanceIndex);

	for (int16 i = FTurboSequence_Helper_Lf::NumInstanceCustomData - GET1_NUMBER; i >= GET0_NUMBER; --i)
	{
		int32 CustomDataIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData + i;
		RenderData.ParticleCustomData.RemoveAt(CustomDataIndex);
	}

	RenderData.bChangedCollectionSizeThisFrame = true;
}

void FTurboSequence_Utility_Lf::UpdateRenderInstanceLod_Concurrent(FSkinnedMeshReference_Lf& Reference,
                                                                   const FSkinnedMeshRuntime_Lf& Runtime,
                                                                   const FSkinnedMeshReferenceLodElement_Lf& LodElement,
                                                                   bool bIsVisible)
{
	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

	const uint8 Lod = bIsVisible * LodElement.GPUMeshIndex + !bIsVisible *
		FTurboSequence_Helper_Lf::NotVisibleMeshIndex;
	if (Lod != RenderData.ParticleLevelOfDetails[InstanceIndex])
	{
		RenderData.bChangedLodCollectionThisFrame = true;
	}
	RenderData.ParticleLevelOfDetails[InstanceIndex] = Lod;
}

void FTurboSequence_Utility_Lf::SetCustomDataForInstance(FSkinnedMeshReference_Lf& Reference, int32 CPUIndex,
                                                         int32 SkinWeightOffset, const FSkinnedMeshRuntime_Lf& Runtime,
                                                         const FSkinnedMeshGlobalLibrary_Lf& Library)
{
	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

	const uint32 CustomDataBaseIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData;

	bool bIsAnimated = GetIsMeshVisible(Runtime, Reference);
	if (Reference.LevelOfDetails.Contains(Runtime.LodIndex))
	{
		//const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];
		bIsAnimated = GetIsMeshAnimated(Runtime, Reference) && bIsAnimated;
	}

	const FIntVector2& BitValueSkinWeightOffset = FTurboSequence_Helper_Lf::DecodeUInt32ToUInt16(
		(SkinWeightOffset + FTurboSequence_Helper_Lf::NumCustomStates) * bIsAnimated);
	const FIntVector2& BitValuesTransformIndex = FTurboSequence_Helper_Lf::DecodeUInt32ToUInt16(
		CPUIndex * Library.MaxNumGPUBones *
		FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer + GET2_NUMBER);

	if (RenderData.ParticleCustomData[CustomDataBaseIndex + GET0_NUMBER] != BitValueSkinWeightOffset.X || RenderData.
		ParticleCustomData[CustomDataBaseIndex + GET1_NUMBER] != BitValueSkinWeightOffset.Y ||
		RenderData.ParticleCustomData[CustomDataBaseIndex + GET2_NUMBER] != BitValuesTransformIndex.X || RenderData.
		ParticleCustomData[CustomDataBaseIndex + GET3_NUMBER] != BitValuesTransformIndex.Y)
	{
		RenderData.bChangedCustomDataCollectionThisFrame = true;
	}

	RenderData.ParticleCustomData[CustomDataBaseIndex + GET0_NUMBER] = BitValueSkinWeightOffset.X;
	RenderData.ParticleCustomData[CustomDataBaseIndex + GET1_NUMBER] = BitValueSkinWeightOffset.Y;


	RenderData.ParticleCustomData[CustomDataBaseIndex + GET2_NUMBER] = BitValuesTransformIndex.X;
	RenderData.ParticleCustomData[CustomDataBaseIndex + GET3_NUMBER] = BitValuesTransformIndex.Y;
}

bool FTurboSequence_Utility_Lf::SetCustomDataForInstance_User(FSkinnedMeshReference_Lf& Reference,
                                                              const FSkinnedMeshRuntime_Lf& Runtime,
                                                              int16 CustomDataFractionIndex, float CustomDataValue)
{
	if (CustomDataFractionIndex < GET4_NUMBER || CustomDataFractionIndex >
		FTurboSequence_Helper_Lf::NumInstanceCustomData)
	{
		return false;
	}

	FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

	if (!RenderData.InstanceMap.Contains(Runtime.GetMeshID()))
	{
		return false;
	}

	const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

	const uint32 CustomDataIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData +
		CustomDataFractionIndex;

	if (!RenderData.ParticleCustomData.IsValidIndex(CustomDataIndex))
	{
		return false;
	}

	if (RenderData.ParticleCustomData[CustomDataIndex] != CustomDataValue)
	{
		RenderData.bChangedCustomDataCollectionThisFrame = true;
	}
	RenderData.ParticleCustomData[CustomDataIndex] = CustomDataValue;

	return true;
}

void FTurboSequence_Utility_Lf::UpdateCullingAndLevelOfDetail(FSkinnedMeshRuntime_Lf& Runtime,
                                                              FSkinnedMeshReference_Lf& Reference,
                                                              const TArray<FCameraView_Lf>& CameraViews,
                                                              TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext,
                                                              FSkinnedMeshGlobalLibrary_Lf& Library)
{
	float ClosestCameraDistance = GET_INFINITY;
	const FVector& ComponentLocation = Runtime.WorldSpaceTransform.GetLocation();
	for (const FCameraView_Lf& CameraView : CameraViews)
	{
		if (float Distance = FVector::Distance(ComponentLocation, CameraView.CameraTransform.GetLocation());
			Distance < ClosestCameraDistance)
		{
			ClosestCameraDistance = Distance;
		}
	}
	Runtime.ClosestCameraDistance = ClosestCameraDistance;

	bool bShouldUpdateLod = true;
	if (IsValid(Runtime.FootprintAsset))
	{
		Runtime.FootprintAsset->OnSetMeshIsUpdatingLod_Concurrent(bShouldUpdateLod, Runtime.GetMeshID(), ThreadContext);
	}

	if (bShouldUpdateLod)
	{
		if (Runtime.LodIndex > INDEX_NONE)
		{
			const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];
			UpdateRenderInstanceLod_Concurrent(Reference, Runtime, LodElement,
			                                   GetIsMeshVisible(Runtime, Reference) && LodElement.
			                                   bIsRenderStateValid);
		}
	}


	int16 MaxNumLevelOfDetails = Reference.LevelOfDetails.Num();
	int16 BestLodIndex = MaxNumLevelOfDetails - GET1_NUMBER;
	for (int16 i = GET0_NUMBER; i < MaxNumLevelOfDetails; ++i)
	{
		if (Reference.LevelOfDetails[i].MinLodDistance <= ClosestCameraDistance && Reference.LevelOfDetails[i].
			MaxLodDistance > ClosestCameraDistance)
		{
			BestLodIndex = i;
			break;
		}
	}
	if (IsValid(Runtime.FootprintAsset))
	{
		Runtime.FootprintAsset->OnSetMeshLod_Concurrent(BestLodIndex, Runtime.GetMeshID(), ThreadContext);
	}
	if (Reference.LevelOfDetails.Contains(BestLodIndex))
	{
		if (bShouldUpdateLod && BestLodIndex != Runtime.LodIndex)
		{
			const FSkinnedMeshReferenceLodElement_Lf& NextLodElement = Reference.LevelOfDetails[
				BestLodIndex];
			UpdateRenderInstanceLod_Concurrent(Reference, Runtime, NextLodElement,
			                                   GetIsMeshVisible(Runtime, Reference) && NextLodElement.
			                                   bIsRenderStateValid);
			Runtime.LodIndex = BestLodIndex;
		}
		if (Runtime.LodIndex < GET0_NUMBER)
		{
			Runtime.LodIndex = BestLodIndex;
		}
	}
	else
	{
		if (Runtime.LodIndex < GET0_NUMBER)
		{
			Runtime.LodIndex = GET0_NUMBER;
		}
	}
}

void FTurboSequence_Utility_Lf::UpdateRendererBounds(FCriticalSection& Mutex, FSkinnedMeshReference_Lf& Reference,
                                                     const FSkinnedMeshRuntime_Lf& Runtime)
{
	// We assume it's called already inside bIsVisible

	const FBoxSphereBounds& Bounds = Reference.FirstValidMeshLevelOfDetail->GetBounds();
	const FVector& MeshLocation = Runtime.WorldSpaceTransform.GetLocation();
	const FVector& MeshBoundsMin = MeshLocation - Bounds.BoxExtent;
	const FVector& MeshBoundsMax = MeshLocation + Bounds.BoxExtent;

	Mutex.Lock();

	FVector& MinBounds = Reference.RenderData[Runtime.MaterialsHash].MinBounds;
	FVector& MaxBounds = Reference.RenderData[Runtime.MaterialsHash].MaxBounds;

	// Min
	MinBounds.X = MeshBoundsMin.X < MinBounds.X ? MeshBoundsMin.X : MinBounds.X;
	MinBounds.Y = MeshBoundsMin.Y < MinBounds.Y ? MeshBoundsMin.Y : MinBounds.Y;
	MinBounds.Z = MeshBoundsMin.Z < MinBounds.Z ? MeshBoundsMin.Z : MinBounds.Z;

	// Max
	MaxBounds.X = MeshBoundsMax.X > MaxBounds.X ? MeshBoundsMax.X : MaxBounds.X;
	MaxBounds.Y = MeshBoundsMax.Y > MaxBounds.Y ? MeshBoundsMax.Y : MaxBounds.Y;
	MaxBounds.Z = MeshBoundsMax.Z > MaxBounds.Z ? MeshBoundsMax.Z : MaxBounds.Z;

	Mutex.Unlock();
}

void FTurboSequence_Utility_Lf::UpdateCameraRendererBounds(FRenderData_Lf& RenderData,
                                                           const TArray<FCameraView_Lf>& CameraViews, int32 Extend)
{
	RenderData.MinBounds = FVector::ZeroVector;
	RenderData.MaxBounds = FVector::ZeroVector;
	for (const FCameraView_Lf& View : CameraViews)
	{
		const FVector& MeshLocation = View.CameraTransform.GetLocation();
		const FVector& MeshBoundsMin = MeshLocation - FVector::OneVector * Extend;
		const FVector& MeshBoundsMax = MeshLocation + FVector::OneVector * Extend;

		FVector& MinBounds = RenderData.MinBounds;
		FVector& MaxBounds = RenderData.MaxBounds;

		// Min
		MinBounds.X = MeshBoundsMin.X < MinBounds.X ? MeshBoundsMin.X : MinBounds.X;
		MinBounds.Y = MeshBoundsMin.Y < MinBounds.Y ? MeshBoundsMin.Y : MinBounds.Y;
		MinBounds.Z = MeshBoundsMin.Z < MinBounds.Z ? MeshBoundsMin.Z : MinBounds.Z;

		// Max
		MaxBounds.X = MeshBoundsMax.X > MaxBounds.X ? MeshBoundsMax.X : MaxBounds.X;
		MaxBounds.Y = MeshBoundsMax.Y > MaxBounds.Y ? MeshBoundsMax.Y : MaxBounds.Y;
		MaxBounds.Z = MeshBoundsMax.Z > MaxBounds.Z ? MeshBoundsMax.Z : MaxBounds.Z;
	}
}

uint32 FTurboSequence_Utility_Lf::GetAnimationLayerGroupHash(const TArray<FTurboSequence_BoneLayer_Lf>& Layers)
{
	uint32 Hash = GET0_NUMBER;
	for (const FTurboSequence_BoneLayer_Lf& Layer : Layers)
	{
		FTurboSequence_Helper_Lf::HashDataTypeToHash(Hash, Layer.BoneLayerName);
	}
	return Hash;
}

bool FTurboSequence_Utility_Lf::ContainsAnyAnimationLayer(const TArray<FTurboSequence_BoneLayer_Lf>& A,
                                                          const TArray<FTurboSequence_BoneLayer_Lf>& B)
{
	if (!A.Num() && !B.Num())
	{
		return true;
	}
	for (const FTurboSequence_BoneLayer_Lf& Layer : A)
	{
		for (const FTurboSequence_BoneLayer_Lf& Other : B)
		{
			if (Layer.BoneLayerName == Other.BoneLayerName)
			{
				return true;
			}
		}
	}
	return false;
}

bool FTurboSequence_Utility_Lf::ContainsRootBoneName(const TArray<FTurboSequence_BoneLayer_Lf>& Collection,
                                                     const FSkinnedMeshRuntime_Lf& Runtime)
{
	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
	for (const FTurboSequence_BoneLayer_Lf& Layer : Collection)
	{
		if (int32 BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Layer.BoneLayerName); BoneIndex ==
			GET0_NUMBER)
		{
			return true;
		}
	}
	return false;
}

uint32 FTurboSequence_Utility_Lf::GenerateAnimationLayerMask(FAnimationMetaData_Lf& Animation,
                                                             TArray<uint16>& OutLayers,
                                                             FCriticalSection& CriticalSection,
                                                             const FSkinnedMeshRuntime_Lf& Runtime,
                                                             const FSkinnedMeshReference_Lf& Reference,
                                                             bool bGenerateLayers)
{
	int16 NumCPUBones = Reference.NumCPUBones;

	if (bGenerateLayers)
	{
		CriticalSection.Lock();
		OutLayers.AddUninitialized(NumCPUBones);
		CriticalSection.Unlock();
	}

	uint32 Hash = GET0_NUMBER;
	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
	for (int32 i = GET0_NUMBER; i < NumCPUBones; ++i)
	{
		uint16 LayerValue = GET0_NUMBER;

		int32 ShortestDepth = INDEX_NONE;
		uint8 DepthAmount = GET0_NUMBER;
		bool bFoundBones = false;
		for (const FTurboSequence_BoneLayer_Lf& Bone : Animation.Settings.BoneLayerMasks)
		{
			if (int16 BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Bone.BoneLayerName);
				GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndex))
			{
				bFoundBones = true;

				if (int32 Depth = ReferenceSkeleton.GetDepthBetweenBones(i, BoneIndex); (ShortestDepth ==
					INDEX_NONE || ShortestDepth > Depth) && Depth != INDEX_NONE)
				{
					ShortestDepth = Depth;
					DepthAmount = Bone.BoneDepth;
				}
			}
		}
		if (!bFoundBones)
		{
			if (int32 Depth = ReferenceSkeleton.GetDepthBetweenBones(i, GET0_NUMBER); (ShortestDepth ==
				INDEX_NONE || ShortestDepth > Depth) && Depth != INDEX_NONE)
			{
				ShortestDepth = Depth;
				DepthAmount = GET0_NUMBER;
			}
		}
		// bFoundBones = false;
		// bFoundRootBone = false;

		if (ShortestDepth > INDEX_NONE)
		{
			int32 ShortestDepth_Other = INDEX_NONE;
			uint8 DepthAmount_Other = GET0_NUMBER;
			for (const FAnimationMetaData_Lf& AnimationFrame : Runtime.AnimationMetaData)
			{
				if (AnimationFrame.AnimationID == Animation.AnimationID)
				{
					continue;
				}

				bFoundBones = false;

				for (const FTurboSequence_BoneLayer_Lf& Bone : AnimationFrame.Settings.BoneLayerMasks)
				{
					if (int16 BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Bone.BoneLayerName);
						GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndex))
					{
						bFoundBones = true;

						if (int32 Depth = ReferenceSkeleton.GetDepthBetweenBones(i, BoneIndex); (
								ShortestDepth_Other == INDEX_NONE || ShortestDepth_Other > Depth) && Depth !=
							INDEX_NONE)
						{
							ShortestDepth_Other = Depth;
							DepthAmount_Other = Bone.BoneDepth;
						}
					}
				}

				if (!bFoundBones)
				{
					if (int32 Depth = ReferenceSkeleton.GetDepthBetweenBones(i, GET0_NUMBER); (
						ShortestDepth_Other == INDEX_NONE || ShortestDepth_Other > Depth) && Depth != INDEX_NONE)
					{
						ShortestDepth_Other = Depth;
						DepthAmount_Other = GET0_NUMBER;
					}
				}
			}

			float Percentage_Other = GET0_NUMBER;
			if (ShortestDepth_Other > INDEX_NONE)
			{
				float ClampedCurrent = FMath::Clamp(ShortestDepth_Other, GET0_NUMBER, DepthAmount_Other);

				Percentage_Other = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
					ClampedCurrent, GET0_NUMBER, DepthAmount_Other);
			}

			float ClampedCurrent = FMath::Clamp(ShortestDepth, GET0_NUMBER, DepthAmount);
			float RawPercentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
				ClampedCurrent, GET0_NUMBER, DepthAmount);

			float Percentage = GET0_NUMBER;
			if (RawPercentage > 0.001f)
			{
				if (Percentage_Other > 0.001f)
				{
					if (ShortestDepth > ShortestDepth_Other)
					{
						Percentage = FMath::Abs(RawPercentage - Percentage_Other);
					}
					else
					{
						Percentage = RawPercentage;
					}
				}
				else
				{
					Percentage = RawPercentage;
				}
			}
			LayerValue = FMath::RoundToInt32(Percentage * 0x7FFF);
		}

		FTurboSequence_Helper_Lf::HashDataTypeToHash(Hash, LayerValue);
		if (bGenerateLayers)
		{
			OutLayers[i] = LayerValue;
		}
	}

	return Hash;
}

bool FTurboSequence_Utility_Lf::MergeAnimationLayerMask(const TArray<uint16>& AnimationLayers,
                                                        uint32 AnimationLayerHash, FCriticalSection& CriticalSection,
                                                        FSkinnedMeshGlobalLibrary_Lf& Library, bool bIsAdd)
{
	FScopeLock Lock(&CriticalSection);

	bool bEditedLayer = false;
	if (bIsAdd)
	{
		if (!Library.AnimationBlendLayerMasks.Num())
		{
			bEditedLayer = true;
			FAnimationBlendLayerMask_Lf Mask;
			Mask.AnimationLayerCounter = GET1_NUMBER;
			Mask.AnimationLayerHash = AnimationLayerHash;
			Mask.RawAnimationLayers = AnimationLayers;
			Library.AnimationBlendLayerMasks.Add(Mask);
		}
		else
		{
			const int16 NumOperations = Library.AnimationBlendLayerMasks.Num();

			bool bFoundPattern = false;
			for (int16 OpIdx = GET0_NUMBER; OpIdx < NumOperations; ++OpIdx)
			{
				if (Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerHash == AnimationLayerHash)
				{
					bFoundPattern = true;
					Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerCounter++;
					break;
				}
			}

			if (!bFoundPattern)
			{
				bEditedLayer = true;
				FAnimationBlendLayerMask_Lf Mask;
				Mask.AnimationLayerCounter = GET1_NUMBER;
				Mask.AnimationLayerHash = AnimationLayerHash;
				Mask.RawAnimationLayers = AnimationLayers;
				Library.AnimationBlendLayerMasks.Add(Mask);
			}
		}
	}
	else
	{
		const int16 NumOperations = Library.AnimationBlendLayerMasks.Num();

		bool bFoundPattern = false;

		for (int16 OpIdx = GET0_NUMBER; OpIdx < NumOperations; ++OpIdx)
		{
			if (Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerHash == AnimationLayerHash)
			{
				Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerCounter--;
				if (Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerCounter <= GET0_NUMBER)
				{
					Library.AnimationBlendLayerMasks.RemoveAt(OpIdx);
				}
				bFoundPattern = true;
				break;
			}
		}

		if (!bFoundPattern)
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Could not found Animation Layer Mask Pattern %d"),
			       AnimationLayerHash);
		}
	}


	return bEditedLayer;
}

bool FTurboSequence_Utility_Lf::UpdatedAnimationLayerMaskIndex(FAnimationMetaData_Lf& Animation,
                                                               FSkinnedMeshRuntime_Lf& Runtime,
                                                               FSkinnedMeshGlobalLibrary_Lf& Library,
                                                               const FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                               Library_RenderThread)
{
	int16 NumOperations = Library.AnimationBlendLayerMasks.Num();

	int16 MatchingPatternIndex = INDEX_NONE;
	for (int16 OpIdx = GET0_NUMBER; OpIdx < NumOperations; ++OpIdx)
	{
		if (Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerHash == Animation.AnimationLayerHash)
		{
			MatchingPatternIndex = OpIdx;
			break;
		}
	}

	if (MatchingPatternIndex > INDEX_NONE)
	{
		Animation.LayerMaskIndex = MatchingPatternIndex;

		FAnimationMetaData_RenderThread_Lf& Animation_RenderThread = Runtime.AnimationMetaData_RenderThread[Runtime.
			AnimationIDs[Animation.AnimationID]];
		Animation_RenderThread.LayerMaskIndex = MatchingPatternIndex;

		return true;
	}
	return false;
}

void FTurboSequence_Utility_Lf::UpdateAnimationLayerMasks(FCriticalSection& CriticalSection,
                                                          FSkinnedMeshGlobalLibrary_Lf& Library,
                                                          FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                          Library_RenderThread)
{
	bool bEditedLayers = false;
	if (Library.AnimationBlendLayerMasksRuntimeDirty.Num())
	{
		int16 NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;

		// TODO: Multi-Thread that, it's having somewhere a logic error when using parallelism here ... 
		// for (int32 Index = GET0_NUMBER; Index < NumDirtyMeshes; ++Index)
		// {

		for (const TTuple<FUintVector2, bool>& Mesh : Library.AnimationBlendLayerMasksRuntimeDirty)
		{
			FSkinnedMeshRuntime_Lf& Runtime = Library.RuntimeSkinnedMeshes[Mesh.Key.X];
			const FSkinnedMeshReference_Lf& Reference = Library.PerReferenceData[Runtime.DataAsset];

			if (Runtime.AnimationIDs.Contains(Mesh.Key.Y))
			{
				FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[Runtime.AnimationIDs[Mesh.Key.Y]];

				if (Mesh.Value)
				{
					TArray<uint16> Layers;

					CriticalSection.Lock();
					int16 NumOperations = Library.AnimationBlendLayerMasks.Num();
					CriticalSection.Unlock();
					bool bNeedLayers = true;
					for (int16 OpIdx = GET0_NUMBER; OpIdx < NumOperations; ++OpIdx)
					{
						if (Library.AnimationBlendLayerMasks[OpIdx].AnimationLayerHash == Animation.
							AnimationLayerHash)
						{
							bNeedLayers = false;
							break;
						}
					}
					if (bNeedLayers)
					{
						GenerateAnimationLayerMask(Animation, Layers, CriticalSection, Runtime, Reference,
						                           true);
					}

					if (MergeAnimationLayerMask(Layers, Animation.AnimationLayerHash, CriticalSection, Library,
					                            true))
					{
						CriticalSection.Lock();
						bEditedLayers = true;
						CriticalSection.Unlock();
					}
				}
				else
				{
					TArray<uint16> DummyLayers;
					MergeAnimationLayerMask(DummyLayers, Animation.AnimationLayerHash, CriticalSection, Library,
					                        false);
				}
			}
		}

		if (bEditedLayers)
		{
			TArray<FAnimationBlendLayerMask_Lf> Masks = Library.AnimationBlendLayerMasks;
			uint16 NumOperations_RenderThread = Library.AnimationBlendLayerMasks.Num();
			int16 NumCPUBones = Library.MaxNumCPUBones;

			ENQUEUE_RENDER_COMMAND(TurboSequence_AddAnimationLayerMaske_Lf)(
				[&Library_RenderThread, Masks, NumOperations_RenderThread, NumCPUBones](
				FRHICommandListImmediate& RHICmdList)
				{
					uint32 NumLayers = NumOperations_RenderThread * NumCPUBones;
					Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread.SetNumUninitialized(
						NumLayers);

					ParallelFor(NumOperations_RenderThread, [&](int32 Index)
					{
						uint32 BaseIndex = Index * NumCPUBones;
						uint32 MaxIndex = BaseIndex + NumCPUBones;
						int16 LayerIndex = GET0_NUMBER;
						for (uint32 i = BaseIndex; i < MaxIndex; ++i)
						{
							if (Masks[Index].RawAnimationLayers.IsValidIndex(LayerIndex))
							{
								Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread[i] = Masks[
									Index].RawAnimationLayers[LayerIndex];
								LayerIndex++;
							}
							else
							{
								Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread[i] =
									GET0_NUMBER;
							}
						}
					}, EParallelForFlags::BackgroundPriority);
				});

			const int32 NumMeshes = Library.RuntimeSkinnedMeshesHashMap.Num();
			int32 NumMeshesPerThread = FMath::CeilToInt(
				static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
			ParallelFor(NumThreads, [&](int32 ThreadsIndex)
			{
				const int32 MeshBaseIndex = ThreadsIndex * NumMeshesPerThread;
				const int32 MeshBaseNum = MeshBaseIndex + NumMeshesPerThread;

				for (int32 Index = MeshBaseIndex; Index < MeshBaseNum; ++Index)
				{
					if (Index >= NumMeshes)
					{
						break;
					}

					FSkinnedMeshRuntime_Lf& Runtime = Library.RuntimeSkinnedMeshes[Library.
						RuntimeSkinnedMeshesHashMap[Index]];

					for (FAnimationMetaData_Lf& Animation : Runtime.AnimationMetaData)
					{
						UpdatedAnimationLayerMaskIndex(Animation, Runtime,
						                               Library, Library_RenderThread);
					}
				}
			}, EParallelForFlags::BackgroundPriority);
		}
		else
		{
			for (const TTuple<FUintVector2, bool>& Mesh : Library.AnimationBlendLayerMasksRuntimeDirty)
			{
				FSkinnedMeshRuntime_Lf& Runtime = Library.RuntimeSkinnedMeshes[Mesh.Key.X];

				if (Runtime.AnimationIDs.Contains(Mesh.Key.Y))
				{
					if (Mesh.Value)
					{
						FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[Runtime.AnimationIDs[
							Mesh.Key.Y]];

						UpdatedAnimationLayerMaskIndex(Animation, Runtime,
						                               Library, Library_RenderThread);
					}
				}
			}
		}

		Library.AnimationBlendLayerMasksRuntimeDirty.Empty();
	}

	if (Library.bMaxNumCPUBonesChanged && !bEditedLayers)
	{
		TArray<FAnimationBlendLayerMask_Lf> Masks = Library.AnimationBlendLayerMasks;
		uint16 NumOperations_RenderThread = Library.AnimationBlendLayerMasks.Num();
		int16 NumCPUBones = Library.MaxNumCPUBones;

		ENQUEUE_RENDER_COMMAND(TurboSequence_AddAnimationLayerMaske_Lf)(
			[&Library_RenderThread, Masks, NumOperations_RenderThread, NumCPUBones](
			FRHICommandListImmediate& RHICmdList)
			{
				uint32 NumLayers = NumOperations_RenderThread * NumCPUBones;
				Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread.
				                     SetNumUninitialized(NumLayers);

				ParallelFor(NumOperations_RenderThread, [&](int32 Index)
				{
					uint32 BaseIndex = Index * NumCPUBones;
					uint32 MaxIndex = BaseIndex + NumCPUBones;
					int16 LayerIndex = GET0_NUMBER;
					for (uint32 i = BaseIndex; i < MaxIndex; ++i)
					{
						if (Masks[Index].RawAnimationLayers.IsValidIndex(LayerIndex))
						{
							Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread[i] = Masks[Index].
								RawAnimationLayers[LayerIndex];
							LayerIndex++;
						}
						else
						{
							Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread[i] = GET0_NUMBER;
						}
					}
				}, EParallelForFlags::BackgroundPriority);
			});
	}
}

void FTurboSequence_Utility_Lf::AddAnimation(FSkinnedMeshRuntime_Lf& Runtime, const FSkinnedMeshReference_Lf& Reference,
                                             FAnimationMetaData_Lf& Animation,
                                             const FAnimationMetaData_RenderThread_Lf& Animation_RenderThread,
                                             const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
                                             FSkinnedMeshGlobalLibrary_Lf& Library)
{
	FScopeLock Lock(&ThreadContext->CriticalSection);
	{
		TArray<uint16> Layers;
		Animation.AnimationLayerHash = GenerateAnimationLayerMask(
			Animation, Layers, ThreadContext->CriticalSection, Runtime, Reference, false);

		if (!Runtime.AnimationGroups.Contains(Animation.AnimationGroupLayerHash))
		{
			FAnimationGroup_Lf Group; // = FAnimationGroup_Lf();
			Group.TotalAnimWeightRuntime = GET0_NUMBER;
			Group.NumAnimationsInGroup = GET1_NUMBER;
			Runtime.AnimationGroups.Add(Animation.AnimationGroupLayerHash, Group);
		}
		else
		{
			Runtime.AnimationGroups[Animation.AnimationGroupLayerHash].NumAnimationsInGroup++;
		}
		Runtime.AnimationMetaData.Add(Animation);
		Runtime.AnimationMetaData_RenderThread.Add(Animation_RenderThread);
		Runtime.AnimationIDs.Add(Animation.AnimationID, GET0_NUMBER);

		Library.AnimationBlendLayerMasksRuntimeDirty.FindOrAdd(
			FUintVector2(Runtime.GetMeshID(), Animation.AnimationID), true);

		int16 NumAnimations = Runtime.AnimationMetaData.Num();
		for (int16 AnimIdx = GET0_NUMBER; AnimIdx < NumAnimations; ++AnimIdx)
		{
			FAnimationMetaData_Lf& AnimationMeta = Runtime.AnimationMetaData[AnimIdx];

			if (Runtime.AnimationIDs.Contains(AnimationMeta.AnimationID))
			{
				Runtime.AnimationIDs[AnimationMeta.AnimationID] = AnimIdx;
			}
		}
		if (!Library.AnimationLibraryData.Contains(Animation.AnimationLibraryHash))
		{
			FAnimationLibraryData_Lf Data = FAnimationLibraryData_Lf();
			Data.AnimationDensity++;
			Data.NumBones = GetSkeletonNumBones(GetReferenceSkeleton(Runtime.DataAsset));
			Data.IndexInCollection = Library.AnimationLibraryData.Num();
			Library.AnimationLibraryData.Add(Animation.AnimationLibraryHash, Data);
		}
		else
		{
			Library.AnimationLibraryData[Animation.AnimationLibraryHash].AnimationDensity++;
		}
	}
	if (IsValid(Runtime.FootprintAsset))
	{
		Runtime.FootprintAsset->
		        OnAddedMeshAnimation_Concurrent(Runtime.GetMeshID(), Animation.AnimationID, ThreadContext);
	}
}

void FTurboSequence_Utility_Lf::RemoveAnimation(FSkinnedMeshRuntime_Lf& Runtime,
                                                const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
                                                FSkinnedMeshGlobalLibrary_Lf& Library,
                                                FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                                int32 Index)
{
	ThreadContext->CriticalSection.Lock();
	const uint32 AnimationID = Runtime.AnimationMetaData[Index].AnimationID;
	ThreadContext->CriticalSection.Unlock();
	if (IsValid(Runtime.FootprintAsset))
	{
		Runtime.FootprintAsset->OnRemovedMeshAnimation_Concurrent(Runtime.GetMeshID(), AnimationID, ThreadContext);
	}

	FScopeLock Lock(&ThreadContext->CriticalSection);
	{
		const FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[Index];
		if (Runtime.AnimationGroups.Contains(Animation.AnimationGroupLayerHash))
		{
			Runtime.AnimationGroups[Animation.AnimationGroupLayerHash].NumAnimationsInGroup--;
			if (Runtime.AnimationGroups[Animation.AnimationGroupLayerHash].NumAnimationsInGroup <= GET0_NUMBER)
			{
				Runtime.AnimationGroups.Remove(Animation.AnimationGroupLayerHash);
			}
		}

		Library.AnimationBlendLayerMasksRuntimeDirty.FindOrAdd(FUintVector2(Runtime.GetMeshID(), AnimationID),
		                                                       false);

		if (Runtime.AnimationIDs.Contains(AnimationID))
		{
			Runtime.AnimationIDs.Remove(AnimationID);
		}
		Runtime.AnimationMetaData.RemoveAt(Index);
		Runtime.AnimationMetaData_RenderThread.RemoveAt(Index);

		int16 NumAnimations = Runtime.AnimationMetaData.Num();
		for (int16 AnimIdx = NumAnimations - GET1_NUMBER; AnimIdx >= GET1_NUMBER; --AnimIdx)
		{
			FAnimationMetaData_Lf& AnimationMeta = Runtime.AnimationMetaData[AnimIdx];

			if (Runtime.AnimationIDs.Contains(AnimationMeta.AnimationID))
			{
				Runtime.AnimationIDs[AnimationMeta.AnimationID] = AnimIdx;
			}
		}
	}
}

bool FTurboSequence_Utility_Lf::RefreshBlendSpaceState(const TObjectPtr<UBlendSpace> BlendSpace,
                                                       FAnimationBlendSpaceData_Lf& Data, float DeltaTime,
                                                       FCriticalSection& CriticalSection)
{
	Data.Tick = FAnimTickRecord(BlendSpace, FVector(Data.CurrentPosition), Data.CachedBlendSampleData,
	                            Data.BlendFilter, true, 1.0f, true, true, 1.0f, Data.CurrentTime, Data.Record);
	const TArray<FName> MarkerNames;
	FAnimAssetTickContext Context(DeltaTime, ERootMotionMode::IgnoreRootMotion, false, MarkerNames);
	if (!Data.Tick.DeltaTimeRecord)
	{
		Data.Tick.DeltaTimeRecord = &Data.DeltaTimeRecord;
	}
	Context.MarkerTickContext.SetMarkerSyncStartPosition(
		FMarkerSyncAnimPosition(FName("-1"), FName("-1"), GET0_NUMBER));

	CriticalSection.Lock();
	BlendSpace->TickAssetPlayer(Data.Tick, Data.NotifyQueue, Context);
	CriticalSection.Unlock();


	if (!Data.CachedBlendSampleData.Num())
	{
		return false;
	}

	return true;
}

FTurboSequence_AnimMinimalBlendSpace_Lf FTurboSequence_Utility_Lf::PlayBlendSpace(
	const FSkinnedMeshReference_Lf& Reference, FSkinnedMeshGlobalLibrary_Lf& Library,
	FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread, FSkinnedMeshRuntime_Lf& Runtime,
	const TObjectPtr<UBlendSpace> BlendSpace, const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	const FTurboSequence_AnimPlaySettings_Lf& AnimSettings, float OverrideWeight, float OverrideStartTime,
	float OverrideEndTime)
{
	FAnimationBlendSpaceData_Lf Data = FAnimationBlendSpaceData_Lf();
	FTurboSequence_AnimMinimalBlendSpace_Lf BlendSpaceData = FTurboSequence_AnimMinimalBlendSpace_Lf(true);
	BlendSpaceData.BlendSpace = BlendSpace;
	BlendSpaceData.BelongsToMeshID = Runtime.GetMeshID();
	BlendSpace->InitializeFilter(&Data.BlendFilter);

	int16 NumSamples = BlendSpace->GetBlendSamples().Num();

	RefreshBlendSpaceState(BlendSpace, Data, GET0_NUMBER, ThreadContext->CriticalSection);

	FTurboSequence_AnimPlaySettings_Lf Settings = AnimSettings;
	Settings.AnimationManagementMode = ETurboSequence_ManagementMode_Lf::SelfManaged;
	Settings.bAnimationTimeSelfManaged = true;

	for (int32 i = GET0_NUMBER; i < NumSamples; ++i)
	{
		float PlayLength = BlendSpace->GetBlendSample(i).Animation->GetPlayLength();

		Data.LongestPlayLength = FMath::Max(Data.LongestPlayLength, PlayLength);

		uint32 AnimID = PlayAnimation(Reference, Library, Library_RenderThread, Runtime, ThreadContext,
		                              BlendSpace->GetBlendSample(i).Animation, Settings, BlendSpace->bLoop,
		                              OverrideWeight, OverrideStartTime, OverrideEndTime);

		if (const FBlendSampleData* Sample = Data.CachedBlendSampleData.FindByPredicate(
			[&i](const FBlendSampleData& Other) { return Other.SampleDataIndex == i; }))
		{
			FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[AnimID]];

			AnimationFrame.Settings.AnimationWeight = Sample->GetClampedWeight();

			AnimationFrame.AnimationTime = FMath::Clamp(Sample->Time, 0.0f, Sample->Animation->GetPlayLength());

			TweakAnimation(Runtime, ThreadContext, Library, Library_RenderThread, AnimationFrame.Settings, AnimID,
			               Reference);
		}

		ThreadContext->CriticalSection.Lock();
		Data.Points.Add(AnimID, i);
		// Might add more info
		BlendSpaceData.Samples.Add(AnimID);
		ThreadContext->CriticalSection.Unlock();
	}

	int32 NumBlendSpaces = Runtime.AnimationBlendSpaceMetaData.Num();
	TArray<TObjectPtr<UBlendSpace>> BlendSpaceKeys;
	ThreadContext->CriticalSection.Lock();
	Runtime.AnimationBlendSpaceMetaData.GetKeys(BlendSpaceKeys);
	ThreadContext->CriticalSection.Unlock();
	for (int32 i = NumBlendSpaces - GET1_NUMBER; i >= GET0_NUMBER; --i)
	{
		const FAnimationBlendSpaceData_Lf& MetaData = Runtime.AnimationBlendSpaceMetaData[BlendSpaceKeys[i]];

		bool bIsValidBlendSpace = true;
		for (const TTuple<uint32, int32>& Point : MetaData.Points)
		{
			if (!Runtime.AnimationIDs.Contains(Point.Key))
			{
				bIsValidBlendSpace = false;
				break;
			}
		}
		if (!bIsValidBlendSpace)
		{
			ThreadContext->CriticalSection.Lock();
			Runtime.AnimationBlendSpaceMetaData.Remove(BlendSpaceKeys[i]);
			ThreadContext->CriticalSection.Unlock();
			continue;
		}

		for (const TTuple<uint32, int32>& Point : MetaData.Points)
		{
			if (Runtime.AnimationIDs.Contains(Point.Key))
			{
				if (FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[Point.
					Key]]; ContainsAnyAnimationLayer(AnimSettings.BoneLayerMasks,
					                                 AnimationFrame.Settings.BoneLayerMasks))
				{
					AnimationFrame.bIsOldAnimation = true;
				}
			}
		}
	}

	if (!BlendSpaceData.Samples.Num())
	{
		return FTurboSequence_AnimMinimalBlendSpace_Lf(false);
	}

	ThreadContext->CriticalSection.Lock();
	Runtime.AnimationBlendSpaceMetaData.Add(BlendSpace, Data);
	ThreadContext->CriticalSection.Unlock();

	return BlendSpaceData;
}

uint32 FTurboSequence_Utility_Lf::PlayAnimation(const FSkinnedMeshReference_Lf& Reference,
                                                FSkinnedMeshGlobalLibrary_Lf& Library,
                                                FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                                FSkinnedMeshRuntime_Lf& Runtime,
                                                const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
                                                UAnimSequence* Animation,
                                                const FTurboSequence_AnimPlaySettings_Lf& AnimSettings,
                                                const bool bIsLoop, float OverrideWeight,
                                                float OverrideStartTime, float OverrideEndTime,
                                                uint32 OverrideAnimationID)
{
	// TObjectPtr<USkeleton> Skeleton = Reference.DataAsset->GetSkeleton();
	//
	// ThreadContext->CriticalSection.Lock();
	// if (Library.MeshIDToMinimalData.Contains(Runtime.GetMeshID()))
	// {
	// 	const FTurboSequence_MinimalMeshData_Lf& MinimalMeshData = Library.MeshIDToMinimalData[Runtime.GetMeshID()];
	// 	if (Runtime.GetMeshID() != MinimalMeshData.RootMotionMeshID)
	// 	{
	// 		FSkinnedMeshRuntime_Lf& RuntimeRoot = Library.RuntimeSkinnedMeshes[MinimalMeshData.RootMotionMeshID];
	// 		Skeleton = RuntimeRoot.DataAsset->GetSkeleton();
	// 	}
	// }
	// ThreadContext->CriticalSection.Unlock();

	const FUintVector& AnimationHash = GetAnimationLibraryKey(Reference.DataAsset->GetSkeleton(),
	                                                          Reference.DataAsset, Animation);

	const bool bIsRestPose = !IsValid(Animation);

	if (AnimSettings.ForceMode != ETurboSequence_AnimationForceMode_Lf::None)
	{
		ClearAnimations(ThreadContext, Runtime, Library, Library_RenderThread, AnimSettings.ForceMode,
		                AnimSettings.BoneLayerMasks, [](const FAnimationMetaData_Lf& AnimationMeta)
		                {
			                return true;
		                });
	}
	FAnimationMetaData_Lf Frame = FAnimationMetaData_Lf();
	Frame.Animation = Animation;
	Frame.AnimationTime = AnimSettings.AnimationPlayTimeInSeconds;
	Frame.bIsLoop = bIsLoop;

	Frame.AnimationWeightStartTime = AnimSettings.StartTransitionTimeInSeconds;
	Frame.AnimationWeightTime = GET0_NUMBER;
	if (AnimSettings.ForceMode != ETurboSequence_AnimationForceMode_Lf::None)
	{
		if (OverrideWeight >= GET0_NUMBER)
		{
			Frame.FinalAnimationWeight = OverrideWeight * AnimSettings.AnimationWeight;
		}
		else
		{
			Frame.FinalAnimationWeight = GET1_NUMBER * AnimSettings.AnimationWeight;
		}
		Frame.AnimationWeightTime = AnimSettings.StartTransitionTimeInSeconds;
	}
	else
	{
		if (OverrideWeight >= GET0_NUMBER)
		{
			Frame.FinalAnimationWeight = OverrideWeight * AnimSettings.AnimationWeight;
		}
		else
		{
			Frame.FinalAnimationWeight = GET0_NUMBER;
		}
	}
	if (OverrideStartTime >= GET0_NUMBER)
	{
		Frame.AnimationWeightTime = OverrideStartTime;
	}
	Frame.AnimationRemoveTime = GET0_NUMBER;
	if (OverrideEndTime >= GET0_NUMBER)
	{
		Frame.AnimationRemoveTime = OverrideEndTime;
	}
	Frame.AnimationRemoveStartTime = AnimSettings.EndTransitionTimeInSeconds;
	Frame.bIsOldAnimation = false;
	Frame.AnimationLibraryHash = AnimationHash;
	if (bIsRestPose)
	{
		Frame.AnimationMaxPlayLength = GET1_NUMBER;
	}
	else
	{
		Frame.AnimationMaxPlayLength = Animation->GetPlayLength();
	}
	Frame.AnimationNormalizedTime = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
		Frame.AnimationTime, GET0_NUMBER, Frame.AnimationMaxPlayLength);

	if (!AnimSettings.BoneLayerMasks.Num())
	{
		Frame.bIsRootBoneAnimation = true;
	}
	else
	{
		Frame.bIsRootBoneAnimation = ContainsRootBoneName(AnimSettings.BoneLayerMasks, Runtime);
	}
	Frame.bIsSelfManagedAnimation = AnimSettings.AnimationManagementMode ==
		ETurboSequence_ManagementMode_Lf::SelfManaged;
	Frame.AnimationGroupLayerHash = GetAnimationLayerGroupHash(AnimSettings.BoneLayerMasks);

	if (OverrideAnimationID > GET0_NUMBER)
	{
		Frame.SetAnimationID(OverrideAnimationID);
	}
	else
	{
		Frame.SetAnimationID(Runtime.AnimationIDs, Runtime.GetMeshID());
	}
	Frame.Settings = AnimSettings;

	uint16 NumAnimationsPreAdd = Runtime.AnimationMetaData.Num();
	for (int32 AnimIdx = NumAnimationsPreAdd - GET1_NUMBER; AnimIdx >= GET1_NUMBER; --AnimIdx)
	{
		FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[AnimIdx];
		if (!AnimationFrame.bIsSelfManagedAnimation && ContainsAnyAnimationLayer(
			AnimSettings.BoneLayerMasks, AnimationFrame.Settings.BoneLayerMasks))
		{
			AnimationFrame.bIsOldAnimation = true;
		}
	}

	FAnimationMetaData_RenderThread_Lf RenderData;


	AddAnimation(Runtime, Reference, Frame, RenderData, ThreadContext, Library);

	return Frame.AnimationID;
}

TObjectPtr<UAnimSequence> FTurboSequence_Utility_Lf::GetHighestPriorityAnimation(const FSkinnedMeshRuntime_Lf& Runtime)
{
	if (!Runtime.AnimationMetaData.Num())
	{
		return nullptr;
	}
	TObjectPtr<UAnimSequence> BestMatchAnimation = nullptr;
	float BestMatchWeight = GET1_NEGATIVE_NUMBER;
	for (const FAnimationMetaData_Lf& Animation : Runtime.AnimationMetaData)
	{
		if (Animation.FinalAnimationWeight > BestMatchWeight)
		{
			BestMatchWeight = Animation.FinalAnimationWeight;
			BestMatchAnimation = Animation.Animation;
		}
	}
	return BestMatchAnimation;
}

void FTurboSequence_Utility_Lf::UpdateBlendSpaces(FSkinnedMeshRuntime_Lf& Runtime,
                                                  const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
                                                  float DeltaTime, FSkinnedMeshGlobalLibrary_Lf& Library,
                                                  FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                                  const FSkinnedMeshReference_Lf& Reference)
{
	for (TTuple<TObjectPtr<UBlendSpace>, FAnimationBlendSpaceData_Lf>& BlendSpace : Runtime.
	     AnimationBlendSpaceMetaData)
	{
		FAnimationBlendSpaceData_Lf& Data = BlendSpace.Value;

		if (!RefreshBlendSpaceState(BlendSpace.Key, Data, DeltaTime, ThreadContext->CriticalSection))
		{
			continue;
		}

		for (const TTuple<uint32, int32>& Point : Data.Points)
		{
			if (Runtime.AnimationIDs.Contains(Point.Key))
			{
				FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[Point.Key]];

				FTurboSequence_AnimPlaySettings_Lf Settings = AnimationFrame.Settings;

				float Weight = GET0_NUMBER;
				float Time = AnimationFrame.AnimationTime;
				if (const FBlendSampleData* Sample = Data.CachedBlendSampleData.FindByPredicate(
					[&Point](const FBlendSampleData& Other) { return Other.SampleDataIndex == Point.Value; }))
				{
					Weight = Sample->GetClampedWeight();
					Time = Sample->Time;
				}

				float AnimLength = AnimationFrame.Animation->GetPlayLength();
				Settings.AnimationWeight = Weight;
				//Settings.Settings.AnimationSpeed = AnimLength / Data.LongestPlayLength;
				AnimationFrame.AnimationTime = FMath::Clamp(Time, 0.0f, AnimLength);

				TweakAnimation(Runtime, ThreadContext, Library, Library_RenderThread, Settings,
				               AnimationFrame.AnimationID, Reference);
			}
		}
	}
}

bool FTurboSequence_Utility_Lf::TweakBlendSpace(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection,
                                                const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpace,
                                                const FVector3f& WantedPosition)
{
	if (!Runtime.AnimationBlendSpaceMetaData.Contains(BlendSpace.BlendSpace))
	{
		return false;
	}
	FAnimationBlendSpaceData_Lf& Data = Runtime.AnimationBlendSpaceMetaData[BlendSpace.BlendSpace];
	Data.CurrentPosition = WantedPosition;
	return true;
}

bool FTurboSequence_Utility_Lf::TweakAnimation(FSkinnedMeshRuntime_Lf& Runtime,
                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
                                               FSkinnedMeshGlobalLibrary_Lf& Library,
                                               FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                               const FTurboSequence_AnimPlaySettings_Lf& Settings, uint32 AnimationID,
                                               const FSkinnedMeshReference_Lf& Reference)
{
	if (!Runtime.AnimationIDs.Contains(AnimationID))
	{
		return false;
	}
	FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[AnimationID]];

	if (Settings.ForceMode != ETurboSequence_AnimationForceMode_Lf::None)
	{
		ClearAnimations(ThreadContext, Runtime, Library, Library_RenderThread, Settings.ForceMode,
		                Settings.BoneLayerMasks, [&AnimationFrame](const FAnimationMetaData_Lf& AnimationMetaData)
		                {
			                return AnimationFrame.AnimationID != AnimationMetaData.AnimationID;
		                });

		AnimationFrame.AnimationWeightStartTime = Settings.StartTransitionTimeInSeconds;
		AnimationFrame.AnimationWeightTime = Settings.StartTransitionTimeInSeconds;
	}
	else
	{
		AnimationFrame.AnimationWeightStartTime = Settings.StartTransitionTimeInSeconds;
	}

	AnimationFrame.Settings = Settings;

	return true;
}

void FTurboSequence_Utility_Lf::SolveAnimations(FSkinnedMeshRuntime_Lf& Runtime, FSkinnedMeshGlobalLibrary_Lf& Library,
                                                FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                                const FSkinnedMeshReference_Lf& Reference,
                                                float DeltaTime, int32 CurrentFrameCount,
                                                const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	if (CurrentFrameCount == Runtime.LastFrameAnimationSolved)
	{
		return;
	}
	Runtime.LastFrameAnimationSolved = CurrentFrameCount;


	UpdateBlendSpaces(Runtime, ThreadContext, DeltaTime, Library, Library_RenderThread, Reference);

	for (TTuple<uint32, FAnimationGroup_Lf>& Group : Runtime.AnimationGroups)
	{
		Group.Value.TotalAnimWeightRuntime = GET0_NUMBER;
	}

	uint16 NumAnimations = Runtime.AnimationMetaData.Num();
	for (int32 AnimIdx = NumAnimations - GET1_NUMBER; AnimIdx >= GET0_NUMBER; --AnimIdx)
	{
		FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[AnimIdx];

		if (IsValid(Animation.Animation))
		{
			if (!Animation.Settings.bAnimationTimeSelfManaged)
			{
				// Animation Time
				Animation.AnimationTime += DeltaTime * Animation.Settings.AnimationSpeed;
				if (Animation.bIsLoop)
				{
					Animation.AnimationTime =
						FMath::Fmod(Animation.AnimationTime, Animation.AnimationMaxPlayLength);
					Animation.AnimationTime = FMath::Clamp(Animation.AnimationTime, 0.0f,
					                                       Animation.AnimationMaxPlayLength);
				}
				else
				{
					Animation.AnimationTime = FMath::Clamp(Animation.AnimationTime, 0.0f,
					                                       Animation.AnimationMaxPlayLength);
				}
			}

			// Frame
			Animation.AnimationNormalizedTime = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
				Animation.AnimationTime, GET0_NUMBER, Animation.AnimationMaxPlayLength);
		}
		else
		{
			Animation.AnimationTime = GET0_NUMBER;
			Animation.AnimationNormalizedTime = GET0_NUMBER;
		}

		// Add
		Animation.AnimationWeightTime = FMath::Clamp(Animation.AnimationWeightTime + DeltaTime, 0.0f,
		                                             Animation.AnimationWeightStartTime);
		float AnimationWeight = FTurboSequence_Helper_Lf::Clamp01(
			FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(Animation.AnimationWeightTime, GET0_NUMBER,
			                                                     Animation.AnimationWeightStartTime));

		// Removal
		if (AnimIdx && Animation.bIsOldAnimation)
		{
			float Time = FMath::Min(Animation.AnimationRemoveTime, Animation.AnimationWeightTime);
			Animation.AnimationRemoveTime =
				FMath::Clamp(Time + DeltaTime, 0.0f, Animation.AnimationRemoveStartTime);
			AnimationWeight = GET1_NUMBER - FTurboSequence_Helper_Lf::Clamp01(
				FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
					Animation.AnimationRemoveTime, GET0_NUMBER, Animation.AnimationRemoveStartTime));

			if (FMath::IsNearlyEqual(AnimationWeight, GET0_NUMBER, 0.01f))
			{
				RemoveAnimation(Runtime, ThreadContext, Library, Library_RenderThread, AnimIdx);
				continue;
			}
		}

		if (AnimIdx)
		{
			// Weight
			Animation.FinalAnimationWeight = FMath::Min(AnimationWeight, Animation.Settings.AnimationWeight);

			// Can Happen when LOD is switching to Fast Animation Mode it's 1 frame using this mode
			if (Runtime.AnimationGroups.Contains(Animation.AnimationGroupLayerHash))
			{
				FAnimationGroup_Lf& Group = Runtime.AnimationGroups[Animation.AnimationGroupLayerHash];

				Group.TotalAnimWeightRuntime += Animation.FinalAnimationWeight;
			}
		}

		int32 CPUPoses;
		int32 GPUPoses = AddAnimationToLibraryChunked(Library, ThreadContext->CriticalSection, CPUPoses,
		                                              Runtime, Animation);

		FAnimationMetaData_RenderThread_Lf& RenderData = Runtime.AnimationMetaData_RenderThread[AnimIdx];

		RenderData.GPUAnimationIndex_0 = GPUPoses;

		Animation.CPUAnimationIndex_0 = CPUPoses;
	}

	uint16 NumAnimationsPostRemove = Runtime.AnimationMetaData.Num();
	float BaseLayerWeight = GET1_NUMBER;
	for (int32 AnimIdx = NumAnimationsPostRemove - GET1_NUMBER; AnimIdx >= GET0_NUMBER; --AnimIdx)
	{
		FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[AnimIdx];
		// Can Happen when LOD is switching to Fast Animation Mode it's 1 frame using this mode
		if (Runtime.AnimationGroups.Contains(Animation.AnimationGroupLayerHash))
		{
			const FAnimationGroup_Lf& Group = Runtime.AnimationGroups[Animation.AnimationGroupLayerHash];

			if (AnimIdx)
			{
				Animation.FinalAnimationWeight /= Group.TotalAnimWeightRuntime;
				Animation.FinalAnimationWeight = FMath::Min(Animation.FinalAnimationWeight,
				                                            Animation.Settings.AnimationWeight);
				BaseLayerWeight -= Animation.FinalAnimationWeight;
			}
			else
			{
				Animation.FinalAnimationWeight = FTurboSequence_Helper_Lf::Clamp01(BaseLayerWeight);
			}
		}

		FAnimationMetaData_RenderThread_Lf& RenderData = Runtime.AnimationMetaData_RenderThread[AnimIdx];
		RenderData.FinalAnimationWeight = Animation.FinalAnimationWeight * 0x7FFF;
	}
}

FTurboSequence_TransposeMatrix_Lf FTurboSequence_Utility_Lf::GetBoneTransformFromLocalPoses(int16 BoneIndex,
	const FAnimationLibraryData_Lf& LibraryData, const FReferenceSkeleton& ReferenceSkeleton,
	const FReferenceSkeleton& AnimationSkeleton, const FAnimPose_Lf& Pose, const TObjectPtr<UAnimSequence> Animation)
{
	FTurboSequence_TransposeMatrix_Lf BoneSpaceTransform = FTurboSequence_TransposeMatrix_Lf();

	if (!BoneIndex && Animation->bEnableRootMotion)
	{
		switch (Animation->RootMotionRootLock)
		{
		case ERootMotionRootLock::RefPose:
			{
				const FMatrix RefPoseMatrix = GetSkeletonRefPose(ReferenceSkeleton)[GET0_NUMBER].
					ToMatrixWithScale();

				for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
				{
					BoneSpaceTransform.Colum[M].X = RefPoseMatrix.M[GET0_NUMBER][M];
					BoneSpaceTransform.Colum[M].Y = RefPoseMatrix.M[GET1_NUMBER][M];
					BoneSpaceTransform.Colum[M].Z = RefPoseMatrix.M[GET2_NUMBER][M];
					BoneSpaceTransform.Colum[M].W = RefPoseMatrix.M[GET3_NUMBER][M];
				}
			}
			break;

		case ERootMotionRootLock::AnimFirstFrame:
			BoneSpaceTransform = Pose.LocalSpacePoses[GET0_NUMBER];
			break;

		case ERootMotionRootLock::Zero:

			{
				const FMatrix ZeroMatrix = FTransform::Identity.ToMatrixWithScale();

				for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
				{
					BoneSpaceTransform.Colum[M].X = ZeroMatrix.M[GET0_NUMBER][M];
					BoneSpaceTransform.Colum[M].Y = ZeroMatrix.M[GET1_NUMBER][M];
					BoneSpaceTransform.Colum[M].Z = ZeroMatrix.M[GET2_NUMBER][M];
					BoneSpaceTransform.Colum[M].W = ZeroMatrix.M[GET3_NUMBER][M];
				}
			}
			break;

		default:
			BoneSpaceTransform = FTurboSequence_TransposeMatrix_Lf();
			break;
		}
	}
	else
	{
		const FName& BoneName = GetSkeletonBoneName(ReferenceSkeleton, BoneIndex);
		if (int32 BoneIdx = GetSkeletonBoneIndex(AnimationSkeleton, BoneName); BoneIdx > INDEX_NONE)
		{
			BoneSpaceTransform = Pose.LocalSpacePoses[LibraryData.BoneNameToAnimationBoneIndex[BoneName]];
		}
		else
		{
			const FMatrix& RefPoseMatrix = GetSkeletonRefPose(ReferenceSkeleton)[GET0_NUMBER].ToMatrixWithScale();

			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				BoneSpaceTransform.Colum[M].X = RefPoseMatrix.M[GET0_NUMBER][M];
				BoneSpaceTransform.Colum[M].Y = RefPoseMatrix.M[GET1_NUMBER][M];
				BoneSpaceTransform.Colum[M].Z = RefPoseMatrix.M[GET2_NUMBER][M];
				BoneSpaceTransform.Colum[M].W = RefPoseMatrix.M[GET3_NUMBER][M];
			}
		}
	}

	return BoneSpaceTransform;
}

void FTurboSequence_Utility_Lf::GetBoneTransformFromAnimationSafe(FMatrix& OutAtom,
                                                                  const FAnimationMetaData_Lf& Animation,
                                                                  uint16 SkeletonBoneIndex,
                                                                  const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
                                                                  const FSkinnedMeshGlobalLibrary_Lf& Library,
                                                                  const FReferenceSkeleton& ReferenceSkeleton)
{
	//OutAtom = FMatrix::Identity;
	if (IsValid(Animation.Animation))
	{
		const FAnimationLibraryData_Lf& LibraryData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

		const FReferenceSkeleton& AnimationSkeleton = Animation.Animation->GetSkeleton()->GetReferenceSkeleton();

		if (LibraryData.AnimPoses.Contains(Animation.CPUAnimationIndex_0))
		{
			const FTurboSequence_TransposeMatrix_Lf& Matrix = GetBoneTransformFromLocalPoses(
				SkeletonBoneIndex, LibraryData, ReferenceSkeleton,
				AnimationSkeleton,
				LibraryData.AnimPoses[Animation.CPUAnimationIndex_0].Pose,
				Animation.Animation);

			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				OutAtom.M[GET0_NUMBER][M] = Matrix.Colum[M].X;
				OutAtom.M[GET1_NUMBER][M] = Matrix.Colum[M].Y;
				OutAtom.M[GET2_NUMBER][M] = Matrix.Colum[M].Z;
				OutAtom.M[GET3_NUMBER][M] = Matrix.Colum[M].W;
			}

			// OutAtom.M[GET0_NUMBER][GET0_NUMBER] = CurrentRow0.X;
			// OutAtom.M[GET0_NUMBER][GET1_NUMBER] = CurrentRow1.X;
			// OutAtom.M[GET0_NUMBER][GET2_NUMBER] = CurrentRow2.X;
			// OutAtom.M[GET0_NUMBER][GET3_NUMBER] = GET0_NUMBER;
			//
			// OutAtom.M[GET1_NUMBER][GET0_NUMBER] = CurrentRow0.Y;
			// OutAtom.M[GET1_NUMBER][GET1_NUMBER] = CurrentRow1.Y;
			// OutAtom.M[GET1_NUMBER][GET2_NUMBER] = CurrentRow2.Y;
			// OutAtom.M[GET1_NUMBER][GET3_NUMBER] = GET0_NUMBER;
			//
			// OutAtom.M[GET2_NUMBER][GET0_NUMBER] = CurrentRow0.Z;
			// OutAtom.M[GET2_NUMBER][GET1_NUMBER] = CurrentRow1.Z;
			// OutAtom.M[GET2_NUMBER][GET2_NUMBER] = CurrentRow2.Z;
			// OutAtom.M[GET2_NUMBER][GET3_NUMBER] = GET0_NUMBER;
			//
			// OutAtom.M[GET3_NUMBER][GET0_NUMBER] = CurrentRow0.W;
			// OutAtom.M[GET3_NUMBER][GET1_NUMBER] = CurrentRow1.W;
			// OutAtom.M[GET3_NUMBER][GET2_NUMBER] = CurrentRow2.W;
			// OutAtom.M[GET3_NUMBER][GET3_NUMBER] = GET1_NUMBER;
		}
		else
		{
			OutAtom = FMatrix::Identity;
		}
	}
	else
	{
		//const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Asset);
		if (GetSkeletonIsValidIndex(ReferenceSkeleton, SkeletonBoneIndex))
		{
			OutAtom = GetSkeletonRefPose(ReferenceSkeleton)[SkeletonBoneIndex].ToMatrixWithScale();
		}
		else
		{
			OutAtom = FMatrix::Identity;
		}
	}
}

FTransform FTurboSequence_Utility_Lf::BendBoneFromAnimations(uint16 BoneIndex, const FSkinnedMeshRuntime_Lf& Runtime,
                                                             const FSkinnedMeshReference_Lf& Reference,
                                                             const FSkinnedMeshGlobalLibrary_Lf& Library)
{
	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);

	FTransform OutAtom = FTransform::Identity;
	bool bFoundFirstAnimation = false;
	float FullScalarWeight = GET0_NUMBER;
	const uint16 NumAnimations = Runtime.AnimationMetaData.Num();
	for (uint16 AnimIdx = GET0_NUMBER; AnimIdx < NumAnimations; ++AnimIdx)
	{
		const FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[AnimIdx];

		float LayerWeight = GET1_NUMBER;
		if (Library.AnimationBlendLayerMasks.IsValidIndex(Animation.LayerMaskIndex) && Library.
			AnimationBlendLayerMasks[Animation.LayerMaskIndex].RawAnimationLayers.IsValidIndex(BoneIndex))
		{
			LayerWeight = static_cast<float>(Library.AnimationBlendLayerMasks[Animation.LayerMaskIndex].
				RawAnimationLayers[BoneIndex]) / static_cast<float>(0x7FFF);
		}

		float Scalar = FTurboSequence_Helper_Lf::Clamp01(Animation.FinalAnimationWeight * LayerWeight);
		FullScalarWeight += Scalar;
		if (FullScalarWeight > GET1_NUMBER)
		{
			Scalar -= FMath::Abs(1.0f - FullScalarWeight);
		}

		if (!Scalar/* || !RotationWeight*/)
		{
			continue;
		}

		FMatrix BoneSpaceAtom = FMatrix::Identity;

		// Since we get the data directly from the GPU Collection Data we can likely ignore the root state,
		// the correct root bone data already baked into the collection .
		GetBoneTransformFromAnimationSafe(BoneSpaceAtom, Animation, BoneIndex, Runtime.DataAsset, Library,
		                                  ReferenceSkeleton);

		const FTransform& Atom = FTransform(BoneSpaceAtom);

		if (!bFoundFirstAnimation)
		{
			bFoundFirstAnimation = true;
			OutAtom.SetScale3D(Atom.GetScale3D() * Scalar);
			OutAtom.SetTranslation(Atom.GetTranslation() * Scalar);
			// float RotationW = RotationWeight;
			// FullRotationWeight += RotationW;
			// if (FullRotationWeight > GET1_NUMBER)
			// {
			// 	RotationW -= FMath::Abs(1.0f - FullRotationWeight);
			// }
			OutAtom.SetRotation(Atom.GetRotation() * FTurboSequence_Helper_Lf::Clamp01(Scalar));
		}
		else
		{
			OutAtom.SetScale3D(OutAtom.GetScale3D() + Atom.GetScale3D() * Scalar);
			OutAtom.SetTranslation(OutAtom.GetTranslation() + Atom.GetTranslation() * Scalar);
			// float RotationW = RotationWeight;
			// FullRotationWeight += RotationW;
			// if (FullRotationWeight > GET1_NUMBER)
			// {
			// 	RotationW -= FMath::Abs(1.0f - FullRotationWeight);
			// }
			OutAtom.SetRotation(FTurboSequence_Helper_Lf::Scale_Quaternion(
				OutAtom.GetRotation(), Atom.GetRotation(),
				FTurboSequence_Helper_Lf::Clamp01(Scalar)).GetNormalized());
		}
	}
	OutAtom.NormalizeRotation();
	return OutAtom;
}

void FTurboSequence_Utility_Lf::ExtractRootMotionFromAnimations(FTransform& OutAtom,
                                                                const FSkinnedMeshRuntime_Lf& Runtime,
                                                                const FSkinnedMeshReference_Lf& Reference,
                                                                float DeltaTime)
{
	FMatrix OutputTransform = FMatrix::Identity;
	for (const FAnimationMetaData_Lf& Animation : Runtime.AnimationMetaData)
	{
		if (ETurboSequence_RootMotionMode_Lf Mode = Animation.Settings.RootMotionMode;
			IsValid(Animation.Animation) && Animation.Animation->HasRootMotion() && (Mode ==
				ETurboSequence_RootMotionMode_Lf::Force || (Mode ==
					ETurboSequence_RootMotionMode_Lf::OnRootBoneAnimated && Animation.bIsRootBoneAnimation)))
		{
			FTransform RootMotion_Transform = Animation.Animation->ExtractRootMotion(
				Animation.AnimationTime, DeltaTime, Animation.bIsLoop);

			float Scalar = Animation.FinalAnimationWeight * Animation.Settings.AnimationSpeed;

			OutputTransform = OutputTransform * (GET1_NUMBER - Scalar) + RootMotion_Transform.ToMatrixWithScale() *
				Scalar;
		}
	}
	OutAtom = FTransform(OutputTransform);
}

void FTurboSequence_Utility_Lf::GetIKTransform(FTransform& OutAtom, uint16 BoneIndex, FSkinnedMeshRuntime_Lf& Runtime,
                                               const FSkinnedMeshReference_Lf& Reference,
                                               FSkinnedMeshGlobalLibrary_Lf& Library,
                                               FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                               const EBoneSpaces::Type Space,
                                               float AnimationDeltaTime, int32 CurrentFrameCount,
                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	SolveAnimations(Runtime, Library, Library_RenderThread, Reference, AnimationDeltaTime, CurrentFrameCount,
	                ThreadContext);

	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
	OutAtom = FTransform::Identity;
	int32 RuntimeIndex = BoneIndex;
	while (RuntimeIndex != INDEX_NONE)
	{
		if (Runtime.IKData.Contains(RuntimeIndex) && Runtime.IKData[RuntimeIndex].bIsInUsingWriteDataThisFrame)
		{
			OutAtom *= Runtime.IKData[RuntimeIndex].IKWriteTransform;
			break;
		}

		OutAtom *= BendBoneFromAnimations(RuntimeIndex, Runtime, Reference, Library);

		RuntimeIndex = GetSkeletonParentIndex(ReferenceSkeleton, RuntimeIndex);
	}

	if (Space == EBoneSpaces::Type::WorldSpace)
	{
		OutAtom *= Runtime.WorldSpaceTransform;
	}
}

bool FTurboSequence_Utility_Lf::SetIKTransform(const FTransform& Atom, uint16 BoneIndex,
                                               FSkinnedMeshRuntime_Lf& Runtime,
                                               const FSkinnedMeshReference_Lf& Reference,
                                               FCriticalSection& CriticalSection,
                                               const EBoneSpaces::Type Space)
{
	if (!Reference.LevelOfDetails[Runtime.LodIndex].CPUBoneToGPUBoneIndicesMap.Contains(BoneIndex))
	{
		return false;
	}

	FTransform IKTransform = Atom;

	// We only work with local space on the GPU
	// which mean we need to bring the World Space Transform into Local Space
	if (Space == EBoneSpaces::Type::WorldSpace)
	{
		IKTransform *= Runtime.WorldSpaceTransform.Inverse();
	}

	if (Runtime.IKData.Contains(BoneIndex))
	{
		Runtime.IKData[BoneIndex].bIsInUsingWriteDataThisFrame = true;
		Runtime.IKData[BoneIndex].IKWriteTransform = IKTransform;
		Runtime.IKData[BoneIndex].AliveCount = GET0_NUMBER;
	}
	else
	{
		FIKBoneData_Lf Data;
		Data.bIsInUsingWriteDataThisFrame = true;
		Data.IKWriteTransform = IKTransform;
		Data.AliveCount = GET0_NUMBER;
		CriticalSection.Lock();
		Runtime.IKData.Add(BoneIndex, Data);
		CriticalSection.Unlock();
	}
	Runtime.bIKDataInUse = true;

	return true;
}


FTurboSequence_PoseCurveData_Lf FTurboSequence_Utility_Lf::GetAnimationCurveByAnimation(
	const FSkinnedMeshRuntime_Lf& Runtime, const FAnimationMetaData_Lf& Animation,
	const FSkinnedMeshGlobalLibrary_Lf& Library, const FName& CurveName)
{
	if (IsValid(Animation.Animation))
	{
		const FAnimationLibraryData_Lf& LibraryData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

		if (LibraryData.AnimPoses.Contains(Animation.CPUAnimationIndex_0))
		{
			float PoseCurve0 = FTurboSequence_Helper_Lf::GetAnimationCurveWeight(
				LibraryData.AnimPoses[Animation.CPUAnimationIndex_0].Pose, CurveName);

			return FTurboSequence_PoseCurveData_Lf(Animation.Animation, CurveName, PoseCurve0);
		}
	}

	return FTurboSequence_PoseCurveData_Lf();
}

TArray<FTurboSequence_PoseCurveData_Lf> FTurboSequence_Utility_Lf::GetAnimationCurveAtTime(
	const FSkinnedMeshRuntime_Lf& Runtime, const FName& CurveName, const FSkinnedMeshGlobalLibrary_Lf& Library,
	FCriticalSection& CriticalSection)
{
	CriticalSection.Lock();
	TArray<FTurboSequence_PoseCurveData_Lf> Array;
	CriticalSection.Unlock();
	for (const FAnimationMetaData_Lf& Animation : Runtime.AnimationMetaData)
	{
		const FTurboSequence_PoseCurveData_Lf& Curve = GetAnimationCurveByAnimation(
			Runtime, Animation, Library, CurveName);
		if (IsValid(Curve.CurveAnimation))
		{
			CriticalSection.Lock();
			Array.Add(Curve);
			CriticalSection.Unlock();
		}
	}
	return Array;
}

void FTurboSequence_Utility_Lf::GetSocketTransform(FTransform& OutTransform, const FName& SocketName,
                                                   FSkinnedMeshRuntime_Lf& Runtime,
                                                   const FSkinnedMeshReference_Lf& Reference,
                                                   FSkinnedMeshGlobalLibrary_Lf& Library,
                                                   FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                                   const EBoneSpaces::Type Space,
                                                   float AnimationDeltaTime, int32 CurrentFrameCount,
                                                   const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	const USkeletalMeshSocket* Socket = Runtime.DataAsset->ReferenceMeshNative->FindSocket(SocketName);
	int32 BoneIndex = GetSkeletonBoneIndex(GetReferenceSkeleton(Runtime.DataAsset), Socket->BoneName);

	GetIKTransform(OutTransform, BoneIndex, Runtime, Reference, Library, Library_RenderThread, Space,
	               AnimationDeltaTime, CurrentFrameCount, ThreadContext);

	OutTransform = FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale) *
		OutTransform;
}
