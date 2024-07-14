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

	/**
	 * Creates a renderer for the given skinned mesh reference.
	 *
	 * @param Reference The skinned mesh reference to create the renderer for.
	 * @param GlobalData The global data for the renderer.
	 * @param RenderReference The Niagara system to use for rendering.
	 * @param LevelOfDetails The map of level of details for the renderer.
	 * @param InstanceSceneComponent The scene component to attach the renderer to.
	 * @param RenderComponents The map of render components for the renderer.
	 * @param FromAsset The mesh asset to create the renderer from.
	 * @param Materials The array of materials to use for the renderer.
	 * @param MaterialsHash The hash of the materials.
	 *
	 * @return The hash of the materials.
	 *
	 * @throws None
	 */
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
	/**
	 * Updates the cameras in the given array with the views from the world.
	 *
	 * @param OutView The array to store the updated camera views.
	 * @param InWorld The world object to retrieve the camera views from.
	 */
	static void UpdateCameras(TArray<FCameraView_Lf>& OutView, const UWorld* InWorld);

	/**
 * Updates the cameras in the scene.
 *
 * @param OutViews The array to store the updated camera views.
 * @param LastFrameCameraTransforms The map of the last frame's camera transforms.
 * @param InWorld The world in which the cameras are located.
 * @param DeltaTime The time elapsed since the last frame.
 *
 * @throws None
 */
	static void UpdateCameras_1(TArray<FCameraView_Lf>& OutViews,
								const TMap<uint8, FTransform>& LastFrameCameraTransforms,
								const UWorld* InWorld, float DeltaTime);

	/**
 * Updates the last frame camera transforms based on the provided camera views.
 *
 * @param OutLastFrameCameraTransforms The map to store the last frame camera transforms.
 * @param CameraViews The array of camera views to update the transforms from.
 *
 * @throws None
 */
	static void UpdateCameras_2(TMap<uint8, FTransform>& OutLastFrameCameraTransforms,
								const TArray<FCameraView_Lf>& CameraViews);

	/**
 * Determines if a skinned mesh is visible to any of the player cameras.
 *
 * @param Runtime The runtime data of the skinned mesh.
 * @param Reference The reference data of the skinned mesh.
 * @param PlayerViews An array of camera views representing the player's perspective.
 *
 * @return void
 *
 * @throws None
 */
	static FORCEINLINE_DEBUGGABLE void IsMeshVisible(FSkinnedMeshRuntime_Lf& Runtime,
							  const FSkinnedMeshReference_Lf& Reference,
							  const TArray<FCameraView_Lf>& PlayerViews)
{
	const FVector& MeshLocation = Runtime.WorldSpaceTransform.GetLocation();
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

	Runtime.bIsVisibleInFrustum = bIsVisibleOnAnyCamera;
	Runtime.bIsVisible = bIsVisibleOnAnyCamera;
}
	/**
	 * Retrieves the bone map index for a given raw index from the specified section.
	 *
	 * @param FromSection The array of bone indices representing the section.
	 * @param RawIndex The raw index for which to retrieve the bone map index.
	 *
	 * @return The bone map index corresponding to the raw index, or INDEX_NONE if the index is out of range.
	 *
	 * @throws None
	 */
	static int32 GetBoneMapIndex_CPU(const TArray<FBoneIndexType>& FromSection,
	                                 int32 RawIndex
	                                 /*, const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset*/);
	/**
	 * Retrieves the bone map index for a given raw index from the specified section using the GPU.
	 *
	 * @param FromMap The map of bone indices representing the section.
	 * @param FromSection The array of bone indices representing the section.
	 * @param RawIndex The raw index for which to retrieve the bone map index.
	 * @param Weight The weight to consider when retrieving the bone map index.
	 *
	 * @return The bone map index corresponding to the raw index, or INDEX_NONE if the index is out of range.
	 *
	 * @throws None
	 */
	static int32 GetBoneMapIndex_GPU(const TMap<uint16, uint16>& FromMap,
	                                 const TArray<FBoneIndexType>& FromSection,
	                                 int32 RawIndex, uint8 Weight);
	/**
	 * Retrieves valid bone data based on influence indices.
	 *
	 * @param FromData The original bone data to validate.
	 * @param CurrentInfluenceIndex The current influence index.
	 * @param MaxInfluenceIndex The maximum influence index allowed.
	 *
	 * @return The valid bone data based on the influence indices.
	 *
	 * @throws None
	 */
	static int32 GetValidBoneData(int32 FromData, uint8 CurrentInfluenceIndex,
	                              uint8 MaxInfluenceIndex);
	/**
	 * Caches TurboSequence assets in the given libraries and critical section.
	 *
	 * @param Library The global library to cache the assets in.
	 * @param Library_RenderThread The render thread library to cache the assets in.
	 * @param CriticalSection The critical section to use for thread safety.
	 *
	 * @throws None
	 */
	static void CacheTurboSequenceAssets(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                     FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                     Library_RenderThread, FCriticalSection& CriticalSection);
	/**
	 * Creates asynchronous chunked mesh data based on the provided mesh asset, global data, and libraries.
	 *
	 * @param FromAsset The mesh asset used to create the chunked mesh data.
	 * @param GlobalData The global data used to create the chunked mesh data.
	 * @param Library The global library used to create the chunked mesh data.
	 * @param Library_RenderThread The render thread library used to create the chunked mesh data.
	 * @param CriticalSection The critical section used for thread safety.
	 *
	 * @throws None
	 */
	static void CreateAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                       const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
	                                       FSkinnedMeshGlobalLibrary_Lf& Library,
	                                       FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                       FCriticalSection& CriticalSection);
	/**
	 * Refreshes asynchronous chunked mesh data based on the provided global data and libraries.
	 *
	 * @param GlobalData The global data used to refresh the chunked mesh data.
	 * @param Library The global library containing the mesh data to refresh.
	 * @param Library_RenderThread The render thread library containing the mesh data to refresh.
	 * @param CriticalSection The critical section used for thread safety.
	 *
	 * @throws None
	 */
	static void RefreshAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
	                                        FSkinnedMeshGlobalLibrary_Lf& Library,
	                                        FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                        FCriticalSection& CriticalSection);
	/**
	 * Creates a reference to a TurboSequence asset in the global library and render thread library.
	 *
	 * @param Library The global library to store the reference data.
	 * @param Library_RenderThread The render thread library to store the render thread reference data.
	 * @param CriticalSection The critical section for thread safety.
	 * @param FromAsset The TurboSequence asset from which to create the reference.
	 *
	 * @return None
	 *
	 * @throws None
	 */
	static void CreateTurboSequenceReference(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                         FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                                         FCriticalSection& CriticalSection,
	                                         const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset);
	/**
	 * Creates level of details for a skinned mesh reference.
	 *
	 * @param Reference The skinned mesh reference to create level of details for.
	 * @param Reference_RenderThread The render thread reference data for the skinned mesh reference.
	 * @param CriticalSection The critical section for thread safety.
	 * @param FromAsset The TurboSequence mesh asset to create level of details from.
	 * @param bIsMeshDataEvaluationFunction Flag indicating if the function is used for mesh data evaluation. Default is false.
	 *
	 * @throws None
	 */
	static void CreateLevelOfDetails(FSkinnedMeshReference_Lf& Reference,
	                                 FSkinnedMeshReference_RenderThread_Lf&
	                                 Reference_RenderThread, FCriticalSection& CriticalSection,
	                                 const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                 bool bIsMeshDataEvaluationFunction = false);
	/**
	 * Creates maximum level of details and CPU bones for the provided global library.
	 *
	 * @param Library The global library to create max LOD and CPU bones for.
	 *
	 * @return None
	 *
	 * @throws None
	 */
	static void CreateMaxLodAndCPUBones(FSkinnedMeshGlobalLibrary_Lf& Library);
	/**
	 * Creates GPU bones for a skinned mesh lod element.
	 *
	 * @param LodElement The skinned mesh reference lod element.
	 * @param CriticalSection The critical section for thread safety.
	 * @param Asset The TurboSequence mesh asset for creating GPU bones.
	 * @param bIsMeshDataEvaluation Flag indicating if the function is used for mesh data evaluation.
	 *
	 * @return None
	 *
	 * @throws None
	 */
	static void CreateGPUBones(FSkinnedMeshReferenceLodElement_Lf& LodElement, FCriticalSection& CriticalSection,
	                           const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, bool bIsMeshDataEvaluation);
	/**
	 * Creates bone maps for the given skinned mesh global library and render thread library.
	 *
	 * @param Library The skinned mesh global library to create bone maps for.
	 * @param Library_RenderThread The skinned mesh render thread library to create bone maps for.
	 * @param CriticalSection The critical section to use for thread safety.
	 *
	 * @throws None
	 */
	static void CreateBoneMaps(FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           FCriticalSection& CriticalSection);
	/**
	 * Creates a raw skin weight texture buffer for a given mesh asset in the specified world.
	 *
	 * @param FromAsset The mesh asset to create the texture buffer from.
	 * @param PostCall A function to call after the texture buffer creation is complete. The function takes a boolean parameter indicating the success of the operation.
	 * @param World The world in which the texture buffer will be created.
	 *
	 * @throws None
	 */
	static void CreateRawSkinWeightTextureBuffer(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                             const TFunction<void(bool bSuccess)>& PostCall,
	                                             const TObjectPtr<UWorld> World);

	/**
	 * Creates the inverse reference pose for a given skinned mesh global library and render thread library.
	 *
	 * @param Library The skinned mesh global library to create the inverse reference pose for.
	 * @param Library_RenderThread The skinned mesh render thread library to create the inverse reference pose for.
	 * @param CriticalSection The critical section to use for thread safety.
	 *
	 * @throws None
	 */
	static void CreateInverseReferencePose(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                       FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                       Library_RenderThread,
	                                       FCriticalSection& CriticalSection);
	/**
	 * Resizes the buffers in the given SkinnedMeshGlobalLibrary_RenderThread_Lf object.
	 *
	 * @param Library The SkinnedMeshGlobalLibrary_RenderThread_Lf object to resize the buffers in.
	 * @param Num The new size of the buffers.
	 *
	 * @throws None
	 */
	static void ResizeBuffers(FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                          int32 Num);
	/**
	 * Checks if the given buffer index is valid in the provided skinned mesh global library.
	 *
	 * @param Library The skinned mesh global library to check the buffer index against.
	 * @param Index The buffer index to check for validity.
	 *
	 * @return True if the buffer index is valid, false otherwise.
	 *
	 * @throws None
	 */
	static bool IsValidBufferIndex(const FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library,
	                               int32 Index);

	/**
	 * Adds an animation to the chunked library with multi-threading support.
	 *
	 * @param Library The skinned mesh global library to add the animation.
	 * @param CriticalSection The critical section for multi-threading.
	 * @param CPUIndices The indices for the CPU.
	 * @param Runtime The skinned mesh runtime data for the animation.
	 * @param Animation The metadata of the animation to add.
	 *
	 * @return The index of the added animation.
	 *
	 * @throws None
	 */
	static int32 AddAnimationToLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
	                                          FCriticalSection& CriticalSection, int32& CPUIndices,
	                                          const FSkinnedMeshRuntime_Lf& Runtime,
	                                          const FAnimationMetaData_Lf& Animation);
	/**
 * Customizes a skinned mesh by applying a target mesh, target materials, and other properties.
 *
 * @param Runtime The runtime data of the skinned mesh.
 * @param TargetMesh The target mesh to apply to the skinned mesh.
 * @param TargetMaterials The target materials to apply to the skinned mesh.
 * @param NiagaraComponents A map of niagara components associated with the skinned mesh.
 * @param Library The global library of skinned meshes.
 * @param Library_RenderThread The render thread library of skinned meshes.
 * @param RootComponent The root component of the skinned mesh.
 * @param ThreadContext The thread context of the skinned mesh.
 *
 * @throws None
 */
	static void CustomizeMesh(FSkinnedMeshRuntime_Lf& Runtime, const TObjectPtr<UTurboSequence_MeshAsset_Lf> TargetMesh,
	                          const TArray<TObjectPtr<UMaterialInterface>>& TargetMaterials,
	                          TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>&
	                          NiagaraComponents, FSkinnedMeshGlobalLibrary_Lf
	                          & Library,
	                          FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                          const TObjectPtr<USceneComponent> RootComponent,
	                          const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext);

	/**
     * Updates the instance transform of a skinned mesh reference based on the provided player views.
     *
     * @param Reference The skinned mesh reference to update.
     * @param Runtime The runtime data of the skinned mesh.
     * @param PlayerViews An array of camera views representing the player's perspective.
     *
     * @throws None
     */
	static void UpdateInstanceTransform_Internal(FSkinnedMeshReference_Lf& Reference,
	                                             const FSkinnedMeshRuntime_Lf& Runtime,
	                                             const TArray<FCameraView_Lf>& PlayerViews);

	/**
		* @brief Updates one instance in an instanced static mesh component
		* @param Reference The Reference to get the instance index
		* @param Runtime The Mesh of the Instance we like to update
		* @param WorldSpaceTransform The transform matrix we like to update
		*/
	static void UpdateInstanceTransform_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FTransform& WorldSpaceTransform, const bool bForce);

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
	/**
	 * Cleans the Niagara renderer for a given skinned mesh reference and runtime.
	 *
	 * @param NiagaraComponents The map of Niagara components.
	 * @param Reference The skinned mesh reference.
	 * @param Runtime The skinned mesh runtime.
	 *
	 * @throws None.
	 */
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
	/**
	 * Updates the level of detail for a render instance concurrently.
	 *
	 * @param Reference The skinned mesh reference.
	 * @param Runtime The skinned mesh runtime.
	 * @param LodElement The LOD element for the render instance.
	 * @param bIsVisible Flag indicating if the instance is visible.
	 *
	 * @return void
	 *
	 * @throws None
	 */
	static void UpdateRenderInstanceLod_Concurrent(
		FSkinnedMeshReference_Lf& Reference, const FSkinnedMeshRuntime_Lf& Runtime,
		const FSkinnedMeshReferenceLodElement_Lf& LodElement, bool bIsVisible);
	/**
	 * Updates the custom data for a specific instance in the skinned mesh reference.
	 *
	 * @param Reference The skinned mesh reference to update custom data for.
	 * @param CPUIndex The CPU index for the instance.
	 * @param SkinWeightOffset The skin weight offset of the instance.
	 * @param Runtime The skinned mesh runtime data for the instance.
	 * @param Library The global library for skinned mesh data.
	 *
	 * @return void
	 *
	 * @throws None
	 */
	static void SetCustomDataForInstance(FSkinnedMeshReference_Lf& Reference,
	                                     int32 CPUIndex, int32 SkinWeightOffset,
	                                     const FSkinnedMeshRuntime_Lf& Runtime,
	                                     const FSkinnedMeshGlobalLibrary_Lf&
	                                     Library);
	/**
	 * Updates the custom data for a specific instance in the skinned mesh reference.
	 *
	 * @param Reference The skinned mesh reference to update custom data for.
	 * @param Runtime The skinned mesh runtime data for the instance.
	 * @param CustomDataFractionIndex The index for custom data.
	 * @param CustomDataValue The value to set for the custom data.
	 *
	 * @return true if the custom data was successfully set, false otherwise.
	 *
	 * @throws None
	 */
	static bool SetCustomDataForInstance_User(FSkinnedMeshReference_Lf& Reference,
	                                          const FSkinnedMeshRuntime_Lf& Runtime, int16 CustomDataFractionIndex,
	                                          float CustomDataValue);

	/**
	 * Determines if a skinned mesh is visible to any of the player cameras.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param Reference The reference data of the skinned mesh.
	 *
	 * @return true if the skinned mesh is visible, false otherwise.
	 *
	 * @throws None
	 */
	static FORCEINLINE_DEBUGGABLE bool GetIsMeshVisible(const FSkinnedMeshRuntime_Lf& Runtime,
												 const FSkinnedMeshReference_Lf& Reference, const bool bPureFrustumCheck = false)
	{
		if (bPureFrustumCheck)
		{
			return Runtime.bIsVisibleInFrustum;
		}

		if (Reference.LevelOfDetails.Contains(Runtime.LodIndex))
		{
			const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];

			switch (Runtime.EIsVisibleOverride)
			{
			case ETurboSequence_IsVisibleOverride_Lf::Default:
				return Runtime.bIsVisible || !LodElement.bIsFrustumCullingEnabled;
			case ETurboSequence_IsVisibleOverride_Lf::IsVisible:
				return true;
			case ETurboSequence_IsVisibleOverride_Lf::IsNotVisible:
				return false;
			case ETurboSequence_IsVisibleOverride_Lf::ScaleToZero:
				return true;
			}

			//return Runtime.bIsVisible || !LodElement.bIsFrustumCullingEnabled;
		}
		switch (Runtime.EIsVisibleOverride)
		{
		case ETurboSequence_IsVisibleOverride_Lf::Default:
			return Runtime.bIsVisible;
		case ETurboSequence_IsVisibleOverride_Lf::IsVisible:
			return true;
		case ETurboSequence_IsVisibleOverride_Lf::IsNotVisible:
			return false;
		case ETurboSequence_IsVisibleOverride_Lf::ScaleToZero:
			return true;
		}
		return Runtime.bIsVisible;
	}
	/**
 * Determines if a skinned mesh is animated.
 *
 * @param Runtime The runtime data of the skinned mesh.
 * @param Reference The reference data of the skinned mesh.
 *
 * @return true if the skinned mesh is animated, false otherwise.
 *
 * @throws None
 */
	static FORCEINLINE_DEBUGGABLE bool GetIsMeshAnimated(const FSkinnedMeshRuntime_Lf& Runtime,
												  const FSkinnedMeshReference_Lf& Reference)
	{
		if (Reference.LevelOfDetails.Contains(Runtime.LodIndex))
		{
			const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];

			switch (Runtime.EIsAnimatedOverride)
			{
			case ETurboSequence_IsAnimatedOverride_Lf::Default:
				return LodElement.bIsAnimated;
			case ETurboSequence_IsAnimatedOverride_Lf::IsAnimated:
				return true;
			case ETurboSequence_IsAnimatedOverride_Lf::IsNotAnimated:
				return false;
			}
		}
		return false;
	}

	/**
	 * Updates the culling and level of detail for a skinned mesh based on the provided camera views.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param Reference The reference data of the skinned mesh.
	 * @param CameraViews An array of camera views representing the player's perspective.
	 * @param ThreadContext The thread context of the skinned mesh.
	 * @param Library The global library of skinned meshes.
	 *
	 * @throws None
	 */
	static void UpdateCullingAndLevelOfDetail(FSkinnedMeshRuntime_Lf& Runtime,
	                                          FSkinnedMeshReference_Lf& Reference,
	                                          const TArray<FCameraView_Lf>& CameraViews,
	                                          TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext,
	                                          FSkinnedMeshGlobalLibrary_Lf& Library);
	/**
	 * Updates the distance updating state of the skinned mesh runtime based on the provided delta time.
	 *
	 * @param Runtime The skinned mesh runtime to update.
	 * @param DeltaTime The time elapsed since the last update.
	 *
	 * @throws None
	 */
	static FORCEINLINE_DEBUGGABLE void UpdateDistanceUpdating(FSkinnedMeshRuntime_Lf& Runtime, float DeltaTime)
{
	{
		Runtime.bIsDistanceUpdatingThisFrame = false;
		if (Runtime.DataAsset->bUseDistanceUpdating)
		{
			float DistanceRatioSeconds = Runtime.ClosestCameraDistance / 250000 * Runtime.DataAsset->
				DistanceUpdatingRatio;
			Runtime.DeltaTimeAccumulator -= DeltaTime;
			if (Runtime.DeltaTimeAccumulator < DistanceRatioSeconds)
			{
				Runtime.DeltaTimeAccumulator = DistanceRatioSeconds;
				Runtime.bIsDistanceUpdatingThisFrame = true;
			}
		}
	}
}
	/**
	 * Updates the renderer bounds of a skinned mesh based on the provided reference and runtime.
	 *
	 * @param Mutex The critical section mutex for thread safety.
	 * @param Reference The reference data of the skinned mesh.
	 * @param Runtime The runtime data of the skinned mesh.
	 *
	 * @throws None
	 */
	static void UpdateRendererBounds(FCriticalSection& Mutex,
	                                 FSkinnedMeshReference_Lf& Reference,
	                                 const FSkinnedMeshRuntime_Lf& Runtime);
	/**
	 * Updates the renderer bounds of a camera based on the provided camera views and extend.
	 *
	 * @param RenderData The render data of the camera.
	 * @param CameraViews The array of camera views to update the bounds from.
	 * @param Extend The extend value to use for updating the bounds.
	 *
	 * @throws None
	 */
	static void UpdateCameraRendererBounds(FRenderData_Lf& RenderData,
	                                       const TArray<FCameraView_Lf>& CameraViews,
	                                       int32 Extend);
	/**
	 * Retrieves the hash value representing the group of animation layers.
	 *
	 * @param Layers The array of bone layers used to calculate the hash.
	 *
	 * @return The hash value representing the group of animation layers.
	 *
	 * @throws None
	 */
	static uint32 GetAnimationLayerGroupHash(const TArray<FTurboSequence_BoneLayer_Lf>& Layers);
	/**
	 * Checks if any animation layer from array A is present in array B.
	 *
	 * @param A The first array of FTurboSequence_BoneLayer_Lf objects.
	 * @param B The second array of FTurboSequence_BoneLayer_Lf objects.
	 *
	 * @return True if any animation layer from array A is present in array B, false otherwise.
	 */
	static bool ContainsAnyAnimationLayer(const TArray<FTurboSequence_BoneLayer_Lf>& A,
	                                      const TArray<FTurboSequence_BoneLayer_Lf>& B);
	/**
	 * Checks if the root bone name is present in the given bone layer collection.
	 *
	 * @param Collection The array of bone layers to search for the root bone name.
	 * @param Runtime The runtime data of the skinned mesh.
	 *
	 * @return True if the root bone name is present, false otherwise.
	 *
	 * @throws None
	 */
	static bool ContainsRootBoneName(const TArray<FTurboSequence_BoneLayer_Lf>& Collection,
	                                 const FSkinnedMeshRuntime_Lf& Runtime);
	/**
	 * Generates an animation layer mask based on the given animation metadata.
	 *
	 * @param Animation The animation metadata used to generate the layer mask.
	 * @param OutLayers The array to store the generated layer mask.
	 * @param CriticalSection The critical section for thread safety.
	 * @param Runtime The skinned mesh runtime data.
	 * @param Reference The skinned mesh reference data.
	 * @param bGenerateLayers Flag indicating whether to generate the layers.
	 *
	 * @return The generated animation layer mask.
	 *
	 * @throws None
	 */
	static uint32 GenerateAnimationLayerMask(FAnimationMetaData_Lf& Animation,
	                                         TArray<uint16>& OutLayers,
	                                         FCriticalSection& CriticalSection,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
	                                         const FSkinnedMeshReference_Lf& Reference,
	                                         bool bGenerateLayers);
	/**
	 * Merges animation layers into the global library with thread safety.
	 *
	 * @param AnimationLayers The array of animation layers to merge.
	 * @param AnimationLayerHash The hash value of the animation layer.
	 * @param CriticalSection The critical section for thread safety.
	 * @param Library The global library to merge into.
	 * @param bIsAdd Flag indicating whether to add the layers.
	 *
	 * @return True if the merge operation was successful, false otherwise.
	 *
	 * @throws None
	 */
	static bool MergeAnimationLayerMask(const TArray<uint16>& AnimationLayers, uint32 AnimationLayerHash,
	                                    FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                    bool bIsAdd);
	/**
	 * Updates the animation layer mask index for the given animation in the skinned mesh runtime.
	 *
	 * @param Animation The animation metadata.
	 * @param Runtime The skinned mesh runtime data.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 *
	 * @return True if the animation layer mask index was successfully updated, false otherwise.
	 *
	 * @throws None
	 */
	static bool UpdatedAnimationLayerMaskIndex(FAnimationMetaData_Lf& Animation,
	                                           FSkinnedMeshRuntime_Lf& Runtime,
	                                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                                           const FSkinnedMeshGlobalLibrary_RenderThread_Lf&
	                                           Library_RenderThread);

	/**
	 * Updates the animation layer mask index for the given animation in the skinned mesh runtime.
	 *
	 * @param Animation The animation metadata.
	 * @param Runtime The skinned mesh runtime data.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 *
	 * @return True if the animation layer mask index was successfully updated, false otherwise.
	 *
	 * @throws None
	 */
	static void UpdateAnimationLayerMasks(FCriticalSection& CriticalSection, FSkinnedMeshGlobalLibrary_Lf& Library,
	                                      FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread);
	/**
	 * Adds an animation to the given skinned mesh runtime.
	 *
	 * @param Runtime The skinned mesh runtime to add the animation to.
	 * @param Reference The reference to the skinned mesh.
	 * @param Animation The metadata of the animation to add.
	 * @param Animation_RenderThread The render thread metadata of the animation.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param Library The global library of skinned meshes.
	 *
	 * @throws None
	 */
	static void AddAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                         const FSkinnedMeshReference_Lf& Reference,
	                         FAnimationMetaData_Lf& Animation,
	                         const FAnimationMetaData_RenderThread_Lf& Animation_RenderThread,
	                         const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	                         FSkinnedMeshGlobalLibrary_Lf& Library);

	/**
	 * Removes an animation from the given skinned mesh runtime.
	 *
	 * @param Runtime The skinned mesh runtime from which to remove the animation.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param Index The index of the animation to remove.
	 *
	 * @throws None
	 */
	static void RemoveAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread, int32 Index);
	/**
	 * Clears animations from the given skinned mesh runtime based on a condition.
	 *
	 * @tparam Function The type of the condition function.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param Runtime The skinned mesh runtime to clear animations from.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param ForceMode The force mode for removing animations.
	 * @param CurrentLayers The current bone layers.
	 * @param Condition The condition function to determine which animations to clear.
	 *
	 * @throws None
	 */
	template <typename Function>
	static void ClearAnimations(const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	                            FSkinnedMeshRuntime_Lf& Runtime,
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
						RemoveAnimation(Runtime, ThreadContext, Library, Library_RenderThread, i);
					}
				}
				// Might Implement an Ultra Force Animation mode where we remove all even they are
				// not in the same layer ...
				else
				{
					RemoveAnimation(Runtime, ThreadContext, Library, Library_RenderThread, i);
				}
			}
		}
	}

	/**
	 * Refreshes the state of the BlendSpace by ticking the BlendSpace and updating the Data.
	 *
	 * @param BlendSpace The BlendSpace to refresh.
	 * @param Data The animation blend space data to update.
	 * @param DeltaTime The time elapsed since the last update.
	 * @param CriticalSection The critical section to lock during the update.
	 *
	 * @return True if the BlendSpace was successfully refreshed, false otherwise.
	 *
	 * @throws None
	 */
	static bool RefreshBlendSpaceState(const TObjectPtr<UBlendSpace> BlendSpace, FAnimationBlendSpaceData_Lf& Data,
	                                   float DeltaTime, FCriticalSection& CriticalSection);
	/**
	 * Plays a BlendSpace animation for a given skinned mesh.
	 *
	 * @param Reference The reference to the skinned mesh.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param BlendSpace The BlendSpace animation to play.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param AnimSettings The settings for the animation playback.
	 * @param OverrideWeight The optional override weight for the animation.
	 * @param OverrideStartTime The optional override start time for the animation.
	 * @param OverrideEndTime The optional override end time for the animation.
	 *
	 * @return The minimal blend space animation data.
	 *
	 * @throws None
	 */
	static FTurboSequence_AnimMinimalBlendSpace_Lf PlayBlendSpace(
		const FSkinnedMeshReference_Lf& Reference, FSkinnedMeshGlobalLibrary_Lf& Library,
		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread, FSkinnedMeshRuntime_Lf& Runtime,
		const TObjectPtr<UBlendSpace> BlendSpace,
		const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext, const FTurboSequence_AnimPlaySettings_Lf&
		AnimSettings,
		float OverrideWeight = INDEX_NONE,
		float OverrideStartTime = INDEX_NONE, float OverrideEndTime = INDEX_NONE);

	/**
	 * Plays an animation for a skinned mesh with specified settings.
	 *
	 * @param Reference The reference to the skinned mesh.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param Animation The animation to play.
	 * @param AnimSettings The settings for the animation playback.
	 * @param bIsLoop Flag indicating if the animation should loop.
	 * @param OverrideWeight The optional override weight for the animation.
	 * @param OverrideStartTime The optional override start time for the animation.
	 * @param OverrideEndTime The optional override end time for the animation.
	 *
	 * @return The ID of the played animation.
	 *
	 * @throws None
	 */
	static uint32 PlayAnimation(const FSkinnedMeshReference_Lf& Reference,
	                            FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            FSkinnedMeshRuntime_Lf& Runtime,
	                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	                            UAnimSequence* Animation,
	                            const FTurboSequence_AnimPlaySettings_Lf& AnimSettings, const bool bIsLoop,
	                            float OverrideWeight = INDEX_NONE,
	                            float OverrideStartTime = INDEX_NONE, float OverrideEndTime = INDEX_NONE);
	/**
	 * Returns the animation sequence with the highest priority from the given runtime.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 *
	 * @return A pointer to the UAnimSequence object with the highest priority, or nullptr if no animations are found.
	 *
	 * @throws None
	 */
	static TObjectPtr<UAnimSequence> GetHighestPriorityAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime);
	/**
	 * Updates the blend spaces of a skinned mesh runtime.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param ThreadContext The thread context for the skinned mesh.
	 * @param DeltaTime The time elapsed since the last update.
	 * @param Library The global library for the skinned mesh.
	 * @param Library_RenderThread The render thread library for the skinned mesh.
	 * @param Reference The reference data for the skinned mesh.
	 *
	 * @throws None
	 */
	static void UpdateBlendSpaces(FSkinnedMeshRuntime_Lf& Runtime,
	                              const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext, float DeltaTime,
	                              FSkinnedMeshGlobalLibrary_Lf& Library,
	                              FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                              const FSkinnedMeshReference_Lf& Reference);
	/**
	 * Tweak the blend space of the skinned mesh runtime to the wanted position.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param CriticalSection The critical section for thread safety.
	 * @param BlendSpace The minimal blend space to tweak.
	 * @param WantedPosition The desired position to set in the blend space.
	 *
	 * @return True if the blend space was successfully tweaked, false otherwise.
	 *
	 * @throws None
	 */
	static bool TweakBlendSpace(FSkinnedMeshRuntime_Lf& Runtime,
	                            FCriticalSection& CriticalSection,
	                            const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpace,
	                            const FVector3f& WantedPosition);
	/**
	 * Tweak the animation of the skinned mesh runtime.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param ThreadContext The thread context for multi-threading.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param Settings The animation play settings.
	 * @param AnimationID The ID of the animation to tweak.
	 * @param Reference The reference data for the skinned mesh.
	 *
	 * @return True if the animation was successfully tweaked, false otherwise.
	 *
	 * @throws None
	 */
	static bool TweakAnimation(FSkinnedMeshRuntime_Lf& Runtime,
	                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const FTurboSequence_AnimPlaySettings_Lf& Settings,
	                           uint32 AnimationID, const FSkinnedMeshReference_Lf& Reference);
	/**
	 * Solves animations for a given skinned mesh runtime.
	 *
	 * @param Runtime The runtime data of the skinned mesh.
	 * @param Library The global library of skinned meshes.
	 * @param Library_RenderThread The render thread library of skinned meshes.
	 * @param Reference The reference data for the skinned mesh.
	 * @param DeltaTime The time elapsed since the last update.
	 * @param CurrentFrameCount The current frame count.
	 * @param ThreadContext The thread context for multi-threading.
	 *
	 * @throws None
	 */
	static void SolveAnimations(FSkinnedMeshRuntime_Lf& Runtime, FSkinnedMeshGlobalLibrary_Lf& Library,
	                            FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                            const FSkinnedMeshReference_Lf& Reference, float DeltaTime,
	                            int32 CurrentFrameCount,
	                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext);
	/**
	 * Returns a FUintVector containing the hashed values of the given USkeleton, UTurboSequence_MeshAsset_Lf,
	 * and UAnimSequence pointers. If the UAnimSequence pointer is not valid, the third value of the FUintVector
	 * will be GET0_NUMBER.
	 *
	 * @param Skeleton A pointer to the USkeleton object.
	 * @param Asset A pointer to the UTurboSequence_MeshAsset_Lf object.
	 * @param Animation A pointer to the UAnimSequence object.
	 *
	 * @return A FUintVector containing the hashed values of the given pointers.
	 *
	 * @throws None
	 */
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

	/**
* Retrieves the bone transform from local poses for a given bone index.
*
* @param BoneIndex The index of the bone.
* @param LibraryData The animation library data.
* @param ReferenceSkeleton The reference skeleton.
* @param AnimationSkeleton The animation skeleton.
* @param Pose The animation pose.
* @param Animation The animation sequence.
*
* @return The transpose matrix representing the bone transform.
*
* @throws None
*/
	static FTurboSequence_TransposeMatrix_Lf GetBoneTransformFromLocalPoses(int16 BoneIndex,
	                                                                        const FAnimationLibraryData_Lf& LibraryData,
	                                                                        const FReferenceSkeleton& ReferenceSkeleton,
	                                                                        const FReferenceSkeleton& AnimationSkeleton,
	                                                                        const FAnimPose_Lf& Pose,
	                                                                        const TObjectPtr<UAnimSequence> Animation);
	/**
* Retrieves the bone transform from the animation safely.
*
* @param OutAtom The output matrix to store the bone transform.
* @param Animation The animation metadata.
* @param SkeletonBoneIndex The index of the skeleton bone.
* @param Asset The mesh asset.
* @param Library The skinned mesh global library.
* @param ReferenceSkeleton The reference skeleton.
*
* @throws None
*/
	static void GetBoneTransformFromAnimationSafe(
		FMatrix& OutAtom, const FAnimationMetaData_Lf& Animation, uint16 SkeletonBoneIndex,
		const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset, const FSkinnedMeshGlobalLibrary_Lf& Library,
		const FReferenceSkeleton& ReferenceSkeleton);

	/**
* Calculates the bone transformation for a given bone index by blending the animations in the runtime.
*
* @param BoneIndex The index of the bone to calculate the transformation for.
* @param Runtime The runtime data of the skinned mesh.
* @param Reference The reference data for the skinned mesh.
* @param Library The global library of skinned meshes.
*
* @return The transformation of the bone calculated from the blended animations.
*
* @throws None
*/
	static FTransform BendBoneFromAnimations(uint16 BoneIndex,
	                                         const FSkinnedMeshRuntime_Lf& Runtime,
	                                         const FSkinnedMeshReference_Lf& Reference,
	                                         const FSkinnedMeshGlobalLibrary_Lf& Library);
	/**
* Extracts root motion from animations for the given skinned mesh runtime.
*
* @param OutAtom The output atom to store the extracted root motion.
* @param Runtime The runtime data of the skinned mesh.
* @param Reference The reference data for the skinned mesh.
* @param DeltaTime The time elapsed since the last update.
*
* @return None
*
* @throws None
*/
	static void ExtractRootMotionFromAnimations(FTransform& OutAtom,
	                                            const FSkinnedMeshRuntime_Lf& Runtime,
	                                            const FSkinnedMeshReference_Lf& Reference,
	                                            float DeltaTime);

	/**
* Calculates the IK transform for a specific bone index based on the skinned mesh runtime.
*
* @param OutAtom The output transform for the bone.
* @param BoneIndex The index of the bone to calculate the IK transform for.
* @param Runtime The skinned mesh runtime data.
* @param Reference The reference data for the skinned mesh.
* @param Library The global library of skinned meshes.
* @param Library_RenderThread The render thread library of skinned meshes.
* @param Space The bone space type for the transformation.
* @param AnimationDeltaTime The time elapsed during the animation.
* @param CurrentFrameCount The current frame count of the animation.
* @param ThreadContext The thread context for the skinned mesh.
*
* @throws None
*/
	static void GetIKTransform(FTransform& OutAtom, uint16 BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FSkinnedMeshGlobalLibrary_Lf& Library,
	                           FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                           const EBoneSpaces::Type Space, float AnimationDeltaTime,
	                           int32 CurrentFrameCount,
	                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext);
	/**
* Sets the IK transform for a bone in the skinned mesh runtime.
*
* @param Atom The transform to set.
* @param BoneIndex The index of the bone.
* @param Runtime The skinned mesh runtime.
* @param Reference The skinned mesh reference.
* @param CriticalSection The critical section for thread safety.
* @param Space The bone space type for the transformation. Defaults to WorldSpace.
*
* @return true if the IK transform was successfully set, false otherwise.
*
* @throws None
*/
	static bool SetIKTransform(const FTransform& Atom, uint16 BoneIndex,
	                           FSkinnedMeshRuntime_Lf& Runtime,
	                           const FSkinnedMeshReference_Lf& Reference,
	                           FCriticalSection& CriticalSection,
	                           const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);
	/**
* Clears the IK state of the given skinned mesh runtime.
*
* @param Runtime The skinned mesh runtime whose IK state needs to be cleared.
* @param CriticalSection The critical section for thread safety.
*
* @throws None
*/
	static FORCEINLINE_DEBUGGABLE void ClearIKState(FSkinnedMeshRuntime_Lf& Runtime, FCriticalSection& CriticalSection)
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
		for (uint16 Remove : ToRemove)
		{
			CriticalSection.Lock();
			Runtime.IKData.Remove(Remove);
			CriticalSection.Unlock();
		}
	}
	/**
* Retrieves the animation curve data for a given animation.
*
* @param Runtime The skinned mesh runtime.
* @param Animation The animation metadata.
* @param Library The global animation library.
* @param CurveName The name of the curve to retrieve.
*
* @return The pose curve data associated with the animation curve.
*
* @throws None
*/
	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveByAnimation(
		const FSkinnedMeshRuntime_Lf& Runtime, const FAnimationMetaData_Lf& Animation,
		const FSkinnedMeshGlobalLibrary_Lf& Library, const FName& CurveName);
	/**
* Retrieves the animation curve data at a specific time for a given curve name.
*
* @param Runtime The runtime data of the skinned mesh.
* @param CurveName The name of the curve.
* @param Library The global library data of the skinned mesh.
* @param CriticalSection The critical section for thread safety.
*
* @return An array of FTurboSequence_PoseCurveData_Lf objects representing the animation curve data.
*
* @throws None
*/
	static TArray<FTurboSequence_PoseCurveData_Lf> GetAnimationCurveAtTime(
		const FSkinnedMeshRuntime_Lf& Runtime, const FName& CurveName, const FSkinnedMeshGlobalLibrary_Lf& Library,
		FCriticalSection& CriticalSection);
	/**
* Retrieves the transform of a socket in the given skinned mesh runtime.
*
* @param OutTransform The output transform of the socket.
* @param SocketName The name of the socket.
* @param Runtime The skinned mesh runtime.
* @param Reference The reference data of the skinned mesh.
* @param Library The global library of skinned meshes.
* @param Library_RenderThread The render thread library of skinned meshes.
* @param Space The bone space type for the transformation.
* @param AnimationDeltaTime The time elapsed during the animation.
* @param CurrentFrameCount The current frame count of the animation.
* @param ThreadContext The thread context for the skinned mesh.
*
* @throws None
*/
	static void GetSocketTransform(FTransform& OutTransform, const FName& SocketName,
	                               FSkinnedMeshRuntime_Lf& Runtime,
	                               const FSkinnedMeshReference_Lf& Reference,
	                               FSkinnedMeshGlobalLibrary_Lf& Library,
	                               FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
	                               const EBoneSpaces::Type Space,
	                               float AnimationDeltaTime,
	                               int32 CurrentFrameCount,
	                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext);
};
