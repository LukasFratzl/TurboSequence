// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TurboSequence_Data_Lf.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "TurboSequence_GlobalData_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "Animation/AnimationPoseData.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Rendering/SkeletalMeshRenderData.h"

class TURBOSEQUENCE_LF_API FTurboSequence_Utility_Lf
{
public:
	FTurboSequence_Utility_Lf() = delete;
	~FTurboSequence_Utility_Lf() = delete;

	static uint32 CreateRenderer(FSkinnedMeshReference_Lf& Reference,
	                             const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
	                             const TObjectPtr<UNiagaraSystem> RenderReference,
	                             const TMap<uint8, FSkinnedMeshReferenceLodElement_Lf>&
	                             LevelOfDetails,
	                             const TObjectPtr<USceneComponent> InstanceSceneComponent,
	                             TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>,
	                                  FRenderingMaterialMap_Lf>& RenderComponents,
	                             const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                             const TArray<TObjectPtr<UMaterialInterface>>& Materials,
	                             const uint32& MaterialsHash)
	{

		if (RenderComponents.Contains(FromAsset) && RenderComponents[FromAsset].NiagaraRenderer.Contains(MaterialsHash))
		{
			return MaterialsHash;
		}

		// Create the Render Data first

		FRenderData_Lf RenderData = FRenderData_Lf(GlobalData->NameNiagaraEmitter,
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

		Component->SetVisibleInRayTracing(true);
		
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

					RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetVariableStaticMesh(
						WantedMeshName, LodElement.Mesh);
				}
			}
		}

		// Set the Materials
		const int32& NumMaterials = Materials.Num();
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

			RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetVariableMaterial(
				WantedMaterialName, MaterialInstance);

			// For Debugging nice...
			RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].Materials[MaterialIdx].MaterialValue = MaterialInstance;
		}

		// Set Bounds
		const uint8& NumLevelOfDetails = LevelOfDetails.Num();
		for (int16 i = NumLevelOfDetails - GET1_NUMBER; i >= GET0_NUMBER; --i)
		{
			if (LevelOfDetails[i].bIsRenderStateValid)
			{
				// Basically 5 000 Meter Radius
				constexpr float BoundsExtend = GET1000_NUMBER * GET1000_NUMBER / GET2_NUMBER;
				const FBox& Bounds = FBox(FVector::OneVector * -BoundsExtend, FVector::OneVector * BoundsExtend);
				RenderComponents[FromAsset].NiagaraRenderer[MaterialsHash].NiagaraRenderer->SetEmitterFixedBounds(
					Reference.RenderData[MaterialsHash].GetEmitterName(), Bounds);
				break;
			}
		}

		return MaterialsHash;
	}

	static void UpdateCameras(TArray<FCameraView_Lf>& OutView, const UWorld* InWorld)
	{
		const bool bIsSinglePlayer = UGameplayStatics::GetNumPlayerControllers(InWorld) < GET2_NUMBER;
		const int32 MaxNumberPlayerControllers = UGameplayStatics::GetNumPlayerControllers(InWorld);
		OutView.SetNum(MaxNumberPlayerControllers);
		for (int32 ViewIdx = GET0_NUMBER; ViewIdx < MaxNumberPlayerControllers; ++ViewIdx)
		{
			const TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(
				InWorld, ViewIdx);
			const TObjectPtr<ULocalPlayer> LocalPlayer = PlayerController->GetLocalPlayer();

			PlayerController->PlayerCameraManager->UpdateCamera(GET0_NUMBER);

			const FMinimalViewInfo& PlayerMinimalViewInfo = PlayerController->PlayerCameraManager->ViewTarget.POV;

			FVector2D ViewportSize;
			LocalPlayer->ViewportClient->GetViewportSize(ViewportSize);
			const FVector2D SplitScreenPlayerViewSize(LocalPlayer->Size.X * ViewportSize.X,
			                                          LocalPlayer->Size.Y * ViewportSize.Y);

			FVector2f ViewportDimensions = FVector2f(ViewportSize.X, ViewportSize.Y);
			
			float Fov = PlayerController->PlayerCameraManager->GetFOVAngle();
			if (!bIsSinglePlayer)
			{
				ViewportDimensions = FVector2f(SplitScreenPlayerViewSize.X / SplitScreenPlayerViewSize.Y);


				float AspectRatio;
				if (((ViewportSize.X > ViewportSize.Y) && (LocalPlayer->AspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)) || (LocalPlayer->AspectRatioAxisConstraint == AspectRatio_MaintainXFOV))
				{
					//if the viewport is wider than it is tall
					AspectRatio = ViewportDimensions.X / (float)ViewportDimensions.Y;
				}
				else
				{
					//if the viewport is taller than it is wide
					AspectRatio = ViewportDimensions.Y / (float)ViewportDimensions.X;
				}
				
				Fov = FMath::RadiansToDegrees(GET2_NUMBER * FMath::Atan(
									  FMath::Tan(FMath::DegreesToRadians(Fov) * 0.5f) * AspectRatio));
			}

			FCameraView_Lf View;
			View.Fov = Fov; // * 1.1f;
			View.ViewportSize = ViewportDimensions;
			View.AspectRatioAxisConstraint = LocalPlayer->AspectRatioAxisConstraint;
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

	static void UpdateCameras_1(TArray<FCameraView_Lf>& OutViews,
	                            const TMap<uint8, FTransform>& LastFrameCameraTransforms,
	                            const UWorld* InWorld, const float& DeltaTime)
	{
		UpdateCameras(OutViews, InWorld);
		const float& Interpolation = GET1_NUMBER; //GET2_NUMBER + DeltaTime * GET4_NUMBER;
		const uint8& NumCameraViews = OutViews.Num();
		for (uint8 i = GET0_NUMBER; i < NumCameraViews; ++i)
		{
			FCameraView_Lf& View = OutViews[i];

			const float& InterpolatedFov = View.Fov; // + GET5_NUMBER * DeltaTime;
			FTurboSequence_Helper_Lf::GetCameraFrustumPlanes_ObjectSpace(
				View.Planes_Internal, InterpolatedFov, View.ViewportSize, View.AspectRatioAxisConstraint, View.NearClipPlane, View.FarClipPlane,
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

	static void UpdateCameras_2(TMap<uint8, FTransform>& OutLastFrameCameraTransforms,
	                            const TArray<FCameraView_Lf>& CameraViews)
	{
		const uint8& MaxNumberLastFrameCameras = FMath::Max(OutLastFrameCameraTransforms.Num(), CameraViews.Num());
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

	static void IsMeshVisible(FSkinnedMeshRuntime_Lf& Runtime,
	                          const FSkinnedMeshReference_Lf& Reference,
	                          const TArray<FCameraView_Lf>& PlayerViews)
	{
		const FVector& MeshLocation = GetWorldSpaceTransformIncludingOffsets(Runtime).GetLocation();
		const FBoxSphereBounds& Bounds = Reference.FirstValidMeshLevelOfDetail->GetBounds();
		const FBox Box(MeshLocation - Bounds.BoxExtent, MeshLocation + Bounds.BoxExtent);

		bool bIsVisibleOnAnyCamera = false;
		for (const FCameraView_Lf& View : PlayerViews)
		{
			if (FTurboSequence_Helper_Lf::Box_Intersects_With_Frustum(Box, View.Planes_Internal,
			                                                          View.InterpolatedCameraTransform_Internal,
			                                                          Bounds.SphereRadius))
			{
				bIsVisibleOnAnyCamera = true;
				break;
			}
		}

		Runtime.bIsVisible = bIsVisibleOnAnyCamera;
	}

	static int32 GetBoneMapIndex_CPU(const TArray<FBoneIndexType>& FromSection,
	                                 const int32& RawIndex
	                                 /*, const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset*/)
	{
		if (!FromSection.IsValidIndex(RawIndex))
		{
			return INDEX_NONE;
		}

		const int32& MeshIndex = FromSection[RawIndex];

		return MeshIndex;
	}

	static int32 GetBoneMapIndex_GPU(const TMap<uint16, uint16>& FromMap,
	                                 const TArray<FBoneIndexType>& FromSection,
	                                 const int32& RawIndex, const uint8& Weight)
	{
		if (const int32& ReferenceIndex = GetBoneMapIndex_CPU(FromSection, RawIndex); Weight && FromMap.Contains(
			ReferenceIndex))
		{
			return FromMap[ReferenceIndex];
		}
		return GET0_NUMBER;
	}

	static int32 GetValidBoneData(const int32& FromData, const uint8& CurrentInfluenceIndex,
	                              const uint8& MaxInfluenceIndex)
	{
		if (CurrentInfluenceIndex <= MaxInfluenceIndex)
		{
			return FromData;
		}
		return INDEX_NONE;
	}

	static void CacheTurboSequenceAssets(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                     FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                     Library_RenderThread, FCriticalSection& CriticalSection)
	{
		const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");
		TArray<FAssetData> AssetClasses;
		AssetRegistry.Get().GetAssetsByClass(
			FTopLevelAssetPath(UTurboSequence_MeshAsset_Lf::StaticClass()->GetPathName()), AssetClasses);

		const int32& NumAssets = AssetClasses.Num();
		FTurboSequence_Helper_Lf::SortAssetsByPathName(AssetClasses);

		for (int32 AssetIdx = GET0_NUMBER; AssetIdx < NumAssets; ++AssetIdx)
		{
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Cast<UTurboSequence_MeshAsset_Lf>(
				AssetClasses[AssetIdx].GetAsset());
			if (!Library.PerReferenceData.Contains(Asset))
			{
				CreateTurboSequenceReference(Library, Library_RenderThread, CriticalSection, Asset);
			}
		}
	}

	static void CreateAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                       const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
	                                       FSkinnedMeshGlobalLibrary_Lf& Library,
	                                       FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                       FCriticalSection& CriticalSection);

	static void RefreshAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
	                                        FSkinnedMeshGlobalLibrary_Lf& Library,
	                                        FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                        FCriticalSection& CriticalSection);

	static void CreateTurboSequenceReference(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                         FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                         FCriticalSection& CriticalSection,
	                                         const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset);

	static void CreateLevelOfDetails(FSkinnedMeshReference_Lf& Reference,
	                                 FSkinnedMeshReference_RenderThread_Lf&
	                                 Reference_RenderThread, FCriticalSection& CriticalSection,
	                                 const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                 const bool& bIsMeshDataEvaluationFunction = false)
	{
		if (bIsMeshDataEvaluationFunction)
		{
			FromAsset->MeshData.Empty();
		}

		const uint8& MaxNumLevelOfDetails = FromAsset->InstancedMeshes.Num();
		const int32& MaxLevelOfDetailRange = FromAsset->AutoLodRatio;
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

			const bool& bIsIncluded = !FromAsset->InstancedMeshes[i].bExcludeLodFromSystem ||
				bIsMeshDataEvaluationFunction;

			const bool& bIsMeshVisible = IsValid(FromAsset->InstancedMeshes[i].StaticMesh) && FromAsset->InstancedMeshes
				[i].bShowMesh;

			if (bIsMeshVisible && bIsIncluded)
			{
				// Get the Mesh Index so the Skin FinalAnimationWeight AnimationOutputTexture can adapt the data
				uint8 NumLevelOfDetails = FromAsset->MeshData.Num();
				if (bIsMeshDataEvaluationFunction)
				{
					NumLevelOfDetails = FromAsset->ReferenceMeshEdited->GetLODNum();
				}
				const uint32& NumVerticesInstancedMesh = FromAsset->InstancedMeshes[i].StaticMesh->GetNumVertices(
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

	static void CreateMaxLodAndCPUBones(FSkinnedMeshGlobalLibrary_Lf& Library)
	{
		Library.MaxNumCPUBones = GET0_NUMBER;
		Library.MaxNumLevelOfDetailsWithMesh = GET0_NUMBER;
		for (const TTuple<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_Lf>& ReferenceOther : Library.
		     PerReferenceData)
		{
			if (const int32& NumBones = GetSkeletonNumBones(GetReferenceSkeleton(ReferenceOther.Key)); Library.
				MaxNumCPUBones < NumBones)
			{
				Library.MaxNumCPUBones = NumBones;
			}
			if (const int32& NumLevelOfDetails = ReferenceOther.Value.NumLevelOfDetailsWithMesh; Library.
				MaxNumLevelOfDetailsWithMesh < NumLevelOfDetails)
			{
				Library.MaxNumLevelOfDetailsWithMesh = NumLevelOfDetails;
			}
		}
	}

	static void CreateGPUBones(FSkinnedMeshReferenceLodElement_Lf& LodElement, FCriticalSection& CriticalSection,
	                           const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, const bool& bIsMeshDataEvaluation);

	static void CreateBoneMaps(FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           FCriticalSection& CriticalSection);

	static void CreateRawSkinWeightTextureBuffer(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                             const TFunction<void(bool bSuccess)>& PostCall,
	                                             const TObjectPtr<UWorld> World);


	static void CreateInverseReferencePose(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                       FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                       Library_RenderThread,
	                                       FCriticalSection& CriticalSection)
	{
		const int32& NumReferences = Library.PerReferenceData.Num();
		TArray<FVector4f> InvRefPoseData;

		if (const int32& RestPoseBufferAddition = NumReferences * Library.MaxNumCPUBones * GET3_NUMBER)
		{
			CriticalSection.Lock();
			InvRefPoseData.AddUninitialized(RestPoseBufferAddition);
			CriticalSection.Unlock();

			//FCriticalSection Mutex;
			ParallelFor(NumReferences, [&](const int32& Index)
			{
				const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Library.PerReferenceDataKeys[Index];
				FSkinnedMeshReference_Lf& Reference = Library.PerReferenceData[Asset];

				const uint16& NumBones = Reference.NumCPUBones;
				CriticalSection.Lock();
				Reference.ComponentSpaceRestPose.AddUninitialized(NumBones);
				CriticalSection.Unlock();
				const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Asset);
				const TArray<FTransform>& ReferencePose = GetSkeletonRefPose(ReferenceSkeleton);
				const int32& InvRestPoseBaseIndex = Index * Library.MaxNumCPUBones * GET3_NUMBER;
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

	static void ResizeBuffers(FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                          const int32& Num)
	{
		// Use this fast first for the bounce check on the render thread
		Library.BoneTransformParams.PerMeshCustomDataIndex_RenderThread.SetNum(Num);

		// Use this for the last one as check in the Render Thread
		Library.BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread.SetNum(Num);
	}

	static bool IsValidBufferIndex(const FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                               const int32& Index)
	{
		if (!(Library.BoneTransformParams.PerMeshCustomDataIndex_RenderThread.IsValidIndex(Index) && Library.BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread.IsValidIndex(Index)))
		{
			return false;
		}

		return true;
	}

	static void RemoveAnimationFromLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                              FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                              FCriticalSection& CriticalSection);

	static int32 AddAnimationToLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                          FCriticalSection& CriticalSection, int32& CPUIndices,
	                                          const FSkinnedMeshRuntime_Lf& Runtime,
	                                          const FAnimationMetaData_Lf& Animation);
	
	static void CustomizeMesh(FSkinnedMeshRuntime_Lf& Runtime, const TObjectPtr<UTurboSequence_MeshAsset_Lf> TargetMesh,
	                          const TArray<TObjectPtr<UMaterialInterface>>& TargetMaterials,
	                          TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>&
	                          NiagaraComponents, FSkinnedMeshGlobalLibrary_Lf& Library,
	                          FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                          const TObjectPtr<USceneComponent> RootComponent, FCriticalSection& CriticalSection);

	static FTransform GetWorldSpaceTransformIncludingOffsets(
		const FSkinnedMeshRuntime_Lf& Runtime)
	{
		return Runtime.WorldSpaceTransform; //Runtime.DeltaOffsetTransform * Runtime.WorldSpaceTransform;
	}

	/**
		* @brief Updates one instance in an instanced static mesh component
		* @param Reference The Reference to get the instance index
		* @param Runtime The Mesh of the Instance we like to update
		* @param WorldSpaceTransform The transform matrix we like to update
		*/
	static void UpdateInstanceTransform_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FTransform& WorldSpaceTransform)
	{
		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		RenderData.bCollectionDirty = true;

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

		RenderData.ParticlePositions[InstanceIndex] = WorldSpaceTransform.GetLocation();
		RenderData.ParticleRotations[InstanceIndex] = FTurboSequence_Helper_Lf::ConvertQuaternionToVector4F(
			WorldSpaceTransform.GetRotation());
		RenderData.ParticleScales[InstanceIndex] = FTurboSequence_Helper_Lf::ConvertVectorToVector3F(
			WorldSpaceTransform.GetScale3D());
	}

	/**
	 * @brief Adds an instance to the given renderer with custom data
	 * @param Reference The Reference To add into the InstanceMap
	 * @param Runtime The Runtime Data
	 * @param CriticalSection The Critical Section for Multi Threading ability
	 * @param WorldSpaceTransform The world location we like to add the instance
	 */
	static void AddRenderInstance(FSkinnedMeshReference_Lf& Reference,
	                              const FSkinnedMeshRuntime_Lf& Runtime,
	                              FCriticalSection& CriticalSection,
	                              const FTransform& WorldSpaceTransform = FTransform::Identity)
	{
		// if (!Reference.RenderData.Contains(Runtime.MaterialsHash))
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Failed to Add data at... %s, %d"), *Runtime.DataAsset->GetPathName(), Runtime.MaterialsHash);
		// 	return;
		// }

		FScopeLock Lock(&CriticalSection);

		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		RenderData.bCollectionDirty = true;

		//CriticalSection.Lock();
		const int32 InstanceIndex = RenderData.InstanceMap.Num();
		RenderData.InstanceMap.Add(Runtime.GetMeshID(), InstanceIndex);

		RenderData.ParticlePositions.Add(WorldSpaceTransform.GetLocation());
		RenderData.ParticleRotations.Add(
			FTurboSequence_Helper_Lf::ConvertQuaternionToVector4F(WorldSpaceTransform.GetRotation()));
		RenderData.ParticleScales.Add(
			FTurboSequence_Helper_Lf::ConvertVectorToVector3F(WorldSpaceTransform.GetScale3D()));

		RenderData.ParticleLevelOfDetails.Add(GET0_NUMBER);

		RenderData.ParticleCustomData.AddDefaulted(FTurboSequence_Helper_Lf::NumInstanceCustomData);
		//CriticalSection.Unlock();
		//SetCustomDataForInstance(Reference, LodElement, Runtime);
	}

	static void CleanNiagaraRenderer(TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>& NiagaraComponents, FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime)
	{
		NiagaraComponents[Runtime.DataAsset].NiagaraRenderer[Runtime.MaterialsHash].NiagaraRenderer->DestroyComponent();
		NiagaraComponents[Runtime.DataAsset].NiagaraRenderer.Remove(Runtime.MaterialsHash);
		if (!NiagaraComponents[Runtime.DataAsset].NiagaraRenderer.Num())
		{
			NiagaraComponents.Remove(Runtime.DataAsset);
		}
		Reference.RenderData.Remove(Runtime.MaterialsHash);
	}


	/**
	 * @brief Removes an instance from the renderer
	 * @param Reference The Reference to remove from the InstanceMap
	 * @param Runtime The Mesh of the Instance you like to remove
	 */
	static void RemoveRenderInstance(FSkinnedMeshReference_Lf& Reference,
	                                 const FSkinnedMeshRuntime_Lf& Runtime,
	                                 FCriticalSection& CriticalSection)
	{
		FScopeLock Lock(&CriticalSection);

		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		RenderData.bCollectionDirty = true;

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];
		//CriticalSection.Lock();
		RenderData.InstanceMap.Remove(Runtime.GetMeshID());
		for (TTuple<uint32, int32>& Instance : RenderData.InstanceMap)
		{
			if (Instance.Value > InstanceIndex)
			{
				Instance.Value--;
			}
		}

		RenderData.ParticlePositions.RemoveAt(InstanceIndex);
		RenderData.ParticleRotations.RemoveAt(InstanceIndex);
		RenderData.ParticleScales.RemoveAt(InstanceIndex);
		RenderData.ParticleLevelOfDetails.RemoveAt(InstanceIndex);

		RenderData.ParticlesToRemove.Add(InstanceIndex);

		for (int16 i = FTurboSequence_Helper_Lf::NumInstanceCustomData - GET1_NUMBER; i >= GET0_NUMBER; --i)
		{
			const int32& CustomDataIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData + i;
			RenderData.ParticleCustomData.RemoveAt(CustomDataIndex);
		}
		//CriticalSection.Unlock();
	}

	static void UpdateRenderInstanceLod_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FSkinnedMeshReferenceLodElement_Lf& LodElement, const bool& bIsVisible)
	{
		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		RenderData.bCollectionDirty = true;

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

		RenderData.ParticleLevelOfDetails[InstanceIndex] = bIsVisible * LodElement.GPUMeshIndex + !bIsVisible *
			FTurboSequence_Helper_Lf::NotVisibleMeshIndex;
	}

	static void SetCustomDataForInstance(FSkinnedMeshReference_Lf& Reference,
	                                     const int32& CPUIndex, const int32& SkinWeightOffset,
	                                     const FSkinnedMeshRuntime_Lf& Runtime,
	                                     const FSkinnedMeshGlobalLibrary_Lf&
	                                     Library)
	{
		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		RenderData.bCollectionDirty = true;

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

		const uint32 CustomDataBaseIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData;

		const FIntVector2& BitValueSkinWeightOffset = FTurboSequence_Helper_Lf::DecodeUInt32ToUInt16(
			(SkinWeightOffset + FTurboSequence_Helper_Lf::NumCustomStates) * Runtime.bIsVisible);
		RenderData.ParticleCustomData[CustomDataBaseIndex + GET0_NUMBER] = BitValueSkinWeightOffset.X;
		RenderData.ParticleCustomData[CustomDataBaseIndex + GET1_NUMBER] = BitValueSkinWeightOffset.Y;

		const FIntVector2& BitValuesTransformIndex = FTurboSequence_Helper_Lf::DecodeUInt32ToUInt16(
			CPUIndex * Library.MaxNumGPUBones *
			FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer + GET2_NUMBER);
		RenderData.ParticleCustomData[CustomDataBaseIndex + GET2_NUMBER] = BitValuesTransformIndex.X;
		RenderData.ParticleCustomData[CustomDataBaseIndex + GET3_NUMBER] = BitValuesTransformIndex.Y;
	}

	static bool SetCustomDataForInstance_User(FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime, const int16& CustomDataFractionIndex, const float& CustomDataValue)
	{
		if (CustomDataFractionIndex < GET4_NUMBER || CustomDataFractionIndex > FTurboSequence_Helper_Lf::NumInstanceCustomData)
		{
			return false;
		}
		
		FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash];

		if (!RenderData.InstanceMap.Contains(Runtime.GetMeshID()))
		{
			return false;
		}

		const int32 InstanceIndex = RenderData.InstanceMap[Runtime.GetMeshID()];

		const uint32 CustomDataIndex = InstanceIndex * FTurboSequence_Helper_Lf::NumInstanceCustomData + CustomDataFractionIndex;

		if (!RenderData.ParticleCustomData.IsValidIndex(CustomDataIndex))
		{
			return false;
		}
		
		RenderData.ParticleCustomData[CustomDataIndex] = CustomDataValue;
		RenderData.bCollectionDirty = true;
		
		return true;
	}


	static void UpdateCullingAndLevelOfDetail(FSkinnedMeshRuntime_Lf& Runtime,
	                                          FSkinnedMeshReference_Lf& Reference,
	                                          const TArray<FCameraView_Lf>& CameraViews,
	                                          TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext,
	                                          const bool& bVisibleLastFrame,
	                                          FSkinnedMeshGlobalLibrary_Lf& Library)
	{
		float ClosestCameraDistance = GET_INFINITY;
		const FVector& ComponentLocation = GetWorldSpaceTransformIncludingOffsets(Runtime).GetLocation();
		for (const FCameraView_Lf& CameraView : CameraViews)
		{
			if (const float& Distance = FVector::Distance(ComponentLocation, CameraView.CameraTransform.GetLocation());
				Distance < ClosestCameraDistance)
			{
				ClosestCameraDistance = Distance;
			}
		}
		Runtime.ClosestCameraDistance = ClosestCameraDistance;

		const bool& bIsInTSMeshDrawRange = !IsValid(Runtime.FootprintAsset) ||
		(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance >= Runtime.FootprintAsset->HybridModeMeshDrawRangeUEInstance);

		const bool& bIsInTSAnimationRange = !IsValid(Runtime.FootprintAsset) ||
		(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance >= Runtime.FootprintAsset->HybridModeAnimationDrawRangeUEInstance);

		if (bIsInTSMeshDrawRange)
		{
			if (Runtime.LodIndex > INDEX_NONE)
			{
				const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];
				UpdateRenderInstanceLod_Concurrent(Reference, Runtime, LodElement,
				                                   Runtime.bIsVisible && LodElement.bIsRenderStateValid);
			}
		}
		if (IsValid(Runtime.FootprintAsset))
		{
			Runtime.FootprintAsset->OnHybridModeUEInstanceAddRemove_Concurrent_Override(Runtime.GetMeshID(), ThreadContext);
		}


		const uint8& MaxNumLevelOfDetails = Reference.LevelOfDetails.Num();
		uint8 ClosestLevelOfDetailIndex = MaxNumLevelOfDetails - GET1_NUMBER;
		for (uint8 i = GET0_NUMBER; i < MaxNumLevelOfDetails; ++i)
		{
			if (Reference.LevelOfDetails[i].MinLodDistance <= ClosestCameraDistance && Reference.LevelOfDetails[i].
				MaxLodDistance > ClosestCameraDistance)
			{
				ClosestLevelOfDetailIndex = i;
				break;
			}
		}
		if (bIsInTSMeshDrawRange && ClosestLevelOfDetailIndex != Runtime.LodIndex)
		{
			const FSkinnedMeshReferenceLodElement_Lf& NextLodElement = Reference.LevelOfDetails[
				ClosestLevelOfDetailIndex];
			UpdateRenderInstanceLod_Concurrent(Reference, Runtime, NextLodElement,
			                                   Runtime.bIsVisible && NextLodElement.bIsRenderStateValid);
		}
		Runtime.LodIndex = ClosestLevelOfDetailIndex;
	}

	static void UpdateDistanceUpdating(FSkinnedMeshRuntime_Lf& Runtime, const float& DeltaTime)
	{
		if (Runtime.DataAsset->bUseDistanceUpdating)
		{
			const float& DistanceRatioSeconds = Runtime.ClosestCameraDistance / 250000 * Runtime.DataAsset->DistanceUpdatingRatio;
			Runtime.DeltaTimeAccumulator -= DeltaTime;
			if (Runtime.DeltaTimeAccumulator < DistanceRatioSeconds)
			{
				Runtime.DeltaTimeAccumulator = DistanceRatioSeconds;
				Runtime.bIsDistanceUpdatingThisFrame = true;
			}
			else
			{
				Runtime.bIsDistanceUpdatingThisFrame = false;
			}
		}
		else
		{
			Runtime.bIsDistanceUpdatingThisFrame = true;
		}
	}

	static void UpdateRendererBounds(FCriticalSection& Mutex,
	                                 FSkinnedMeshReference_Lf& Reference,
	                                 const FSkinnedMeshRuntime_Lf& Runtime)
	{
		// We assume it's called already inside bIsVisible

		const FBoxSphereBounds& Bounds = Reference.FirstValidMeshLevelOfDetail->GetBounds();
		const FVector& MeshLocation = GetWorldSpaceTransformIncludingOffsets(Runtime).GetLocation();
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

	static void UpdateCameraRendererBounds(FRenderData_Lf& RenderData,
	                                       const TArray<FCameraView_Lf>& CameraViews,
	                                       const int32& Extend)
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

	static uint32 GetAnimationLayerGroupHash(const TArray<FTurboSequence_BoneLayer_Lf>& Layers)
	{
		uint32 Hash = GET0_NUMBER;
		for (const FTurboSequence_BoneLayer_Lf& Layer : Layers)
		{
			FTurboSequence_Helper_Lf::HashDataTypeToHash(Hash, Layer.BoneLayerName);
		}
		return Hash;
	}

	static bool ContainsAnyAnimationLayer(const TArray<FTurboSequence_BoneLayer_Lf>& A,
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

	static bool ContainsRootBoneName(const TArray<FTurboSequence_BoneLayer_Lf>& Collection,
	                                 const FSkinnedMeshRuntime_Lf& Runtime)
	{
		const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
		for (const FTurboSequence_BoneLayer_Lf& Layer : Collection)
		{
			if (const int32& BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Layer.BoneLayerName); BoneIndex ==
				GET0_NUMBER)
			{
				return true;
			}
		}
		return false;
	}

	static uint32 GenerateAnimationLayerMask(FAnimationMetaData_Lf& Animation,
	                                         TArray<uint16>& OutLayers,
	                                         FCriticalSection& CriticalSection,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
	                                         const FSkinnedMeshReference_Lf& Reference,
	                                         const bool& bGenerateLayers)
	{
		const int16& NumCPUBones = Reference.NumCPUBones;

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
				if (const int16& BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Bone.BoneLayerName);
					GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndex))
				{
					bFoundBones = true;

					if (const int32& Depth = ReferenceSkeleton.GetDepthBetweenBones(i, BoneIndex); (ShortestDepth ==
						INDEX_NONE || ShortestDepth > Depth) && Depth != INDEX_NONE)
					{
						ShortestDepth = Depth;
						DepthAmount = Bone.BoneDepth;
					}
				}
			}
			if (!bFoundBones)
			{
				if (const int32& Depth = ReferenceSkeleton.GetDepthBetweenBones(i, GET0_NUMBER); (ShortestDepth ==
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
						if (const int16& BoneIndex = GetSkeletonBoneIndex(ReferenceSkeleton, Bone.BoneLayerName);
							GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndex))
						{
							bFoundBones = true;

							if (const int32& Depth = ReferenceSkeleton.GetDepthBetweenBones(i, BoneIndex); (
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
						if (const int32& Depth = ReferenceSkeleton.GetDepthBetweenBones(i, GET0_NUMBER); (
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
					const float& ClampedCurrent = FMath::Clamp(ShortestDepth_Other, GET0_NUMBER, DepthAmount_Other);

					Percentage_Other = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
						ClampedCurrent, GET0_NUMBER, DepthAmount_Other);
				}

				const float& ClampedCurrent = FMath::Clamp(ShortestDepth, GET0_NUMBER, DepthAmount);
				const float& RawPercentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
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

	static bool MergeAnimationLayerMask(const TArray<uint16>& AnimationLayers, const uint32& AnimationLayerHash,
	                                    FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                    const bool& bIsAdd)
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
				UE_LOG(LogTemp, Warning, TEXT("Could not found Animation Layer Mask Pattern %d"), AnimationLayerHash);
			}
		}


		return bEditedLayer;
	}

	static bool UpdatedAnimationLayerMaskIndex(FAnimationMetaData_Lf& Animation,
	                                           FSkinnedMeshRuntime_Lf& Runtime,
	                                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                                           const FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                           Library_RenderThread)
	{

		const int16& NumOperations = Library.AnimationBlendLayerMasks.Num();

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


	static void UpdateAnimationLayerMasks(FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                      FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread)
	{
		bool bEditedLayers = false;
		if (Library.AnimationBlendLayerMasksRuntimeDirty.Num())
		{

			const int16& NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;

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
						const int16& NumOperations = Library.AnimationBlendLayerMasks.Num();
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
						const uint32& NumLayers = NumOperations_RenderThread * NumCPUBones;
						Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread.SetNumUninitialized(
							NumLayers);

						ParallelFor(NumOperations_RenderThread, [&](const int32& Index)
						{
							const uint32& BaseIndex = Index * NumCPUBones;
							const uint32& MaxIndex = BaseIndex + NumCPUBones;
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
				const int32& NumMeshesPerThread = FMath::CeilToInt(
					static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
				ParallelFor(NumThreads, [&](const int32& ThreadsIndex)
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
					const uint32& NumLayers = NumOperations_RenderThread * NumCPUBones;
					Library_RenderThread.BoneTransformParams.AnimationLayers_RenderThread.
					                     SetNumUninitialized(NumLayers);

					ParallelFor(NumOperations_RenderThread, [&](const int32& Index)
					{
						const uint32& BaseIndex = Index * NumCPUBones;
						const uint32& MaxIndex = BaseIndex + NumCPUBones;
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
	
	static void AddAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                         const FSkinnedMeshReference_Lf& Reference,
	                         FAnimationMetaData_Lf& Animation,
	                         const FAnimationMetaData_RenderThread_Lf& Animation_RenderThread,
	                         FCriticalSection& CriticalSection,
	                         FSkinnedMeshGlobalLibrary_Lf& Library)
	{
		FScopeLock Lock(&CriticalSection);
		{
			TArray<uint16> Layers;
			Animation.AnimationLayerHash = GenerateAnimationLayerMask(
				Animation, Layers, CriticalSection, Runtime, Reference, false);

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
			
			Library.AnimationBlendLayerMasksRuntimeDirty.FindOrAdd(FUintVector2(Runtime.GetMeshID(), Animation.AnimationID), true);

			const int16& NumAnimations = Runtime.AnimationMetaData.Num();
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
	}

	
	static void RemoveAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            const int32& Index)
	{
		FScopeLock Lock(&CriticalSection);
		{
			const FAnimationMetaData_Lf& Animation = Runtime.AnimationMetaData[Index];
			const uint32 AnimationID = Animation.AnimationID;

			if (Runtime.AnimationGroups.Contains(Animation.AnimationGroupLayerHash))
			{
				Runtime.AnimationGroups[Animation.AnimationGroupLayerHash].NumAnimationsInGroup--;
				if (Runtime.AnimationGroups[Animation.AnimationGroupLayerHash].NumAnimationsInGroup <= GET0_NUMBER)
				{
					Runtime.AnimationGroups.Remove(Animation.AnimationGroupLayerHash);
				}
			}

			Library.AnimationBlendLayerMasksRuntimeDirty.FindOrAdd(FUintVector2(Runtime.GetMeshID(), AnimationID), false);

			if (Runtime.AnimationIDs.Contains(AnimationID))
			{
				Runtime.AnimationIDs.Remove(AnimationID);
			}
			Runtime.AnimationMetaData.RemoveAt(Index);
			Runtime.AnimationMetaData_RenderThread.RemoveAt(Index);

			const int16& NumAnimations = Runtime.AnimationMetaData.Num();
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

	template <typename Function>
	static void ClearAnimations(FCriticalSection& CriticalSection,
	                            FSkinnedMeshRuntime_Lf& Runtime,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            const ETurboSequence_AnimationForceMode_Lf& ForceMode,
	                            const TArray<FTurboSequence_BoneLayer_Lf>& CurrentLayers,
	                            const Function& Condition)
	{
		const int16& NumAnimations = Runtime.AnimationMetaData.Num();
		for (int32 i = NumAnimations - GET1_NUMBER; i >= GET1_NUMBER; --i) // First Anim -> Rest Pose, so we keep it
		{
			if (::Invoke(Condition, Runtime.AnimationMetaData[i]))
			{
				// Remove only animations in the same layer
				if (ForceMode == ETurboSequence_AnimationForceMode_Lf::PerLayer)
				{
					if (ContainsAnyAnimationLayer(Runtime.AnimationMetaData[i].Settings.BoneLayerMasks, CurrentLayers))
					{
						RemoveAnimation(Runtime, CriticalSection, Library, Library_RenderThread, i);
					}
				}
				// Might Implement an Ultra Force Animation mode where we remove all even they are
				// not in the same layer ...
				else
				{
					RemoveAnimation(Runtime, CriticalSection, Library, Library_RenderThread, i);
				}
			}
		}
	}

	static bool RefreshBlendSpaceState(const TObjectPtr<UBlendSpace> BlendSpace, FAnimationBlendSpaceData_Lf& Data,
	                                   const float& DeltaTime, FCriticalSection& CriticalSection)
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

	static FTurboSequence_AnimMinimalBlendSpace_Lf PlayBlendSpace(
		const FSkinnedMeshReference_Lf& Reference, FSkinnedMeshGlobalLibrary_Lf& Library,
		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread, FSkinnedMeshRuntime_Lf& Runtime,
		const TObjectPtr<UBlendSpace> BlendSpace, FCriticalSection& CriticalSection,
		const FTurboSequence_AnimPlaySettings_Lf& AnimSettings, const float& OverrideWeight = INDEX_NONE,
		const float& OverrideStartTime = INDEX_NONE,
		const float& OverrideEndTime = INDEX_NONE)
	{
		FAnimationBlendSpaceData_Lf Data = FAnimationBlendSpaceData_Lf();
		FTurboSequence_AnimMinimalBlendSpace_Lf BlendSpaceData = FTurboSequence_AnimMinimalBlendSpace_Lf(true);
		BlendSpaceData.BlendSpace = BlendSpace;
		BlendSpaceData.BelongsToMeshID = Runtime.GetMeshID();
		BlendSpace->InitializeFilter(&Data.BlendFilter);

		const int16& NumSamples = BlendSpace->GetBlendSamples().Num();

		RefreshBlendSpaceState(BlendSpace, Data, GET0_NUMBER, CriticalSection);

		FTurboSequence_AnimPlaySettings_Lf Settings = AnimSettings;
		Settings.AnimationManagementMode = ETurboSequence_ManagementMode_Lf::SelfManaged;
		Settings.bAnimationTimeSelfManaged = true;

		for (int32 i = GET0_NUMBER; i < NumSamples; ++i)
		{

			const float& PlayLength = BlendSpace->GetBlendSample(i).Animation->GetPlayLength();

			Data.LongestPlayLength = FMath::Max(Data.LongestPlayLength, PlayLength);

			const uint32& AnimID = PlayAnimation(Reference, Library, Library_RenderThread, Runtime, CriticalSection,
			                                     BlendSpace->GetBlendSample(i).Animation, Settings, BlendSpace->bLoop,
			                                     OverrideWeight, OverrideStartTime, OverrideEndTime);

			if (const FBlendSampleData* Sample = Data.CachedBlendSampleData.FindByPredicate(
				[&i](const FBlendSampleData& Other) { return Other.SampleDataIndex == i; }))
			{
				FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[AnimID]];

				AnimationFrame.Settings.AnimationWeight = Sample->GetClampedWeight();

				AnimationFrame.AnimationTime = FMath::Clamp(Sample->Time, 0.0f, Sample->Animation->GetPlayLength());

				TweakAnimation(Runtime, CriticalSection, Library, Library_RenderThread, AnimationFrame.Settings, AnimID,
				               Reference);
			}

			CriticalSection.Lock();
			Data.Points.Add(AnimID, i);
			// Might add more info
			BlendSpaceData.Samples.Add(AnimID);
			CriticalSection.Unlock();
		}

		const int32& NumBlendSpaces = Runtime.AnimationBlendSpaceMetaData.Num();
		TArray<TObjectPtr<UBlendSpace>> BlendSpaceKeys;
		CriticalSection.Lock();
		Runtime.AnimationBlendSpaceMetaData.GetKeys(BlendSpaceKeys);
		CriticalSection.Unlock();
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
				CriticalSection.Lock();
				Runtime.AnimationBlendSpaceMetaData.Remove(BlendSpaceKeys[i]);
				CriticalSection.Unlock();
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

		CriticalSection.Lock();
		Runtime.AnimationBlendSpaceMetaData.Add(BlendSpace, Data);
		CriticalSection.Unlock();

		return BlendSpaceData;
	}

	
	static uint32 PlayAnimation(const FSkinnedMeshReference_Lf& Reference,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            UAnimSequence* Animation,
	                            const FTurboSequence_AnimPlaySettings_Lf& AnimSettings,
	                            const bool bIsLoop, const float& OverrideWeight = INDEX_NONE,
	                            const float& OverrideStartTime = INDEX_NONE,
	                            const float& OverrideEndTime = INDEX_NONE)
	{
		const FUintVector& AnimationHash = GetAnimationLibraryKey(Reference.DataAsset->GetSkeleton(),
		                                                          Reference.DataAsset, Animation);

		const bool bIsRestPose = !IsValid(Animation);

		if (AnimSettings.ForceMode != ETurboSequence_AnimationForceMode_Lf::None)
		{
			ClearAnimations(CriticalSection, Runtime, Library, Library_RenderThread, AnimSettings.ForceMode,
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

		Frame.SetAnimationID(Runtime.AnimationIDs, Runtime.GetMeshID());
		Frame.Settings = AnimSettings;
		
		const uint16& NumAnimationsPreAdd = Runtime.AnimationMetaData.Num();
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


		AddAnimation(Runtime, Reference, Frame, RenderData, CriticalSection, Library);

		return Frame.AnimationID;
	}

	static TObjectPtr<UAnimSequence> GetHighestPriorityAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime)
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

	static void UpdateBlendSpaces(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection,
	                              const float& DeltaTime, FSkinnedMeshGlobalLibrary_Lf& Library,
	                              FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                              const FSkinnedMeshReference_Lf& Reference)
	{
		for (TTuple<TObjectPtr<UBlendSpace>, FAnimationBlendSpaceData_Lf>& BlendSpace : Runtime.
		     AnimationBlendSpaceMetaData)
		{
			FAnimationBlendSpaceData_Lf& Data = BlendSpace.Value;

			if (!RefreshBlendSpaceState(BlendSpace.Key, Data, DeltaTime, CriticalSection))
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

					const float& AnimLength = AnimationFrame.Animation->GetPlayLength();
					Settings.AnimationWeight = Weight;
					//Settings.Settings.AnimationSpeed = AnimLength / Data.LongestPlayLength;
					AnimationFrame.AnimationTime = FMath::Clamp(Time, 0.0f, AnimLength);

					TweakAnimation(Runtime, CriticalSection, Library, Library_RenderThread, Settings,
					               AnimationFrame.AnimationID, Reference);
				}
			}
		}
	}

	static bool TweakBlendSpace(FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
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

	static bool TweakAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                           FCriticalSection& CriticalSection,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const FTurboSequence_AnimPlaySettings_Lf& Settings,
	                           const uint32& AnimationID,
	                           const FSkinnedMeshReference_Lf& Reference)
	{
		if (!Runtime.AnimationIDs.Contains(AnimationID))
		{
			return false;
		}
		FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[AnimationID]];

		if (Settings.ForceMode != ETurboSequence_AnimationForceMode_Lf::None)
		{
			ClearAnimations(CriticalSection, Runtime, Library, Library_RenderThread, Settings.ForceMode,
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

	static void SolveAnimations(FSkinnedMeshRuntime_Lf& Runtime, FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            const FSkinnedMeshReference_Lf& Reference, const float& DeltaTime,
	                            const int64& CurrentFrameCount, FCriticalSection& CriticalSection)
	{
		if (CurrentFrameCount == Runtime.LastFrameAnimationSolved)
		{
			return;
		}
		Runtime.LastFrameAnimationSolved = CurrentFrameCount;


		UpdateBlendSpaces(Runtime, CriticalSection, DeltaTime, Library, Library_RenderThread, Reference);

		for (TTuple<uint32, FAnimationGroup_Lf>& Group : Runtime.AnimationGroups)
		{
			Group.Value.TotalAnimWeightRuntime = GET0_NUMBER;
		}

		const uint16& NumAnimations = Runtime.AnimationMetaData.Num();
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
				const float& Time = FMath::Min(Animation.AnimationRemoveTime, Animation.AnimationWeightTime);
				Animation.AnimationRemoveTime =
					FMath::Clamp(Time + DeltaTime, 0.0f, Animation.AnimationRemoveStartTime);
				AnimationWeight = GET1_NUMBER - FTurboSequence_Helper_Lf::Clamp01(
					FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
						Animation.AnimationRemoveTime, GET0_NUMBER, Animation.AnimationRemoveStartTime));

				if (FMath::IsNearlyEqual(AnimationWeight, GET0_NUMBER, 0.01f))
				{
					RemoveAnimation(Runtime, CriticalSection, Library, Library_RenderThread, AnimIdx);
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
			const int32& GPUPoses = AddAnimationToLibraryChunked(Library, CriticalSection, CPUPoses,
			                                                     Runtime, Animation);

			FAnimationMetaData_RenderThread_Lf& RenderData = Runtime.AnimationMetaData_RenderThread[AnimIdx];

			RenderData.GPUAnimationIndex_0 = GPUPoses;

			Animation.CPUAnimationIndex_0 = CPUPoses;
		}

		const uint16& NumAnimationsPostRemove = Runtime.AnimationMetaData.Num();
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


	// Licence Start
	// Copyright Epic Games, Inc. All Rights Reserved.
	/**
 * Utility function to determine the two key indices to interpolate given a relative position in the animation
 *
 * @param	RelativePos		The relative position to solve in the range [0,1] inclusive.
 * @param	NumKeys			The number of keys present in the track being solved.
 * @param	PosIndex0Out	Output value for the closest key index before the RelativePos specified.
 * @param	PosIndex1Out	Output value for the closest key index after the RelativePos specified.
 * @return	The rate at which to interpolate the two keys returned to obtain the final result.
 */
	static float AnimationCodecTimeToIndex(
		//float SequenceLength,
		float RelativePos,
		int32 NumKeys,
		EAnimInterpolationType Interpolation,
		int32& PosIndex0Out,
		int32& PosIndex1Out)
	{
		float Alpha;

		if (NumKeys < 2)
		{
			checkSlow(NumKeys == 1); // check if data is empty for some reason.
			PosIndex0Out = 0;
			PosIndex1Out = 0;
			return 0.0f;
		}
		// Check for before-first-frame case.
		if (RelativePos <= 0.f)
		{
			PosIndex0Out = 0;
			PosIndex1Out = 0;
			Alpha = 0.0f;
		}
		else
		{
			NumKeys -= 1; // never used without the minus one in this case
			// Check for after-last-frame case.
			if (RelativePos >= 1.0f)
			{
				// If we're not looping, key n-1 is the final key.
				PosIndex0Out = NumKeys;
				PosIndex1Out = NumKeys;
				Alpha = 0.0f;
			}
			else
			{
				// For non-looping animation, the last frame is the ending frame, and has no duration.
				const float KeyPos = RelativePos * static_cast<float>(NumKeys);
				checkSlow(KeyPos >= 0.0f);
				const float KeyPosFloor = floorf(KeyPos);
				PosIndex0Out = FMath::Min(FMath::TruncToInt(KeyPosFloor), NumKeys);
				Alpha = (Interpolation == EAnimInterpolationType::Step) ? 0.0f : KeyPos - KeyPosFloor;
				PosIndex1Out = FMath::Min(PosIndex0Out + 1, NumKeys);
			}
		}
		return Alpha;
	}

	static void AnimationCodecTimeToIndex(
		//float SequenceLength,
		float RelativePos,
		int32 NumKeys,
		EAnimInterpolationType Interpolation,
		int32& PosIndex0Out)
	{
		//float Alpha;

		if (NumKeys < 2)
		{
			checkSlow(NumKeys == 1); // check if data is empty for some reason.
			PosIndex0Out = 0;
			//PosIndex1Out = 0;
			return; // 0.0f;
		}
		// Check for before-first-frame case.
		if (RelativePos <= 0.f)
		{
			PosIndex0Out = 0;
			//PosIndex1Out = 0;
			//Alpha = 0.0f;
		}
		else
		{
			NumKeys -= 1; // never used without the minus one in this case
			// Check for after-last-frame case.
			if (RelativePos >= 1.0f)
			{
				// If we're not looping, key n-1 is the final key.
				PosIndex0Out = NumKeys;
				//PosIndex1Out = NumKeys;
				//Alpha = 0.0f;
			}
			else
			{
				// For non-looping animation, the last frame is the ending frame, and has no duration.
				const float KeyPos = RelativePos * static_cast<float>(NumKeys);
				checkSlow(KeyPos >= 0.0f);
				const float KeyPosFloor = floorf(KeyPos);
				PosIndex0Out = FMath::Min(FMath::TruncToInt(KeyPosFloor), NumKeys);
				//Alpha = (Interpolation == EAnimInterpolationType::Step) ? 0.0f : KeyPos - KeyPosFloor;
				//PosIndex1Out = FMath::Min(PosIndex0Out + 1, NumKeys);
			}
		}
		//return Alpha;
	}

	// Licence End

	static FUintVector GetAnimationLibraryKey(const TObjectPtr<USkeleton> Skeleton,
	                                          const TObjectPtr<UTurboSequence_MeshAsset_Lf>
	                                          Asset, const UAnimSequence* Animation)
	{
		if (!IsValid(Animation))
		{
			return FUintVector(GetTypeHash(Skeleton), GetTypeHash(Asset), GET0_NUMBER);
		}
		return FUintVector(GetTypeHash(Skeleton), GetTypeHash(Asset), GetTypeHash(Animation));
	}

	static bool IsAnimationValid(const TObjectPtr<UAnimSequence> Animation,
	                             const TObjectPtr<USkeletalMesh> Mesh)
	{
		return IsValid(Animation) && Animation->GetSkeleton()->IsCompatibleMesh(Mesh);
	}

	static const FReferenceSkeleton& GetReferenceSkeleton_Raw(
		const TObjectPtr<USkinnedAsset>& Mesh)
	{
		return Mesh->GetRefSkeleton();
	}

	static const FReferenceSkeleton& GetReferenceSkeleton(
		const TObjectPtr<UTurboSequence_MeshAsset_Lf>& Asset, const bool& bIsMeshDataEvaluation = false)
	{
		if (bIsMeshDataEvaluation)
		{
			return GetReferenceSkeleton_Raw(Asset->ReferenceMeshNative);
		}

		return Asset->GetReferenceSkeleton();
	}

	static const TArray<FTransform>& GetSkeletonRefPose(
		const FReferenceSkeleton& ReferenceSkeleton)
	{
		return ReferenceSkeleton.GetRefBonePose();
	}

	static int32 GetSkeletonParentIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                    const int32& Index)
	{
		return ReferenceSkeleton.GetParentIndex(Index);
	}

	static int32 GetSkeletonNumBones(const FReferenceSkeleton& ReferenceSkeleton)
	{
		return ReferenceSkeleton.GetNum();
	}

	static int32 GetSkeletonBoneIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                  const FName& BoneName)
	{
		return ReferenceSkeleton.FindBoneIndex(BoneName);
	}

	static FName GetSkeletonBoneName(const FReferenceSkeleton& ReferenceSkeleton,
	                                 const int32& BoneIndex)
	{
		return ReferenceSkeleton.GetBoneName(BoneIndex);
	}

	static bool GetSkeletonIsValidIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                    const int32& BoneIndex)
	{
		return ReferenceSkeleton.IsValidIndex(BoneIndex);
	}

	static FTurboSequence_TransposeMatrix_Lf GetBoneTransformFromLocalPoses(const int16& BoneIndex,
	                                                                        const FAnimationLibraryData_Lf& LibraryData,
	                                                                        const FReferenceSkeleton& ReferenceSkeleton,
	                                                                        const FReferenceSkeleton& AnimationSkeleton,
	                                                                        const FAnimPose_Lf& Pose,
	                                                                        const TObjectPtr<UAnimSequence> Animation)
	{
		FTurboSequence_TransposeMatrix_Lf BoneSpaceTransform = FTurboSequence_TransposeMatrix_Lf();

		if (!BoneIndex && Animation->bEnableRootMotion)
		{
			switch (Animation->RootMotionRootLock)
			{
			case ERootMotionRootLock::RefPose:
				{
					const FMatrix RefPoseMatrix = GetSkeletonRefPose(ReferenceSkeleton)[GET0_NUMBER].ToMatrixWithScale();

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
			if (const int32& BoneIdx = GetSkeletonBoneIndex(AnimationSkeleton, BoneName); BoneIdx > INDEX_NONE)
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

	static void GetBoneTransformFromAnimationSafe(
		FMatrix& OutAtom, const FAnimationMetaData_Lf& Animation, const uint16& SkeletonBoneIndex,
		const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, const FSkinnedMeshGlobalLibrary_Lf& Library,
		const FReferenceSkeleton& ReferenceSkeleton)
	{
		//OutAtom = FMatrix::Identity;
		if (IsValid(Animation.Animation))
		{
			const FAnimationLibraryData_Lf& LibraryData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

			const FReferenceSkeleton& AnimationSkeleton = Animation.Animation->GetSkeleton()->GetReferenceSkeleton();

			if (LibraryData.AnimPoses.Contains(Animation.CPUAnimationIndex_0))
			{
				const FTurboSequence_TransposeMatrix_Lf& Matrix = GetBoneTransformFromLocalPoses(SkeletonBoneIndex, LibraryData, ReferenceSkeleton,
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


	static FTransform BendBoneFromAnimations(const uint16& BoneIndex,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
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
			// the correct root bone data already baked into the collection ..
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

	static void ExtractRootMotionFromAnimations(FTransform& OutAtom,
	                                            const FSkinnedMeshRuntime_Lf& Runtime,
	                                            const FSkinnedMeshReference_Lf& Reference,
	                                            const float& DeltaTime)
	{
		FMatrix OutputTransform = FMatrix::Identity;
		for (const FAnimationMetaData_Lf& Animation : Runtime.AnimationMetaData)
		{
			if (const ETurboSequence_RootMotionMode_Lf& Mode = Animation.Settings.RootMotionMode;
				IsValid(Animation.Animation) && Animation.Animation->HasRootMotion() && (Mode ==
					ETurboSequence_RootMotionMode_Lf::Force || (Mode ==
						ETurboSequence_RootMotionMode_Lf::OnRootBoneAnimated && Animation.bIsRootBoneAnimation)))
			{
				FTransform RootMotion_Transform = Animation.Animation->ExtractRootMotion(
					Animation.AnimationTime, DeltaTime, Animation.bIsLoop);

				const float& Scalar = Animation.FinalAnimationWeight * Animation.Settings.AnimationSpeed;

				OutputTransform = OutputTransform * (GET1_NUMBER - Scalar) + RootMotion_Transform.ToMatrixWithScale() *
					Scalar;
			}
		}
		OutAtom = FTransform(OutputTransform);
	}


	static void GetIKTransform(FTransform& OutAtom, const uint16& BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const EBoneSpaces::Type& Space, const float& AnimationDeltaTime,
	                           const int64& CurrentFrameCount, FCriticalSection& CriticalSection)
	{
		SolveAnimations(Runtime, Library, Library_RenderThread, Reference, AnimationDeltaTime, CurrentFrameCount,
		                CriticalSection);

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
			OutAtom *= GetWorldSpaceTransformIncludingOffsets(Runtime);
		}
	}

	static bool SetIKTransform(const FTransform& Atom, const uint16& BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FCriticalSection& CriticalSection,
	                           const EBoneSpaces::Type& Space = EBoneSpaces::WorldSpace)
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
			IKTransform *= GetWorldSpaceTransformIncludingOffsets(Runtime).Inverse();
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

	static void ClearIKState(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection)
	{
		TArray<uint16> ToRemove;
		for (TTuple<uint16, FIKBoneData_Lf>& Bone : Runtime.IKData)
		{
			Bone.Value.bIsInUsingWriteDataThisFrame = false;
			Bone.Value.AliveCount++;
			if (Bone.Value.AliveCount > GET2_NUMBER)
			{
				CriticalSection.Lock();
				ToRemove.Add(Bone.Key);
				CriticalSection.Unlock();
			}
		}
		Runtime.bIKDataInUse = false;
		for (const uint16& Remove : ToRemove)
		{
			CriticalSection.Lock();
			Runtime.IKData.Remove(Remove);
			CriticalSection.Unlock();
		}
	}

	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveByAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime, const FAnimationMetaData_Lf& Animation,
		const FSkinnedMeshGlobalLibrary_Lf& Library, const FName& CurveName)
	{
		if (IsValid(Animation.Animation))
		{
			const FAnimationLibraryData_Lf& LibraryData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

			if (LibraryData.AnimPoses.Contains(Animation.CPUAnimationIndex_0))
			{
				const float& PoseCurve0 = FTurboSequence_Helper_Lf::GetAnimationCurveWeight(
					LibraryData.AnimPoses[Animation.CPUAnimationIndex_0].Pose, CurveName);

				return FTurboSequence_PoseCurveData_Lf(Animation.Animation, CurveName, PoseCurve0);
			}
		}

		return FTurboSequence_PoseCurveData_Lf();
	}

	static TArray<FTurboSequence_PoseCurveData_Lf> GetAnimationCurveAtTime(
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

	static void GetSocketTransform(FTransform& OutTransform, const FName& SocketName,
	                               FSkinnedMeshRuntime_Lf& Runtime,
	                               const FSkinnedMeshReference_Lf& Reference,
	                               FSkinnedMeshGlobalLibrary_Lf& Library,
	                               FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                               Library_RenderThread, const EBoneSpaces::Type& Space,
	                               const float& AnimationDeltaTime,
	                               const int64& CurrentFrameCount,
	                               FCriticalSection& CriticalSection)
	{
		const USkeletalMeshSocket* Socket = Runtime.DataAsset->ReferenceMeshNative->FindSocket(SocketName);
		const int32& BoneIndex = GetSkeletonBoneIndex(GetReferenceSkeleton(Runtime.DataAsset), Socket->BoneName);

		GetIKTransform(OutTransform, BoneIndex, Runtime, Reference, Library, Library_RenderThread, Space,
		               AnimationDeltaTime, CurrentFrameCount, CriticalSection);

		OutTransform = FTransform(Socket->RelativeRotation, Socket->RelativeLocation, Socket->RelativeScale) *
			OutTransform;
	}
};
