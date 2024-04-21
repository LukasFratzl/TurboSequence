// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "TurboSequence_Data_Lf.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "TurboSequence_GlobalData_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "Animation/AnimationPoseData.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"

class TURBOSEQUENCE_LF_API FTurboSequence_Utility_Lf
{
public:
	FTurboSequence_Utility_Lf() = delete;
	~FTurboSequence_Utility_Lf() = delete;

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
	                             uint32 MaterialsHash);

	static void UpdateCameras(TArray<FCameraView_Lf>& OutView, const UWorld* InWorld);

	static void UpdateCameras_1(TArray<FCameraView_Lf>& OutViews,
	                            const TMap<uint8, FTransform>& LastFrameCameraTransforms,
	                            const UWorld* InWorld, float DeltaTime);

	static void UpdateCameras_2(TMap<uint8, FTransform>& OutLastFrameCameraTransforms,
	                            const TArray<FCameraView_Lf>& CameraViews);

	static void IsMeshVisible(FSkinnedMeshRuntime_Lf& Runtime,
	                          const FSkinnedMeshReference_Lf& Reference,
	                          const TArray<FCameraView_Lf>& PlayerViews);

	static int32 GetBoneMapIndex_CPU(const TArray<FBoneIndexType>& FromSection,
	                                 int32 RawIndex
	                                 /*, const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset*/);

	static int32 GetBoneMapIndex_GPU(const TMap<uint16, uint16>& FromMap,
	                                 const TArray<FBoneIndexType>& FromSection,
	                                 int32 RawIndex, uint8 Weight);

	static int32 GetValidBoneData(int32 FromData, uint8 CurrentInfluenceIndex,
	                              uint8 MaxInfluenceIndex);

	static void CacheTurboSequenceAssets(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                     FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                     Library_RenderThread, FCriticalSection& CriticalSection);

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
	                                 bool bIsMeshDataEvaluationFunction = false);

	static void CreateMaxLodAndCPUBones(FSkinnedMeshGlobalLibrary_Lf& Library);

	static void CreateGPUBones(FSkinnedMeshReferenceLodElement_Lf& LodElement, FCriticalSection& CriticalSection,
	                           const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, bool bIsMeshDataEvaluation);

	static void CreateBoneMaps(FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           FCriticalSection& CriticalSection);

	static void CreateRawSkinWeightTextureBuffer(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                             const TFunction<void(bool bSuccess)>& PostCall,
	                                             const TObjectPtr<UWorld> World);


	static void CreateInverseReferencePose(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                       FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                       Library_RenderThread,
	                                       FCriticalSection& CriticalSection);

	static void ResizeBuffers(FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                          int32 Num);

	static bool IsValidBufferIndex(const FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                               int32 Index);


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

	/**
		* @brief Updates one instance in an instanced static mesh component
		* @param Reference The Reference to get the instance index
		* @param Runtime The Mesh of the Instance we like to update
		* @param WorldSpaceTransform The transform matrix we like to update
		*/
	static void UpdateInstanceTransform_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FTransform& WorldSpaceTransform);

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
	                              const FTransform& WorldSpaceTransform = FTransform::Identity);

	static void CleanNiagaraRenderer(
		TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>& NiagaraComponents,
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime);


	/**
	 * @brief Removes an instance from the renderer
	 * @param Reference The Reference to remove from the InstanceMap
	 * @param Runtime The Mesh of the Instance you like to remove
	 */
	static void RemoveRenderInstance(FSkinnedMeshReference_Lf& Reference,
	                                 const FSkinnedMeshRuntime_Lf& Runtime,
	                                 FCriticalSection& CriticalSection);

	static void UpdateRenderInstanceLod_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FSkinnedMeshReferenceLodElement_Lf& LodElement, bool bIsVisible);

	static void SetCustomDataForInstance(FSkinnedMeshReference_Lf& Reference,
	                                     int32 CPUIndex, int32 SkinWeightOffset,
	                                     const FSkinnedMeshRuntime_Lf& Runtime,
	                                     const FSkinnedMeshGlobalLibrary_Lf&
	                                     Library);

	static bool SetCustomDataForInstance_User(FSkinnedMeshReference_Lf& Reference,
	                                          const FSkinnedMeshRuntime_Lf& Runtime, int16 CustomDataFractionIndex,
	                                          float CustomDataValue);


	static bool GetIsMeshVisible(const FSkinnedMeshRuntime_Lf& Runtime, const FSkinnedMeshReference_Lf& Reference);


	static void UpdateCullingAndLevelOfDetail(FSkinnedMeshRuntime_Lf& Runtime,
	                                          FSkinnedMeshReference_Lf& Reference,
	                                          const TArray<FCameraView_Lf>& CameraViews,
	                                          TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext,
	                                          bool bVisibleLastFrame,
	                                          FSkinnedMeshGlobalLibrary_Lf& Library);

	static void UpdateDistanceUpdating(FSkinnedMeshRuntime_Lf& Runtime, float DeltaTime);

	static void UpdateRendererBounds(FCriticalSection& Mutex,
	                                 FSkinnedMeshReference_Lf& Reference,
	                                 const FSkinnedMeshRuntime_Lf& Runtime);

	static void UpdateCameraRendererBounds(FRenderData_Lf& RenderData,
	                                       const TArray<FCameraView_Lf>& CameraViews,
	                                       int32 Extend);

	static uint32 GetAnimationLayerGroupHash(const TArray<FTurboSequence_BoneLayer_Lf>& Layers);

	static bool ContainsAnyAnimationLayer(const TArray<FTurboSequence_BoneLayer_Lf>& A,
	                                      const TArray<FTurboSequence_BoneLayer_Lf>& B);

	static bool ContainsRootBoneName(const TArray<FTurboSequence_BoneLayer_Lf>& Collection,
	                                 const FSkinnedMeshRuntime_Lf& Runtime);

	static uint32 GenerateAnimationLayerMask(FAnimationMetaData_Lf& Animation,
	                                         TArray<uint16>& OutLayers,
	                                         FCriticalSection& CriticalSection,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
	                                         const FSkinnedMeshReference_Lf& Reference,
	                                         bool bGenerateLayers);

	static bool MergeAnimationLayerMask(const TArray<uint16>& AnimationLayers, uint32 AnimationLayerHash,
	                                    FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                    bool bIsAdd);

	static bool UpdatedAnimationLayerMaskIndex(FAnimationMetaData_Lf& Animation,
	                                           FSkinnedMeshRuntime_Lf& Runtime,
	                                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                                           const FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                           Library_RenderThread);


	static void UpdateAnimationLayerMasks(FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                      FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread);

	static void AddAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                         const FSkinnedMeshReference_Lf& Reference,
	                         FAnimationMetaData_Lf& Animation,
	                         const FAnimationMetaData_RenderThread_Lf& Animation_RenderThread,
	                         FCriticalSection& CriticalSection,
	                         FSkinnedMeshGlobalLibrary_Lf& Library);


	static void RemoveAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            int32 Index);

	template <typename Function>
	static void ClearAnimations(FCriticalSection& CriticalSection, FSkinnedMeshRuntime_Lf& Runtime,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            ETurboSequence_AnimationForceMode_Lf ForceMode,
	                            const TArray<FTurboSequence_BoneLayer_Lf>& CurrentLayers,
	                            const Function& Condition)
	{
		int16 NumAnimations = Runtime.AnimationMetaData.Num();
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
	                                   float DeltaTime, FCriticalSection& CriticalSection);

	static FTurboSequence_AnimMinimalBlendSpace_Lf PlayBlendSpace(
		const FSkinnedMeshReference_Lf& Reference, FSkinnedMeshGlobalLibrary_Lf& Library,
		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread, FSkinnedMeshRuntime_Lf& Runtime,
		const TObjectPtr<UBlendSpace> BlendSpace, FCriticalSection& CriticalSection,
		const FTurboSequence_AnimPlaySettings_Lf& AnimSettings, float OverrideWeight = INDEX_NONE,
		float OverrideStartTime = INDEX_NONE,
		float OverrideEndTime = INDEX_NONE);


	static uint32 PlayAnimation(const FSkinnedMeshReference_Lf& Reference,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            UAnimSequence* Animation,
	                            const FTurboSequence_AnimPlaySettings_Lf& AnimSettings,
	                            const bool bIsLoop, float OverrideWeight = INDEX_NONE,
	                            float OverrideStartTime = INDEX_NONE,
	                            float OverrideEndTime = INDEX_NONE);

	static TObjectPtr<UAnimSequence> GetHighestPriorityAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime);

	static void UpdateBlendSpaces(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection,
	                              float DeltaTime, FSkinnedMeshGlobalLibrary_Lf& Library,
	                              FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                              const FSkinnedMeshReference_Lf& Reference);

	static bool TweakBlendSpace(FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpace,
	                            const FVector3f& WantedPosition);

	static bool TweakAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                           FCriticalSection& CriticalSection,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const FTurboSequence_AnimPlaySettings_Lf& Settings,
	                           uint32 AnimationID,
	                           const FSkinnedMeshReference_Lf& Reference);

	static void SolveAnimations(FSkinnedMeshRuntime_Lf& Runtime, FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            const FSkinnedMeshReference_Lf& Reference, float DeltaTime,
	                            int32 CurrentFrameCount, FCriticalSection& CriticalSection);

	static FORCEINLINE FUintVector GetAnimationLibraryKey(const TObjectPtr<USkeleton> Skeleton,
	                                                      const TObjectPtr<UTurboSequence_MeshAsset_Lf>
	                                                      Asset, const UAnimSequence* Animation)
	{
		if (!IsValid(Animation))
		{
			return FUintVector(GetTypeHash(Skeleton), GetTypeHash(Asset), GET0_NUMBER);
		}
		return FUintVector(GetTypeHash(Skeleton), GetTypeHash(Asset), GetTypeHash(Animation));
	}

	static FORCEINLINE const FReferenceSkeleton& GetReferenceSkeleton_Raw(
		const TObjectPtr<USkinnedAsset>& Mesh)
	{
		return Mesh->GetRefSkeleton();
	}

	static FORCEINLINE const FReferenceSkeleton& GetReferenceSkeleton(
		const TObjectPtr<UTurboSequence_MeshAsset_Lf>& Asset, bool bIsMeshDataEvaluation = false)
	{
		if (bIsMeshDataEvaluation)
		{
			return GetReferenceSkeleton_Raw(Asset->ReferenceMeshNative);
		}

		return Asset->GetReferenceSkeleton();
	}

	static FORCEINLINE const TArray<FTransform>& GetSkeletonRefPose(
		const FReferenceSkeleton& ReferenceSkeleton)
	{
		return ReferenceSkeleton.GetRefBonePose();
	}

	static FORCEINLINE int32 GetSkeletonParentIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                                int32 Index)
	{
		return ReferenceSkeleton.GetParentIndex(Index);
	}

	static FORCEINLINE int32 GetSkeletonNumBones(const FReferenceSkeleton& ReferenceSkeleton)
	{
		return ReferenceSkeleton.GetNum();
	}

	static FORCEINLINE int32 GetSkeletonBoneIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                              const FName& BoneName)
	{
		return ReferenceSkeleton.FindBoneIndex(BoneName);
	}

	static FORCEINLINE FName GetSkeletonBoneName(const FReferenceSkeleton& ReferenceSkeleton,
	                                             int32 BoneIndex)
	{
		return ReferenceSkeleton.GetBoneName(BoneIndex);
	}

	static FORCEINLINE bool GetSkeletonIsValidIndex(const FReferenceSkeleton& ReferenceSkeleton,
	                                                int32 BoneIndex)
	{
		return ReferenceSkeleton.IsValidIndex(BoneIndex);
	}

	static FTurboSequence_TransposeMatrix_Lf GetBoneTransformFromLocalPoses(int16 BoneIndex,
	                                                                        const FAnimationLibraryData_Lf& LibraryData,
	                                                                        const FReferenceSkeleton& ReferenceSkeleton,
	                                                                        const FReferenceSkeleton& AnimationSkeleton,
	                                                                        const FAnimPose_Lf& Pose,
	                                                                        const TObjectPtr<UAnimSequence> Animation);

	static void GetBoneTransformFromAnimationSafe(
		FMatrix& OutAtom, const FAnimationMetaData_Lf& Animation, uint16 SkeletonBoneIndex,
		const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, const FSkinnedMeshGlobalLibrary_Lf& Library,
		const FReferenceSkeleton& ReferenceSkeleton);


	static FTransform BendBoneFromAnimations(uint16 BoneIndex,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
	                                         const FSkinnedMeshReference_Lf& Reference,
	                                         const FSkinnedMeshGlobalLibrary_Lf& Library);

	static void ExtractRootMotionFromAnimations(FTransform& OutAtom,
	                                            const FSkinnedMeshRuntime_Lf& Runtime,
	                                            const FSkinnedMeshReference_Lf& Reference,
	                                            float DeltaTime);


	static void GetIKTransform(FTransform& OutAtom, uint16 BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const EBoneSpaces::Type Space, float AnimationDeltaTime,
	                           int32 CurrentFrameCount, FCriticalSection& CriticalSection);

	static bool SetIKTransform(const FTransform& Atom, uint16 BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FCriticalSection& CriticalSection,
	                           const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	static void ClearIKState(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection);

	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveByAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime, const FAnimationMetaData_Lf& Animation,
		const FSkinnedMeshGlobalLibrary_Lf& Library, const FName& CurveName);

	static TArray<FTurboSequence_PoseCurveData_Lf> GetAnimationCurveAtTime(
		const FSkinnedMeshRuntime_Lf& Runtime, const FName& CurveName, const FSkinnedMeshGlobalLibrary_Lf& Library,
		FCriticalSection& CriticalSection);

	static void GetSocketTransform(FTransform& OutTransform, const FName& SocketName,
	                               FSkinnedMeshRuntime_Lf& Runtime,
	                               const FSkinnedMeshReference_Lf& Reference,
	                               FSkinnedMeshGlobalLibrary_Lf& Library,
	                               FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                               Library_RenderThread, const EBoneSpaces::Type Space,
	                               float AnimationDeltaTime,
	                               int32 CurrentFrameCount,
	                               FCriticalSection& CriticalSection);
};
