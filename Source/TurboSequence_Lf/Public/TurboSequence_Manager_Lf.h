// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_Data_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "TurboSequence_Utility_Lf.h"
#include "GameFramework/Actor.h"
#include "TurboSequence_Manager_Lf.generated.h"

UCLASS()
class TURBOSEQUENCE_LF_API ATurboSequence_Manager_Lf : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurboSequence_Manager_Lf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	< - - - - - - - - - - - - - - - - - - - - >
					DATA
	< - - - - - - - - - - - - - - - - - - - - >
*/

	// For the Frustum Culling we need Camera Transforms
	inline static TMap<uint8, FTransform> LastFrameCameraTransforms;
	// Indicates when the Mesh is Solving at the moment
	inline static bool bMeshesSolvingAtTheMoment = false;
	// The Global Library which holds all Game Thread Data
	inline static FSkinnedMeshGlobalLibrary_Lf GlobalLibrary;
	// The Global Library which holds all Render Thread Data
	inline static FSkinnedMeshGlobalLibrary_RenderThread_Lf GlobalLibrary_RenderThread;

	// The Instance
	inline static TObjectPtr<ATurboSequence_Manager_Lf> Instance;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	// The Rendering Part needs Niagara to draw the meshes: 1 Draw Call per Mesh | 1 Draw Call per Material
	TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf> NiagaraComponents;

	UPROPERTY(EditAnywhere)
	// The Global Data which keeps track of internal Texture Data
	TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData;

protected:
	UPROPERTY()
	// The Current Thread Context, Please Call GetThreadContext()
	TObjectPtr<UTurboSequence_ThreadContext_Lf> CurrentThreadContext_Runtime;

public:
	/**
	 * Get the Thread Context | Any Thread
	 * @return The Thread Context with the Critical Section
	 */
	TObjectPtr<UTurboSequence_ThreadContext_Lf> GetThreadContext()
	{
		if (IsInGameThread())
		{
			if (!IsValid(CurrentThreadContext_Runtime))
			{
				CurrentThreadContext_Runtime = NewObject<UTurboSequence_ThreadContext_Lf>();
			}
		}

		if (!IsValid(CurrentThreadContext_Runtime))
		{
			UE_LOG(LogTurboSequence_Lf, Error, TEXT("Make sure to refresh the Thread Context in the main thread..., to fix this call the Game Thread Functions in the Game Thread..."));
		}

		return CurrentThreadContext_Runtime;
	}


	/**
	 * Refreshes the Thread Context, usually called in Tick from the Manager 
	 */
	void RefreshThreadContext_GameThread()
	{
		CurrentThreadContext_Runtime = NewObject<UTurboSequence_ThreadContext_Lf>();
	}

	// Will be used internally to collect garbage once per frame even when calling more than 1 time
	bool bCollectGarbageThisFrame = false;


	/**
	 * Adds a Skinned Mesh Instance in a World 
	 * @param FromSpawnData The Spawn Data to have info about what to spawn
	 * @param SpawnTransform The Transform where to spawn
	 * @param InWorld In which world
	 * @return The Minimal Mesh Data which is the Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Instance Spawn Data", Keywords="Turbo, Sequence, TS, Add, Create, Instance, Spawn"))
	static FTurboSequence_MinimalMeshData_Lf AddSkinnedMeshInstance_GameThread(const FTurboSequence_MeshSpawnData_Lf FromSpawnData,
	                                                                           const FTransform SpawnTransform,
	                                                                           UWorld* InWorld);

	/**
	 * Removes a Skinned Mesh Instance from a World
	 * @param MeshData The Mesh IDs to understand which mesh
	 * @param InWorld In which world we remove the instance
	 * @return true if the function is successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Remove, Destroy, Instance, Despawn"))
	static bool RemoveSkinnedMeshInstance_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld);

protected:
	// NODE: Please use the versions with FTurboSequence_MeshSpawnData_Lf& FromSpawnData
	static uint32 AddSkinnedMeshInstance_GameThread(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
	                                                const FTransform& SpawnTransform,
	                                                const TObjectPtr<UWorld> InWorld,
	                                                const TArray<TObjectPtr<UMaterialInterface>>& OverrideMaterials =
		                                                TArray<TObjectPtr<UMaterialInterface>>(),
	                                                const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset = nullptr);

	static bool RemoveSkinnedMeshInstance_GameThread(const int64& MeshID, const TObjectPtr<UWorld> InWorld);

public:
	/**
	 * Solves all Meshes of one Update Group
	 * @param DeltaTime The Delta Time because the function should run every frame or every Update Group Tick
	 * @param InWorld In Which world we like to solve the Meshes
	 * @param UpdateContext The Update Context to have info like which update group etc.
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Animation, Mesh, Motion, Transform, Solve, Update, Render, Animate"))
	static void SolveMeshes_GameThread(const float& DeltaTime, UWorld* InWorld, FTurboSequence_UpdateContext_Lf UpdateContext);

protected:
	static void SolveMeshes_RenderThread(FRHICommandListImmediate& RHICmdList);

public:
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Add, Update, Mesh, Group"))
	static void AddInstanceToUpdateGroup_RawID_Concurrent(const int32 GroupIndex, const int64& MeshID);

	/**
	 * Adds an Mesh to the Update Group with the given Index
	 * @param GroupIndex The Update Group Index, it will automatic increase the 2D Array to match the 1st Dimension with this index
	 * @param MeshData The Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Add, Update, Mesh, Group"))
	static void AddInstanceToUpdateGroup_Concurrent(const int32 GroupIndex, const FTurboSequence_MinimalMeshData_Lf& MeshData);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Remove, Update, Mesh, Group"))
	static void RemoveInstanceFromUpdateGroup_RawID_Concurrent(const int32 GroupIndex, const int64& MeshID);


	/**
	 * Removes a Instance in the Update Group
	 * @param GroupIndex The Group Index which hosting the Mesh ID
	 * @param MeshData The Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Remove, Update, Mesh, Group"))
	static void RemoveInstanceFromUpdateGroup_Concurrent(const int32 GroupIndex, const FTurboSequence_MinimalMeshData_Lf& MeshData);

	/**
	 * Get the Number of Mesh Collections in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @return The Number of Mesh Collections in a Update Group
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Number, Num, Update, Mesh, Group"))
	static int32 GetNumMeshCollectionsInUpdateGroup_Concurrent(const int32 GroupIndex);

	/**
	 * Get the Number of Mesh IDs in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @return The Number of Mesh IDs in a Update Group
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Number, Num, Update, Mesh, Group"))
	static int32 GetNumMeshIDsInUpdateGroup_RawID_Concurrent(const int32 GroupIndex);


	/**
	 * Clean the Manager, NOTE: The manager is auto manage itself, you don't need to implement this function
	 * @param bIsEndPlay Is end of the play session or map change
	 */
	static void CleanManager_GameThread(const bool& bIsEndPlay);

	/*
    < - - - - - - - - - - - - - - - - - - - - >
                    Helpers
    < - - - - - - - - - - - - - - - - - - - - >
*/
	static FORCEINLINE_DEBUGGABLE void ClearBuffers()
	{
		LastFrameCameraTransforms.Empty();
		GlobalLibrary = FSkinnedMeshGlobalLibrary_Lf();
		GlobalLibrary_RenderThread = FSkinnedMeshGlobalLibrary_RenderThread_Lf();
	}

	/**
	 * Get the Mesh World Space Transform
	 * @param MeshData The Mesh ID
	 * @return The World Space Transform of the Mesh Instance
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="World Space Transform", Keywords="Turbo, Sequence, TS, Get, Transform, World, Mesh"))
	static FTransform GetMeshWorldSpaceTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData)
	{
		if (MeshData.IsMeshDataValid())
		{
			return GetMeshWorldSpaceTransform_RawID_Concurrent(MeshData.RootMotionMeshID);
		}
		return FTransform::Identity;
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="World Space Transform", Keywords="Turbo, Sequence, TS, Get, Transform, World, Mesh"))
	static FTransform GetMeshWorldSpaceTransform_RawID_Concurrent(const int64& MeshID)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(GlobalLibrary.RuntimeSkinnedMeshes[MeshID]);
		}
		return FTransform::Identity;
	}

	/**
	 * Get the Mesh to Camera Distance
	 * @param MeshData The Mesh ID
	 * @return The Closest Distance to the Closest Camera
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Camera Distance", Keywords="Turbo, Sequence, TS, Get, Distance, Camera, Mesh"))
	static float GetMeshClosestCameraDistance_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData)
	{
		if (MeshData.IsMeshDataValid())
		{
			return GetMeshClosestCameraDistance_RawID_Concurrent(MeshData.RootMotionMeshID);
		}
		return INDEX_NONE;
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Camera Distance", Keywords="Turbo, Sequence, TS, Get, Distance, Camera, Mesh"))
	static float GetMeshClosestCameraDistance_RawID_Concurrent(const int64& MeshID)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return GlobalLibrary.RuntimeSkinnedMeshes[MeshID].ClosestCameraDistance;
		}
		return INDEX_NONE;
	}

	/**
	 * Setting ( Location, Rotation, Scale ) to the World Space Mesh Instance Transform
	 * @param MeshData The Mesh ID
	 * @param Location The Location to set
	 * @param Rotation The Rotation to set
	 * @param Scale The Scale to set
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceLocationRotationScale_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData, const FVector Location, const FQuat Rotation, const FVector Scale)
	{
		if (MeshData.IsMeshDataValid())
		{
			SetMeshWorldSpaceLocationRotationScale_RawID_Concurrent(MeshData.RootMotionMeshID, Location, Rotation, Scale);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				SetMeshWorldSpaceLocationRotationScale_RawID_Concurrent(MeshID, Location, Rotation, Scale);
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceLocationRotationScale_RawID_Concurrent(const int64& MeshID, const FVector Location, const FQuat Rotation, const FVector Scale)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
			Runtime.WorldSpaceTransform.SetLocation(Location);
			Runtime.WorldSpaceTransform.SetRotation(Rotation);
			Runtime.WorldSpaceTransform.SetScale3D(Scale);
			if (FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset]; Reference.
				RenderData[Runtime.MaterialsHash].InstanceMap.Contains(MeshID))
			{
				FTurboSequence_Utility_Lf::UpdateInstanceTransform_Concurrent(
					Reference, Runtime,
					FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime));
			}
		}
	}

	/**
	 * Set a Transform to the World Space Mesh Instance Transform
	 * @param MeshData The Mesh ID
	 * @param Transform The Transform to set
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData, const FTransform Transform)
	{
		if (MeshData.IsMeshDataValid())
		{
			SetMeshWorldSpaceTransform_RawID_Concurrent(MeshData.RootMotionMeshID, Transform);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				SetMeshWorldSpaceTransform_RawID_Concurrent(MeshID, Transform);
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceTransform_RawID_Concurrent(const int64& MeshID, const FTransform Transform)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
			Runtime.WorldSpaceTransform = Transform;
			if (FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset]; Reference.
				RenderData[Runtime.MaterialsHash].InstanceMap.Contains(MeshID))
			{
				FTurboSequence_Utility_Lf::UpdateInstanceTransform_Concurrent(
					Reference, Runtime,
					FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime));
			}
		}
	}

	/**
	 * Gets the Root Motion Transforms based on all animation which are playing, accurate blended with the Animation Weights
	 * @param OutRootMotion The Returning Root Motion Transform
	 * @param MeshData The Mesh ID
	 * @param DeltaTime The Delta Time of the Group Delta Time or the Frame Delta Time, because the Root Motion Extraction needs DeltaTime for defining the Scalar Transform
	 * @param Space The Space of the Extraction
	 * @return True if the Function is Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static bool GetRootMotionTransform_Concurrent(FTransform& OutRootMotion,
	                                              const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                              const float& DeltaTime,
	                                              const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace)
	{
		if (MeshData.IsMeshDataValid())
		{
			return GetRootMotionTransform_RawID_Concurrent(OutRootMotion, MeshData.RootMotionMeshID, DeltaTime, Space);
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static bool GetRootMotionTransform_RawID_Concurrent(FTransform& OutRootMotion,
	                                                    const int64& MeshID,
	                                                    const float& DeltaTime,
	                                                    const EBoneSpaces::Type& Space = EBoneSpaces::WorldSpace)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}

		const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
		FTurboSequence_Utility_Lf::ExtractRootMotionFromAnimations(OutRootMotion, Runtime, Reference, DeltaTime);

		if (Space == EBoneSpaces::ComponentSpace)
		{
			return true;
		}

		OutRootMotion *= FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime);

		return true;
	}

	/**
	 * A Helper function to move the mesh in World Space with Root Motion
	 * @param MeshData The Mesh ID
	 * @param DeltaTime The Delta Time which the Root Motion needs to get extracted, Use your Group Delta Time or the Frame Delta Time
	 * @param bZeroZAxis Do you want Zero Z values in the Component Space Root Motion Transform
	 * @param bIncludeScale Do you want include the Root Motion Scale3D Factor which will scale the mesh
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static void MoveMeshWithRootMotion_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                              const float& DeltaTime,
	                                              const bool bZeroZAxis = false,
	                                              const bool bIncludeScale = false)
	{
		if (MeshData.IsMeshDataValid())
		{
			MoveMeshWithRootMotion_RawID_Concurrent(MeshData.RootMotionMeshID, DeltaTime, bZeroZAxis, bIncludeScale);
			const FTransform& MeshTransform = GetMeshWorldSpaceTransform_RawID_Concurrent(MeshData.RootMotionMeshID);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				SetMeshWorldSpaceTransform_RawID_Concurrent(MeshID, MeshTransform);
			}
		}
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static void MoveMeshWithRootMotion_RawID_Concurrent(const int64& MeshID,
	                                                    const float& DeltaTime,
	                                                    const bool& ZeroZAxis = false,
	                                                    const bool& bIncludeScale = false)
	{
		if (FTransform Atom; GetRootMotionTransform_RawID_Concurrent(Atom, MeshID, DeltaTime, EBoneSpaces::ComponentSpace))
		{
			if (ZeroZAxis)
			{
				FVector AtomLocation = Atom.GetLocation();
				AtomLocation.Z = GET0_NUMBER;
				Atom.SetLocation(AtomLocation);
			}

			FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
			Atom *= Runtime.WorldSpaceTransform;
			//Atom *= Runtime.DeltaOffsetTransform.Inverse();
			if (bIncludeScale)
			{
				Runtime.WorldSpaceTransform.SetScale3D(Atom.GetScale3D());
			}
			Runtime.WorldSpaceTransform.SetRotation(Atom.GetRotation());
			Runtime.WorldSpaceTransform.SetLocation(Atom.GetLocation());
			if (FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
				Reference.RenderData[Runtime.MaterialsHash].InstanceMap.Contains(MeshID))
			{
				FTurboSequence_Utility_Lf::UpdateInstanceTransform_Concurrent(
					Reference, Runtime,
					FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime));
			}
		}
	}

	/**
	 * Get the Number of all meshes no matter which update group
	 * @return The Number of all all instance meshes, no matter which Update Group
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Num Meshes", Keywords="Turbo, Sequence, TS, Get, Number, Instances"))
	static int32 GetNumOfAllMeshes_Concurrent()
	{
		return GlobalLibrary.RuntimeSkinnedMeshes.Num();
	}

	/**
	 * Playing an Animation
	 * @param MeshData Mesh ID
	 * @param Animation The Animation to Play
	 * @param AnimSettings The Animation Settings for playing the Animation
	 * @return The Animation ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Animation Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay"))
	static FTurboSequence_AnimMinimalCollection_Lf PlayAnimation_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                                        UAnimSequence* Animation,
	                                                                        const FTurboSequence_AnimPlaySettings_Lf& AnimSettings
	)
	{
		if (MeshData.IsMeshDataValid())
		{
			FTurboSequence_AnimMinimalCollection_Lf Data = FTurboSequence_AnimMinimalCollection_Lf(true);
			Data.RootMotionMesh = PlayAnimation_RawID_Concurrent(MeshData.RootMotionMeshID, Animation, AnimSettings);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				Data.CustomizableMeshes.Add(PlayAnimation_RawID_Concurrent(MeshID, Animation, AnimSettings));
			}
			return Data;
		}
		return FTurboSequence_AnimMinimalCollection_Lf(false);
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Animation Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay"))
	static FTurboSequence_AnimMinimalData_Lf PlayAnimation_RawID_Concurrent(const int64& MeshID,
	                                                                        UAnimSequence* Animation,
	                                                                        const FTurboSequence_AnimPlaySettings_Lf& AnimSettings
	)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return FTurboSequence_AnimMinimalData_Lf(false);
		}

		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		bool bLoop = true;
		if (IsValid(Animation)) // Cause of rest pose will pass here
		{
			bLoop = Animation->bLoop;
		}
		FTurboSequence_AnimMinimalData_Lf MinimalAnimation = FTurboSequence_AnimMinimalData_Lf(true);
		MinimalAnimation.BelongsToMeshID = MeshID;
		MinimalAnimation.AnimationID = FTurboSequence_Utility_Lf::PlayAnimation(Reference, GlobalLibrary, GlobalLibrary_RenderThread, Runtime, Instance->GetThreadContext()->CriticalSection, Animation, AnimSettings, bLoop);
		return MinimalAnimation;
	}


	/**
	 * Playing an Blend Space
	 * @param MeshData The Mesh ID
	 * @param BlendSpace The Blend Space to play
	 * @param AnimSettings The Animation settings for this blend space
	 * @return The Blend Space ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Blend Space Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay, Blend Space, Blend"))
	static FTurboSequence_AnimMinimalBlendSpaceCollection_Lf PlayBlendSpace_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                                                   UBlendSpace* BlendSpace,
	                                                                                   const FTurboSequence_AnimPlaySettings_Lf AnimSettings
	)
	{
		if (MeshData.IsMeshDataValid())
		{
			FTurboSequence_AnimMinimalBlendSpaceCollection_Lf Data = FTurboSequence_AnimMinimalBlendSpaceCollection_Lf(true);
			Data.RootMotionMesh = PlayBlendSpace_RawID_Concurrent(MeshData.RootMotionMeshID, BlendSpace, AnimSettings);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				Data.CustomizableMeshes.Add(PlayBlendSpace_RawID_Concurrent(MeshID, BlendSpace, AnimSettings));
			}
			return Data;
		}
		return FTurboSequence_AnimMinimalBlendSpaceCollection_Lf(false);
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Blend Space Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay, Blend Space, Blend"))
	static FTurboSequence_AnimMinimalBlendSpace_Lf PlayBlendSpace_RawID_Concurrent(const int64& MeshID,
	                                                                               UBlendSpace* BlendSpace,
	                                                                               const FTurboSequence_AnimPlaySettings_Lf& AnimSettings
	)
	{
		if (!IsValid(BlendSpace))
		{
			return FTurboSequence_AnimMinimalBlendSpace_Lf(false);
		}

		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return FTurboSequence_AnimMinimalBlendSpace_Lf(false);
		}

		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		return FTurboSequence_Utility_Lf::PlayBlendSpace(Reference, GlobalLibrary, GlobalLibrary_RenderThread, Runtime, BlendSpace, Instance->GetThreadContext()->CriticalSection, AnimSettings);
	}


	/**
	 * Gets the highest priority animation of a mesh
	 * @param MeshData The Mesh ID
	 * @return The highest priority animation
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Animation Sequence", Keywords="Turbo, Sequence, TS, Get, Animation, Play, Priority"))
	static UAnimSequence* GetHighestPriorityPlayingAnimation_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData)
	{
		if (MeshData.IsMeshDataValid())
		{
			return GetHighestPriorityPlayingAnimation_RawID_Concurrent(MeshData.RootMotionMeshID);
		}
		return nullptr;
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Animation Sequence", Keywords="Turbo, Sequence, TS, Get, Animation, Play, Priority"))
	static UAnimSequence* GetHighestPriorityPlayingAnimation_RawID_Concurrent(const int64& MeshID)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return nullptr;
		}

		const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		return FTurboSequence_Utility_Lf::GetHighestPriorityAnimation(Runtime);
	}

	/**
	 * Tweaks an Animation with new Settings
	 * @param TweakSettings The new Animation Settings
	 * @param AnimationData The Animation ID
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks"))
	static bool TweakAnimation_Concurrent(const FTurboSequence_AnimPlaySettings_Lf TweakSettings, const FTurboSequence_AnimMinimalData_Lf& AnimationData)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(AnimationData.BelongsToMeshID))
		{
			return false;
		}

		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[AnimationData.BelongsToMeshID];
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
		return FTurboSequence_Utility_Lf::TweakAnimation(Runtime, Instance->GetThreadContext()->CriticalSection, GlobalLibrary, GlobalLibrary_RenderThread, TweakSettings, AnimationData.AnimationID, Reference);
	}

	/**
	 * Tweaks an Animation Collection with new Settings
	 * @param TweakSettings The new Animation Settings
	 * @param AnimationData The Animation IDs
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks"))
	static bool TweakAnimationCollection_Concurrent(const FTurboSequence_AnimPlaySettings_Lf TweakSettings, const FTurboSequence_AnimMinimalCollection_Lf& AnimationData)
	{
		if (AnimationData.IsAnimCollectionValid())
		{
			bool bValid = TweakAnimation_Concurrent(TweakSettings, AnimationData.RootMotionMesh);
			for (const FTurboSequence_AnimMinimalData_Lf& ID : AnimationData.CustomizableMeshes)
			{
				if (TweakAnimation_Concurrent(TweakSettings, ID))
				{
					bValid = true;
				}
			}
			return bValid;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks, Blend"))
	static bool TweakBlendSpace_RawID_Concurrent(const int64& MeshID, const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpaceData, const FVector3f& WantedPosition)
	{
		if (!BlendSpaceData.IsAnimBlendSpaceValid())
		{
			return false;
		}

		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}

		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		//const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		return FTurboSequence_Utility_Lf::TweakBlendSpace(Runtime, Instance->GetThreadContext()->CriticalSection, BlendSpaceData, WantedPosition);
	}

	/**
	 * Tweaks an Blend Space with a Grid Position
	 * @param BlendSpaceData The Blend Space ID
	 * @param WantedPosition The new Blend Space Grid Position
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks, Blend"))
	static bool TweakBlendSpace_Concurrent(const FTurboSequence_AnimMinimalBlendSpaceCollection_Lf& BlendSpaceData, const FVector3f WantedPosition)
	{
		if (BlendSpaceData.IsAnimCollectionValid())
		{
			bool bValid = TweakBlendSpace_RawID_Concurrent(BlendSpaceData.RootMotionMesh.BelongsToMeshID, BlendSpaceData.RootMotionMesh, WantedPosition);
			for (const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpace : BlendSpaceData.CustomizableMeshes)
			{
				if (TweakBlendSpace_RawID_Concurrent(BlendSpace.BelongsToMeshID, BlendSpace, WantedPosition))
				{
					bValid = true;
				}
			}
			return bValid;
		}
		return false;
	}

	/**
	 * Get the Current Animation Setting of an Animation as ref
	 * @param AnimationSettings Out Animation Setting
	 * @param AnimationData From Animation Data
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Settings, Tweaks, Weight, Scale, Speed"))
	static bool GetAnimationSettings_Concurrent(FTurboSequence_AnimPlaySettings_Lf& AnimationSettings, const FTurboSequence_AnimMinimalData_Lf& AnimationData)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(AnimationData.BelongsToMeshID))
		{
			return false;
		}

		const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[AnimationData.BelongsToMeshID];
		if (!Runtime.AnimationIDs.Contains(AnimationData.AnimationID))
		{
			return false;
		}

		const FAnimationMetaData_Lf& AnimationFrame = Runtime.AnimationMetaData[Runtime.AnimationIDs[AnimationData.AnimationID]];
		AnimationSettings = AnimationFrame.Settings;
		return true;
	}

	/**
	 * Gets the Current Animation Settings from an Animation Collection as Ref
	 * @param AnimationSettings Out Animation Settings Array
	 * @param AnimationData The Animation IDs
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Settings, Tweaks, Weight, Scale, Speed"))
	static bool GetAnimationCollectionSettings_Concurrent(TArray<FTurboSequence_AnimPlaySettings_Lf>& AnimationSettings, const FTurboSequence_AnimMinimalCollection_Lf& AnimationData)
	{
		FTurboSequence_AnimPlaySettings_Lf RootSettings;
		if (GetAnimationSettings_Concurrent(RootSettings, AnimationData.RootMotionMesh))
		{
			AnimationSettings.Reset();
			AnimationSettings.Add(RootSettings);
			for (const FTurboSequence_AnimMinimalData_Lf& Data : AnimationData.CustomizableMeshes)
			{
				FTurboSequence_AnimPlaySettings_Lf CustomizableSettings;
				GetAnimationSettings_Concurrent(CustomizableSettings, Data);
				AnimationSettings.Add(CustomizableSettings);
			}
			return true;
		}
		return false;
	}

	/**
	 * Get the IK Transform of an Bone by Ref
	 * @param OutIKTransform The IK Transform which getting created
	 * @param MeshData The Mesh ID
	 * @param BoneName The Bone Name of the IK bone
	 * @param AnimationDeltaTime Sometimes the Animations Update, so we need a DeltaTime in this case
	 * @param Space The Space of the IK Transform
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Transform, IK, Bone"))
	static bool GetIKTransform_Concurrent(FTransform& OutIKTransform,
	                                      const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                      const FName BoneName,
	                                      const float AnimationDeltaTime,
	                                      const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace)
	{
		if (MeshData.IsMeshDataValid())
		{
			if (GetIKTransform_RawID_Concurrent(OutIKTransform, MeshData.RootMotionMeshID, BoneName, AnimationDeltaTime, Space))
			{
				return true;
			}

			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (GetIKTransform_RawID_Concurrent(OutIKTransform, MeshID, BoneName, AnimationDeltaTime, Space))
				{
					return true;
				}
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Transform, IK, Bone"))
	static bool GetIKTransform_RawID_Concurrent(FTransform& OutIKTransform,
	                                            const int64& MeshID,
	                                            const FName& BoneName,
	                                            const float& AnimationDeltaTime,
	                                            const EBoneSpaces::Type& Space = EBoneSpaces::WorldSpace)
	{
		OutIKTransform = FTransform::Identity;

		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}
		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		const FReferenceSkeleton& ReferenceSkeleton = FTurboSequence_Utility_Lf::GetReferenceSkeleton(Runtime.DataAsset);
		if (const int32& BoneIndexFromName = FTurboSequence_Utility_Lf::GetSkeletonBoneIndex(ReferenceSkeleton, BoneName); FTurboSequence_Utility_Lf::GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndexFromName))
		{
			const int64& CurrentFrameCount = UKismetSystemLibrary::GetFrameCount();

			FTurboSequence_Utility_Lf::GetIKTransform(OutIKTransform, BoneIndexFromName, Runtime, Reference,
			                                          GlobalLibrary, GlobalLibrary_RenderThread, Space, AnimationDeltaTime, CurrentFrameCount,
			                                          Instance->GetThreadContext()->CriticalSection);

			return true;
		}

		return false;
	}

	/**
	 * Sets the IK Transform from a Bone of a Mesh
	 * @param MeshData The Mesh ID
	 * @param BoneName The Bone Name of the Bone which is effected
	 * @param IKTransform The way the Bone should bend defined with an Transform
	 * @param Space The Space of the Transform
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Transform, IK, Bone"))
	static bool SetIKTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                      const FName BoneName,
	                                      const FTransform IKTransform,
	                                      const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace)
	{
		if (MeshData.IsMeshDataValid())
		{
			if (SetIKTransform_RawID_Concurrent(MeshData.RootMotionMeshID, BoneName, IKTransform, Space))
			{
				return true;
			}

			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (SetIKTransform_RawID_Concurrent(MeshID, BoneName, IKTransform, Space))
				{
					return true;
				}
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Transform, IK, Bone"))
	static bool SetIKTransform_RawID_Concurrent(const int64& MeshID,
	                                            const FName& BoneName,
	                                            const FTransform& IKTransform,
	                                            const EBoneSpaces::Type& Space = EBoneSpaces::WorldSpace)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}
		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		if (!IsValid(Runtime.DataAsset))
		{
			return false;
		}
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		const FReferenceSkeleton& ReferenceSkeleton = FTurboSequence_Utility_Lf::GetReferenceSkeleton(Runtime.DataAsset);
		if (const int32& BoneIndexFromName = FTurboSequence_Utility_Lf::GetSkeletonBoneIndex(ReferenceSkeleton, BoneName); FTurboSequence_Utility_Lf::GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndexFromName))
		{
			return FTurboSequence_Utility_Lf::SetIKTransform(IKTransform, BoneIndexFromName, Runtime, Reference, Instance->GetThreadContext()->CriticalSection, Space);
		}

		return false;
	}


	/**
	 * Gets the Socket Transform of a Mesh by Ref
	 * @param OutSocketTransform The Socket Transform which getting calculated
	 * @param MeshData The Mesh ID
	 * @param SocketName The Socket name from the Skeleton or Mesh Asset
	 * @param AnimationDeltaTime Sometimes the Animation is solving to define the Transform, so we need the DeltaTime
	 * @param Space The Space which the Transform getting calculated
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Socket, Transform, IK, Item"))
	static bool GetSocketTransform_Concurrent(FTransform& OutSocketTransform,
	                                          const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                          const FName SocketName,
	                                          const float AnimationDeltaTime,
	                                          const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace)
	{
		if (MeshData.IsMeshDataValid())
		{
			if (GetSocketTransform_RawID_Concurrent(OutSocketTransform, MeshData.RootMotionMeshID, SocketName, AnimationDeltaTime, Space))
			{
				return true;
			}

			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (GetSocketTransform_RawID_Concurrent(OutSocketTransform, MeshID, SocketName, AnimationDeltaTime, Space))
				{
					return true;
				}
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Socket, Transform, IK, Item"))
	static bool GetSocketTransform_RawID_Concurrent(FTransform& OutSocketTransform,
	                                                const int64& MeshID,
	                                                const FName& SocketName,
	                                                const float& AnimationDeltaTime,
	                                                const EBoneSpaces::Type& Space = EBoneSpaces::WorldSpace)
	{
		OutSocketTransform = FTransform::Identity;

		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}
		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		if (!IsValid(Runtime.DataAsset))
		{
			return false;
		}
		const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		if (IsValid(Runtime.DataAsset) && IsValid(Runtime.DataAsset->ReferenceMeshNative) && Runtime.DataAsset->ReferenceMeshNative->FindSocket(SocketName))
		{
			const int64& CurrentFrameCount = UKismetSystemLibrary::GetFrameCount();

			FTurboSequence_Utility_Lf::GetSocketTransform(OutSocketTransform, SocketName, Runtime, Reference,
			                                              GlobalLibrary, GlobalLibrary_RenderThread, Space, AnimationDeltaTime, CurrentFrameCount,
			                                              Instance->GetThreadContext()->CriticalSection);

			return true;
		}

		return false;
	}

	/**
	 * Detects if the Mesh is visible this frame in the Camera Frustum
	 * @param MeshData The Mesh ID
	 * @return True if Visible in the Camera
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Is Visisble", Keywords="Turbo, Sequence, TS, Get, Visible, Camera, Frustum"))
	static bool GetIsMeshVisibleInCameraFrustum_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData)
	{
		if (MeshData.IsMeshDataValid())
		{
			if (GetIsMeshVisibleInCameraFrustum_RawID_Concurrent(MeshData.RootMotionMeshID))
			{
				return true;
			}
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (GetIsMeshVisibleInCameraFrustum_RawID_Concurrent(MeshID))
				{
					return true;
				}
			}
		}
		return false;
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Is Visisble", Keywords="Turbo, Sequence, TS, Get, Visible, Camera, Frustum"))
	static bool GetIsMeshVisibleInCameraFrustum_RawID_Concurrent(const int64& MeshID)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return GlobalLibrary.RuntimeSkinnedMeshes[MeshID].bIsVisible;
		}
		return false;
	}

	/**
	 * Gets the Reference Pose Transform of a bone in a given space by ref
	 * @param OutRefPoseTransform The Transform output of the Ref Pose with the given Parameters
	 * @param MeshData The Mesh ID
	 * @param BoneName The Name of the Bone to return the Ref Pose Transform
	 * @param MeshTransformSpace The Ref Pose Space
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Reference, Pose, Transform"))
	static bool GetReferencePoseTransform_Concurrent(FTransform& OutRefPoseTransform,
	                                                 const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                 const FName BoneName,
	                                                 const ETurboSequence_TransformSpace_Lf MeshTransformSpace = ETurboSequence_TransformSpace_Lf::WorldSpace)
	{
		if (MeshData.IsMeshDataValid())
		{
			if (GetReferencePoseTransform_RawID_Concurrent(OutRefPoseTransform, MeshData.RootMotionMeshID, BoneName, MeshTransformSpace))
			{
				return true;
			}
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (GetReferencePoseTransform_RawID_Concurrent(OutRefPoseTransform, MeshID, BoneName, MeshTransformSpace))
				{
					return true;
				}
			}
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Reference, Pose, Transform"))
	static bool GetReferencePoseTransform_RawID_Concurrent(FTransform& OutRefPoseTransform,
	                                                       const int64& MeshID,
	                                                       const FName& BoneName,
	                                                       const ETurboSequence_TransformSpace_Lf& MeshTransformSpace = ETurboSequence_TransformSpace_Lf::WorldSpace)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
			const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
			const FReferenceSkeleton& ReferenceSkeleton = FTurboSequence_Utility_Lf::GetReferenceSkeleton(Reference.DataAsset);
			if (const int32& BoneIndexFromName = FTurboSequence_Utility_Lf::GetSkeletonBoneIndex(ReferenceSkeleton, BoneName); FTurboSequence_Utility_Lf::GetSkeletonIsValidIndex(ReferenceSkeleton, BoneIndexFromName))
			{
				switch (MeshTransformSpace)
				{
				case ETurboSequence_TransformSpace_Lf::BoneSpace:

					OutRefPoseTransform = ReferenceSkeleton.GetRefBonePose()[BoneIndexFromName];
					return true;

				case ETurboSequence_TransformSpace_Lf::ComponentSpace:

					OutRefPoseTransform = Reference.ComponentSpaceRestPose[BoneIndexFromName];
					return true;

				case ETurboSequence_TransformSpace_Lf::WorldSpace:

					OutRefPoseTransform = Reference.ComponentSpaceRestPose[BoneIndexFromName] * FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime);
					return true;

				default:
					return false;
				}
			}
		}

		return false;
	}


	/**
	 * Gets the Number of GPU Bones from a Given LOD Index
	 * @param FromAsset From the Mesh Asset
	 * @param LodIndex The Lod Index of the LODs
	 * @return The Number of GPU Bones of this LOD
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Num GPU Bones", Keywords="Turbo, Sequence, TS, Get, Num, Bones"))
	static int32 GetNumOfGPUBones_Concurrent(const UTurboSequence_MeshAsset_Lf* FromAsset, const int32 LodIndex)
	{
		if (GlobalLibrary.PerReferenceData.Contains(FromAsset))
		{
			return GlobalLibrary.PerReferenceData[FromAsset].LevelOfDetails[LodIndex].CPUBoneToGPUBoneIndicesMap.Num();
		}
		return GET0_NUMBER;
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Mesh Asset", Keywords="Turbo, Sequence, TS, Get, Mesh, Asset"))
	static UTurboSequence_MeshAsset_Lf* GetMeshAsset_RawID_Concurrent(const int64& MeshID)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return GlobalLibrary.RuntimeSkinnedMeshes[MeshID].DataAsset;
		}
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Customize, Change, Mesh, Material"))
	static bool CustomizeMesh_RawID_GameThread(const int64& MeshID,
	                                           UTurboSequence_MeshAsset_Lf* TargetMesh,
	                                           const TArray<UMaterialInterface*>& TargetMaterials)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}

		FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		if (Runtime.DataAsset == TargetMesh && !TargetMaterials.Num())
		{
			return false;
		}


		TArray<TObjectPtr<UMaterialInterface>> Materials;
		Materials.Append(TargetMaterials);

		const uint32& MaterialHash = FTurboSequence_Helper_Lf::GetArrayHash(Materials);
		if (Runtime.DataAsset == TargetMesh && Runtime.MaterialsHash == MaterialHash) // It's the same
		{
			return false;
		}

		if (const bool& bMaterialChangeValid = Materials.Num() && Runtime.MaterialsHash != MaterialHash; !bMaterialChangeValid)
		{
			if (!TargetMesh->IsMeshAssetValid())
			{
				return false;
			}
		}

		if (GlobalLibrary.PerReferenceData.Contains(TargetMesh))
		{
			const FSkinnedMeshReference_Lf& PostReference = GlobalLibrary.PerReferenceData[TargetMesh];
			if (!IsValid(PostReference.FirstValidMeshLevelOfDetail))
			{
				return false;
			}
		}

		FTurboSequence_Utility_Lf::CustomizeMesh(Runtime, TargetMesh, Materials, Instance->NiagaraComponents, GlobalLibrary, GlobalLibrary_RenderThread, Instance->GetRootComponent(), Instance->GetThreadContext()->CriticalSection);

		return true;
	}

	/**
	 * Customizes the Mesh with a Target Spawn data
	 * @param MeshData The Mesh ID
	 * @param TargetMesh The Target Mesh Spawn Data, this is how the mesh should look finally
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Customize, Change, Mesh, Material"))
	static bool CustomizeMesh_GameThread(FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                     const FTurboSequence_MeshSpawnData_Lf TargetMesh)
	{
		if (!MeshData.IsMeshDataValid())
		{
			return false;
		}

		if (MeshData.CustomizableMeshIDs.Num() != TargetMesh.CustomizableMeshes.Num())
		{
			return false; // TODO: Add Management from Add and Remove instances here as well
		}


		bool bFinished = CustomizeMesh_RawID_GameThread(MeshData.RootMotionMeshID, TargetMesh.RootMotionMesh.Mesh, TargetMesh.RootMotionMesh.OverrideMaterials);

		const int16& NumCustomizable = TargetMesh.CustomizableMeshes.Num();
		for (int16 i = GET0_NUMBER; i < NumCustomizable; ++i)
		{
			if (CustomizeMesh_RawID_GameThread(MeshData.CustomizableMeshIDs[i], TargetMesh.CustomizableMeshes[i].Mesh, TargetMesh.CustomizableMeshes[i].OverrideMaterials))
			{
				bFinished = true;
			}
		}

		return bFinished;
	}

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Curve", Keywords="Turbo, Sequence, TS, Get, Animation, Curve"))
	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveValue_RawID_Concurrent(const int64& MeshID,
	                                                                               const FName& CurveName,
	                                                                               UAnimSequence* Animation)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return FTurboSequence_PoseCurveData_Lf();
		}

		const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		//const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		if (!GlobalLibrary.AnimationLibraryData.Contains(
			FTurboSequence_Utility_Lf::GetAnimationLibraryKey(Runtime.DataAsset->GetSkeleton(), Runtime.DataAsset, Animation)))
		{
			return FTurboSequence_PoseCurveData_Lf();
		}

		FAnimationMetaData_Lf AnimationMetaData;
		for (const FAnimationMetaData_Lf& MetaData : Runtime.AnimationMetaData)
		{
			if (MetaData.Animation == Animation)
			{
				AnimationMetaData = MetaData;
				break;
			}
		}

		if (!IsValid(AnimationMetaData.Animation))
		{
			return FTurboSequence_PoseCurveData_Lf();
		}

		return FTurboSequence_Utility_Lf::GetAnimationCurveByAnimation(Runtime, AnimationMetaData, GlobalLibrary, CurveName);
	}

	/**
	 * Gets the Animation Curve Data from a Curve Name and from the current state of the Mesh Animation solving
	 * @param MeshData The Mesh ID
	 * @param CurveName The Curve Name Defined in the Animation
	 * @param Animation The Animation of the Curve
	 * @return The Curve Data of the given frame of the current Animation
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Curve", Keywords="Turbo, Sequence, TS, Get, Animation, Curve"))
	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveValue_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                                         const FName CurveName,
	                                                                         UAnimSequence* Animation)
	{
		if (!MeshData.IsMeshDataValid())
		{
			return FTurboSequence_PoseCurveData_Lf();
		}

		return GetAnimationCurveValue_RawID_Concurrent(MeshData.RootMotionMeshID, CurveName, Animation);
	}

	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Footprint Asset", Keywords="Turbo, Sequence, TS, Get, Footpint, Asset"))
	static UTurboSequence_FootprintAsset_Lf* GetFootprintAsset_RawID_Concurrent(const int64& MeshID)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return GlobalLibrary.RuntimeSkinnedMeshes[MeshID].FootprintAsset;
		}
		return nullptr;
	}

	/**
	 * Gets the Footprint Asset from the Mesh
	 * @param MeshData The Mesh ID
	 * @return The Footprint Asset
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence", meta=(ReturnDisplayName="Footprint Asset", Keywords="Turbo, Sequence, TS, Get, Footpint, Asset"))
	static UTurboSequence_FootprintAsset_Lf* GetFootprintAsset_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData)
	{
		return GetFootprintAsset_RawID_Concurrent(MeshData.RootMotionMeshID);
	}

	/**
	 * Collecting garbage of data which is not possible to shrink dynamically because the data is too much,
	 * Stuff like:
	 * - Animation Keyframe Data stored in the GPU
	 *
	 * Sometimes it makes sense to Collect is every frame it can help boosting FPS
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Collect, Garbage, Clear"))
	static bool CollectGarbage()
	{
		return FTurboSequence_Utility_Lf::ClearAnimationsFromLibrary(GlobalLibrary, Instance->GetThreadContext()->CriticalSection, GlobalLibrary_RenderThread);
	}


	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Custom, Data, Rendering, Per Instance") )
	static bool SetCustomDataToInstance_RawID_Concurrent(const int64& MeshID, const uint8& CustomDataFractionIndex, const float& CustomDataValue)
	{
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			return false;
		}

		const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
		FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];

		return FTurboSequence_Utility_Lf::SetCustomDataForInstance_User(Reference, Runtime, CustomDataFractionIndex, CustomDataValue);
	}

	/**
	 * Sets Custom data for one Mesh which the material of this mesh can read
	 * @param MeshData The Mesh ID
	 * @param CustomDataFractionIndex The Custom Data Slice, NOTE: the first 4 Custom Data Items are reserved for the system and it only goes to max Index of 15 
	 * @param CustomDataValue The Value of the Custom Data as Float
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Custom, Data, Rendering, Per Instance") )
	static bool SetCustomDataToInstance_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData, const uint8 CustomDataFractionIndex, const float CustomDataValue)
	{
		if (!MeshData.IsMeshDataValid())
		{
			return false;
		}

		bool bSuccess = SetCustomDataToInstance_RawID_Concurrent(MeshData.RootMotionMeshID, CustomDataFractionIndex, CustomDataValue);
		for (const int64& MeshID : MeshData.CustomizableMeshIDs)
		{
			SetCustomDataToInstance_RawID_Concurrent(MeshID, CustomDataFractionIndex, CustomDataValue);
		}

		return bSuccess;
	}
};
