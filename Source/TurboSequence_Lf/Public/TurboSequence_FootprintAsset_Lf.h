// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "UObject/Object.h"
#include "TurboSequence_FootprintAsset_Lf.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_FootprintAsset_Lf : public UDataAsset
{
	GENERATED_BODY()

public:
	UTurboSequence_FootprintAsset_Lf();

	/**
	 * Overrides the Mesh Visibility
	 * @param IsVisibleOverride Choose the Visibility of the Mesh
	 * @param bDefaultVisibility The Current Visibility of this Mesh this Frame
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnSetMeshIsVisible_Concurrent(ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
	                                           const bool bDefaultVisibility, const int32 MeshID,
	                                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Gets the Final Visiblity of the Mesh and only calls when the visibility changes
	 * @param bHasVisibilityChanged The Final Visibility
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnGetMeshVisibilityChanged_Concurrent(bool bHasVisibilityChanged, const int32 MeshID,
	                                                   const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Overrides the Mesh Is Animated State
	 * @param IsAnimatedOverride Choose the Animated State of the Mesh
	 * @param bDefaultIsAnimated The Current is Animated state of this Mesh this Frame
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnSetMeshIsAnimated_Concurrent(ETurboSequence_IsAnimatedOverride_Lf& IsAnimatedOverride,
	                                          const bool bDefaultIsAnimated, const int32 MeshID,
	                                          const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Overrides the Auto Lod Update of the Mesh
	 * @param bIsUpdatingLodOverride Choose if the Mesh Should Auto Update the Lod
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride, const int32 MeshID,
	                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Gets the Lod Index state of the Mesh
	 * @param OldLodIndex The Old Lod Index
	 * @param NewLodIndex The Final Lod Index
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnGetMeshLodChanged_Concurrent(const int16 OldLodIndex, const int16 NewLodIndex, const int32 MeshID,
	                                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Sets the Lod Index of the Mesh Instance
	 * @param LodIndexOverride The Wanted Lod Index, by default it's the valid system Lod index
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnSetMeshLod_Concurrent(int16& LodIndexOverride, const int32 MeshID,
	                                     const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * When you call ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread this function getting called
	 * @param MeshID The Mesh ID
	 * @param FromAsset The Mesh Asset
	 */
	virtual void OnAddedMeshInstance_GameThread(const int32 MeshID,
	                                            const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset)
	{
	}

	/**
	 * When you call ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread this function getting called
	 * @param MeshID The Mesh ID
	 * @param FromAsset The Mesh Asset
	 */
	virtual void OnRemovedMeshInstance_GameThread(const int32 MeshID,
	                                              const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset)
	{
	}

	/**
	 * Gets called when the Mesh made it into the final stage of the CPU part which is the schedulling to the Compute shader,
	 * basically when Animation Playing Visually this frame it getting called
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnMeshFinalRenderingUpdate_Concurrent(const int32 MeshID,
	                                                   const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Gets always called when a Mesh Instance is valid and the Update Group solves this frame
	 * @param MeshID The Mesh ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnMeshPreSolveAnimationMeta_Concurrent(const int32 MeshID,
	                                                    const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                                    ThreadContext)
	{
	}

	/**
	 * Gets Called when an Animation getting added into a mesh with the Play Animation function or with the Play Blendspace Function
	 * @param MeshID The Mesh ID
	 * @param AnimationID The Animation ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnAddedMeshAnimation_Concurrent(const int32 MeshID, const uint32 AnimationID,
	                                             const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}

	/**
	 * Gets called when the Animation gets removed by user Input or by the manager system internally
	 * @param MeshID The Mesh ID
	 * @param AnimationID The Animation ID
	 * @param ThreadContext The Thread Context for using Critical Sections
	 */
	virtual void OnRemovedMeshAnimation_Concurrent(const int32 MeshID, const uint32 AnimationID,
	                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
	{
	}


	/**
	*  Calls when the first mesh was referencing this Footprint asset,
	*  it can happen multiple times in a simulation
	 */
	virtual void OnFirstMeshAdded_GameThread()
	{
	}


	/**
	* Getting called when the last mesh which was referencing this Footprint asset was removed,
	* it does mean at the moment on this function call,
	* there are all meshes which reference this footprint asset removed from the manager
	* and it can happen multiple times in a simulation
	 */
	virtual void OnLastMeshRemoved_GameThread()
	{
	}


	/**
	 * Getting called when the manager updates via Tick
	 * @param DeltaTime The Delta Time of the Manager Tick Function
	 */
	virtual void OnManagerUpdated_GameThread(const float DeltaTime)
	{
	}


	/**
	 * Getting called when the Manager exit playmode
	 * @param EndPlayReason The Play End reason of the Manager
	 */
	virtual void OnManagerEndPlay_GameThread(const EEndPlayReason::Type EndPlayReason)
	{
	}
};
