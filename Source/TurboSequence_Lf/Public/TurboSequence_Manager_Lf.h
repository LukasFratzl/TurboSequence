// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_Data_Lf.h"
#include "TurboSequence_FootprintAsset_Lf.h"
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

	friend class FTurboSequence_Utility_Lf;

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

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	// The Rendering Part needs Niagara to draw the meshes: 1 Draw Call per Mesh | 1 Draw Call per Material
	TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf> NiagaraComponents;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	// The Global Data which keeps track of internal Texture Data
	TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData;

	// The Map with the footprint assets in the simulation
	// < Asset | Number of Meshes which use it >
	inline static TMap<TObjectPtr<UTurboSequence_FootprintAsset_Lf>, int32> FootprintAssetsInUse;

protected:
	UPROPERTY(EditAnywhere, Category="TurboSequence")
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
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "Make sure to refresh the Thread Context in the main thread..., to fix this call the Game Thread Functions in the Game Thread..."
			       ));
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
	//bool bCollectGarbageThisFrame = false;


	/**
	 * Adds a Skinned Mesh Instance in a World 
	 * @param FromSpawnData The Spawn Data to have info about what to spawn
	 * @param SpawnTransform The Transform where to spawn
	 * @param InWorld In which world
	 * @return The Minimal Mesh Data which is the Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Instance Spawn Data", Keywords="Turbo, Sequence, TS, Add, Create, Instance, Spawn"))
	static FTurboSequence_MinimalMeshData_Lf AddSkinnedMeshInstance_GameThread(
		const FTurboSequence_MeshSpawnData_Lf FromSpawnData,
		const FTransform SpawnTransform,
		UWorld* InWorld);

	/**
	 * Removes a Skinned Mesh Instance from a World
	 * @param MeshData The Mesh IDs to understand which mesh
	 * @param InWorld In which world we remove the instance
	 * @return true if the function is successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Remove, Destroy, Instance, Despawn"))
	static bool RemoveSkinnedMeshInstance_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                 UWorld* InWorld);

protected:
	// NODE: Please use the versions with FTurboSequence_MeshSpawnData_Lf& FromSpawnData
	static int32 AddSkinnedMeshInstance_GameThread(const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset,
	                                               const FTransform& SpawnTransform,
	                                               const TObjectPtr<UWorld>& InWorld,
	                                               const TArray<TObjectPtr<UMaterialInterface>>& OverrideMaterials =
		                                               TArray<TObjectPtr<UMaterialInterface>>(),
	                                               const TObjectPtr<UTurboSequence_FootprintAsset_Lf>& FootprintAsset =
		                                               nullptr,
		                                               const int32 OverrideMeshID = INDEX_NONE);

	static bool RemoveSkinnedMeshInstance_GameThread(int32 MeshID, const TObjectPtr<UWorld> InWorld);

public:
	/**
	 * Solves all Meshes of one Update Group
	 * @param DeltaTime The Delta Time because the function should run every frame or every Update Group Tick
	 * @param InWorld In Which world we like to solve the Meshes
	 * @param UpdateContext The Update Context to have info like which update group etc.
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Animation, Mesh, Motion, Transform, Solve, Update, Render, Animate"))
	static void SolveMeshes_GameThread(float DeltaTime, UWorld* InWorld, FTurboSequence_UpdateContext_Lf UpdateContext);

protected:
	static void SolveMeshes_RenderThread(FRHICommandListImmediate& RHICmdList);

public:
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Add, Update, Mesh, Group"))
	static void AddInstanceToUpdateGroup_RawID_Concurrent(const int32 GroupIndex, int32 MeshID);

	/**
	 * Adds a Mesh to the Update Group with the given Index
	 * @param GroupIndex The Update Group Index, it will automatic increase the 2D Array to match the 1st Dimension with this index
	 * @param MeshData The Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Add, Update, Mesh, Group"))
	static void AddInstanceToUpdateGroup_Concurrent(const int32 GroupIndex,
	                                                const FTurboSequence_MinimalMeshData_Lf& MeshData);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Remove, Update, Mesh, Group"))
	static void RemoveInstanceFromUpdateGroup_RawID_Concurrent(const int32 GroupIndex, int32 MeshID);


	/**
	 * Removes an Instance in the Update Group
	 * @param GroupIndex The Group Index which hosting the Mesh ID
	 * @param MeshData The Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Remove, Update, Mesh, Group"))
	static void RemoveInstanceFromUpdateGroup_Concurrent(const int32 GroupIndex,
	                                                     const FTurboSequence_MinimalMeshData_Lf& MeshData);

	/**
	 * Get the Number of Mesh Data in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @return The Number of Mesh Collections in the Update Group
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Number, Num, Update, Mesh, Group"))
	static int32 GetNumMeshDataInUpdateGroup_Concurrent(const int32 GroupIndex);

	/**
	 * Get the Number of Mesh IDs in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @return The Number of Mesh IDs in the Update Group
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Number, Num, Update, Mesh, Group"))
	static int32 GetNumMeshIDsInUpdateGroup_RawID_Concurrent(const int32 GroupIndex);

	/**
	 * Get the Mesh Data in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @param IndexInGroup The Index in the Mesh Data Group
	 * @return The Mesh Data
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Get"))
	static FTurboSequence_MinimalMeshData_Lf GetMeshDataInUpdateGroupFromIndex_Concurrent(
		const int32 GroupIndex, const int32 IndexInGroup);

	/**
	 * Get the Mesh ID in a Update Group with the given Index, useful if you need iterate
	 * @param GroupIndex The Group Index
	 * @param IndexInGroup The Index in the Mesh ID Group
	 * @return The Mesh ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Get"))
	static int32 GetMeshIDInUpdateGroupFromIndex_Concurrent(const int32 GroupIndex, const int32 IndexInGroup);


	/**
	 * Check if the given Mesh Data is present in the specified Update Group concurrently.
	 *
	 * @param MeshData The Mesh Data to check for in the Update Group
	 * @param GroupIndex The index of the Update Group to search in
	 *
	 * @return true if the Mesh Data is found in the Update Group, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Contains"))
	static bool ContainsMeshDataInUpdateGroup_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                     const int32 GroupIndex);

	/**
	 * Checks if the given Mesh ID is present in the specified Update Group concurrently.
	 *
	 * @param MeshID The ID of the Mesh to check for in the Update Group
	 * @param GroupIndex The index of the Update Group to search in
	 *
	 * @return true if the Mesh ID is found in the Update Group, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Contains"))
	static bool ContainsMeshIDInUpdateGroup_Concurrent(const int32 MeshID,
	                                                   const int32 GroupIndex);

	/**
 * Checks if the given Mesh ID is present in the specified Update Group concurrently and returns the index.
 *
 * @param MeshID The ID of the Mesh to check for in the Update Group
 *
 * @return the Update group index, otherwise on error -1
 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Get"))
	static int32 GetUpdateGroupIndexFromMeshID_Concurrent(const int32 MeshID);

	/**
 * Gets the Mesh Data from a Mesh ID
 * @param MeshID The Group ID
 * @return The Mesh Data
 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Update, Mesh, Group, Get"))
	static FTurboSequence_MinimalMeshData_Lf GetMeshDataFromMeshID_Concurrent(const int32 MeshID);


	/**
	 * Clean the Manager, NOTE: The manager is auto manage itself, you don't need to implement this function
	 * @param bIsEndPlay Is end of the play session or map change
	 */
	static void CleanManager_GameThread(bool bIsEndPlay);

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
		FootprintAssetsInUse.Empty();
	}

	/**
	 * Get the Mesh World Space Transform
	 * @param MeshData The Mesh ID
	 * @return The World Space Transform of the Mesh Instance
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="World Space Transform", Keywords="Turbo, Sequence, TS, Get, Transform, World, Mesh"))
	static FTransform GetMeshWorldSpaceTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData);

	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="World Space Transform", Keywords="Turbo, Sequence, TS, Get, Transform, World, Mesh"))
	static FTransform GetMeshWorldSpaceTransform_RawID_Concurrent(int32 MeshID);

	/**
	 * Get the Mesh to Camera Distance
	 * @param MeshData The Mesh ID
	 * @return The Closest Distance to the Closest Camera
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Camera Distance", Keywords="Turbo, Sequence, TS, Get, Distance, Camera, Mesh"))
	static float GetMeshClosestCameraDistance_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData);

	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Camera Distance", Keywords="Turbo, Sequence, TS, Get, Distance, Camera, Mesh"))
	static float GetMeshClosestCameraDistance_RawID_Concurrent(int32 MeshID);

	/**
	 * Setting ( Location, Rotation, Scale ) to the World Space Mesh Instance Transform
	 * @param MeshData The Mesh ID
	 * @param Location The Location to set
	 * @param Rotation The Rotation to set
	 * @param Scale The Scale to set
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceLocationRotationScale_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                              const FVector Location, const FQuat Rotation,
	                                                              const FVector Scale, const bool bForce = false);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceLocationRotationScale_RawID_Concurrent(int32 MeshID, const FVector Location,
	                                                                    const FQuat Rotation, const FVector Scale,
	                                                                    const bool bForce = false);

	/**
	 * Set a Transform to the World Space Mesh Instance Transform
	 * @param MeshData The Mesh ID
	 * @param Transform The Transform to set
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                  const FTransform Transform, const bool bForce = false);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Mesh, Motion, Transform"))
	static void SetMeshWorldSpaceTransform_RawID_Concurrent(int32 MeshID, const FTransform Transform,
	                                                        const bool bForce = false);

	/**
	 * Gets the Root Motion Transforms based on all animation which are playing, accurate blended with the Animation Weights
	 * @param OutRootMotion The Returning Root Motion Transform
	 * @param MeshData The Mesh ID
	 * @param DeltaTime The Delta Time of the Group Delta Time or the Frame Delta Time, because the Root Motion Extraction needs DeltaTime for defining the Scalar Transform
	 * @param Space The Space of the Extraction
	 * @return True if the Function is Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static bool GetRootMotionTransform_Concurrent(FTransform& OutRootMotion,
	                                              const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                              float DeltaTime,
	                                              const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static bool GetRootMotionTransform_RawID_Concurrent(FTransform& OutRootMotion,
	                                                    int32 MeshID,
	                                                    float DeltaTime,
	                                                    const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	/**
	 * A Helper function to move the mesh in World Space with Root Motion
	 * @param MeshData The Mesh ID
	 * @param DeltaTime The Delta Time which the Root Motion needs to get extracted, Use your Group Delta Time or the Frame Delta Time
	 * @param bZeroZAxis Do you want Zero Z values in the Component Space Root Motion Transform
	 * @param bIncludeScale Do you want to include the Root Motion Scale3D Factor which will scale the mesh
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static void MoveMeshWithRootMotion_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                              float DeltaTime,
	                                              const bool bZeroZAxis = false,
	                                              const bool bIncludeScale = false);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(Keywords="Turbo, Sequence, TS, Set, Animation, Root, Motion, Transform"))
	static void MoveMeshWithRootMotion_RawID_Concurrent(int32 MeshID,
	                                                    float DeltaTime,
	                                                    bool ZeroZAxis = false,
	                                                    bool bIncludeScale = false);

	/**
	 * Get the Number of all meshes no matter which update group
	 * @return The Number of all instance meshes, no matter which Update Group
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Num Meshes", Keywords="Turbo, Sequence, TS, Get, Number, Instances"))
	static int32 GetNumOfAllMeshes_Concurrent();

	/**
	 * Playing an Animation
	 * @param MeshData Mesh ID
	 * @param Animation The Animation to Play
	 * @param AnimSettings The Animation Settings for playing the Animation
	 * @return The Animation ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Animation Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay"))
	static FTurboSequence_AnimMinimalCollection_Lf PlayAnimation_Concurrent(
		const FTurboSequence_MinimalMeshData_Lf& MeshData,
		UAnimSequence* Animation,
		const FTurboSequence_AnimPlaySettings_Lf& AnimSettings
	);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Animation Data", Keywords="Turbo, Sequence, TS, Set, Animation, PLay"))
	static FTurboSequence_AnimMinimalData_Lf PlayAnimation_RawID_Concurrent(int32 MeshID,
	                                                                        UAnimSequence* Animation,
	                                                                        const FTurboSequence_AnimPlaySettings_Lf&
	                                                                        AnimSettings
	);


	/**
	 * Playing an Blend Space
	 * @param MeshData The Mesh ID
	 * @param BlendSpace The Blend Space to play
	 * @param AnimSettings The Animation settings for this blend space
	 * @return The Blend Space ID
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Blend Space Data", Keywords=
			"Turbo, Sequence, TS, Set, Animation, PLay, Blend Space, Blend"))
	static FTurboSequence_AnimMinimalBlendSpaceCollection_Lf PlayBlendSpace_Concurrent(
		const FTurboSequence_MinimalMeshData_Lf& MeshData,
		UBlendSpace* BlendSpace,
		const FTurboSequence_AnimPlaySettings_Lf AnimSettings
	);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Blend Space Data", Keywords=
			"Turbo, Sequence, TS, Set, Animation, PLay, Blend Space, Blend"))
	static FTurboSequence_AnimMinimalBlendSpace_Lf PlayBlendSpace_RawID_Concurrent(int32 MeshID,
		UBlendSpace* BlendSpace,
		const FTurboSequence_AnimPlaySettings_Lf& AnimSettings
	);


	// /**
 // * Gets all Animations Playing right now on the character as an animation collection array
 // * @param OutAnimations The animation collection array output
 // * @param MeshData The Mesh ID
 // */
	// UFUNCTION(BlueprintPure, Category="Turbo Sequence",
	// 	meta=(ReturnDisplayName="Animation Sequence", Keywords="Turbo, Sequence, TS, Get, Animation, Play, Priority"))
	// static void GetAllAnimationsFromMeshData_Concurrent(
	// TArray<FTurboSequence_AnimMinimalCollection_Lf> OutAnimations,
	// 	const FTurboSequence_MinimalMeshData_Lf& MeshData);


	/**
	 * Gets the highest priority animation of a mesh
	 * @param MeshData The Mesh ID
	 * @return The highest priority animation
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Animation Sequence", Keywords="Turbo, Sequence, TS, Get, Animation, Play, Priority"))
	static UAnimSequence* GetHighestPriorityPlayingAnimation_Concurrent(
		const FTurboSequence_MinimalMeshData_Lf& MeshData);

	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Animation Sequence", Keywords="Turbo, Sequence, TS, Get, Animation, Play, Priority"))
	static UAnimSequence* GetHighestPriorityPlayingAnimation_RawID_Concurrent(int32 MeshID);

	/**
	 * Tweaks an Animation with new Settings
	 * @param TweakSettings The new Animation Settings
	 * @param AnimationData The Animation ID
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks"))
	static bool TweakAnimation_Concurrent(const FTurboSequence_AnimPlaySettings_Lf TweakSettings,
	                                      const FTurboSequence_AnimMinimalData_Lf& AnimationData);

	/**
	 * Tweaks an Animation Collection with new Settings
	 * @param TweakSettings The new Animation Settings
	 * @param AnimationData The Animation IDs
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks"))
	static bool TweakAnimationCollection_Concurrent(const FTurboSequence_AnimPlaySettings_Lf TweakSettings,
	                                                const FTurboSequence_AnimMinimalCollection_Lf& AnimationData);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks, Blend"))
	static bool TweakBlendSpace_RawID_Concurrent(int32 MeshID,
	                                             const FTurboSequence_AnimMinimalBlendSpace_Lf& BlendSpaceData,
	                                             const FVector3f& WantedPosition);

	/**
	 * Tweaks an Blend Space with a Grid Position
	 * @param BlendSpaceData The Blend Space ID
	 * @param WantedPosition The new Blend Space Grid Position
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Animation, Settings, Tweaks, Blend"))
	static bool TweakBlendSpace_Concurrent(const FTurboSequence_AnimMinimalBlendSpaceCollection_Lf& BlendSpaceData,
	                                       const FVector3f WantedPosition);

	/**
	 * Get the Current Animation Setting of an Animation as ref
	 * @param AnimationSettings Out Animation Setting
	 * @param AnimationData From Animation Data
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords=
			"Turbo, Sequence, TS, Get, Animation, Settings, Tweaks, Weight, Scale, Speed"))
	static bool GetAnimationSettings_Concurrent(FTurboSequence_AnimPlaySettings_Lf& AnimationSettings,
	                                            const FTurboSequence_AnimMinimalData_Lf& AnimationData);

	/**
	 * Gets the Current Animation Settings from an Animation Collection as Ref
	 * @param AnimationSettings Out Animation Settings Array
	 * @param AnimationData The Animation IDs
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords=
			"Turbo, Sequence, TS, Get, Animation, Settings, Tweaks, Weight, Scale, Speed"))
	static bool GetAnimationCollectionSettings_Concurrent(TArray<FTurboSequence_AnimPlaySettings_Lf>& AnimationSettings,
	                                                      const FTurboSequence_AnimMinimalCollection_Lf& AnimationData);

	static bool GetAnimationMetaData_Concurrent(FAnimationMetaData_Lf& AnimationMetaData,
	                                            const FTurboSequence_AnimMinimalData_Lf& AnimationData);

	static bool GetAnimationCollectionMetaData_Concurrent(TArray<FAnimationMetaData_Lf>& AnimationSettings,
	                                                      const FTurboSequence_AnimMinimalCollection_Lf& AnimationData);

	/**
	 * Get the IK Transform of a Bone by Ref
	 * @param OutIKTransform The IK Transform which getting created
	 * @param MeshData The Mesh ID
	 * @param BoneName The Bone Name of the IK bone
	 * @param AnimationDeltaTime Sometimes the Animations Update, so we need a DeltaTime in this case
	 * @param Space The Space of the IK Transform
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Transform, IK, Bone"))
	static bool GetIKTransform_Concurrent(FTransform& OutIKTransform,
	                                      const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                      const FName BoneName,
	                                      const float AnimationDeltaTime,
	                                      const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Transform, IK, Bone"))
	static bool GetIKTransform_RawID_Concurrent(FTransform& OutIKTransform,
	                                            int32 MeshID,
	                                            const FName& BoneName,
	                                            float AnimationDeltaTime,
	                                            const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	/**
	 * Sets the IK Transform from a Bone of a Mesh
	 * @param MeshData The Mesh ID
	 * @param BoneName The Bone Name of the Bone which is effected
	 * @param IKTransform The way the Bone should bend defined with the Transform
	 * @param Space The Space of the Transform
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Transform, IK, Bone"))
	static bool SetIKTransform_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                      const FName BoneName,
	                                      const FTransform IKTransform,
	                                      const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Transform, IK, Bone"))
	static bool SetIKTransform_RawID_Concurrent(int32 MeshID,
	                                            const FName& BoneName,
	                                            const FTransform& IKTransform,
	                                            const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);


	/**
	 * Gets the Socket Transform of a Mesh by Ref
	 * @param OutSocketTransform The Socket Transform which getting calculated
	 * @param MeshData The Mesh ID
	 * @param SocketName The Socket name from the Skeleton or Mesh Asset
	 * @param AnimationDeltaTime Sometimes the Animation is solving to define the Transform, so we need the DeltaTime
	 * @param Space The Space which the Transform getting calculated
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Socket, Transform, IK, Item"))
	static bool GetSocketTransform_Concurrent(FTransform& OutSocketTransform,
	                                          const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                          const FName SocketName,
	                                          const float AnimationDeltaTime,
	                                          const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Socket, Transform, IK, Item"))
	static bool GetSocketTransform_RawID_Concurrent(FTransform& OutSocketTransform,
	                                                int32 MeshID,
	                                                const FName& SocketName,
	                                                float AnimationDeltaTime,
	                                                const EBoneSpaces::Type Space = EBoneSpaces::WorldSpace);

	/**
	 * Detects if the Mesh is visible this frame in the Camera Frustum
	 * @param MeshData The Mesh ID
	 * @param bDetectBoundsCheckOnly if True -> The Check does not care if the mesh is Visible, it checks the bounds
	 * @return True if Visible in the Camera
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Is Visisble", Keywords="Turbo, Sequence, TS, Get, Visible, Camera, Frustum"))
	static bool GetIsMeshVisibleInCameraFrustum_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                       const bool bDetectBoundsCheckOnly);

	/**
 * Detects if the Mesh is visible this frame in the Camera Frustum
 * @param MeshID The Mesh ID
 * @param bDetectBoundsCheckOnly if True -> The Check does not care if the mesh is Visible, it checks the bounds
 * @return True if Visible in the Camera
 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Is Visisble", Keywords="Turbo, Sequence, TS, Get, Visible, Camera, Frustum"))
	static bool GetIsMeshVisibleInCameraFrustum_RawID_Concurrent(int32 MeshID, const bool bDetectBoundsCheckOnly)
	{
		if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
		{
			const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
			const FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
			return FTurboSequence_Utility_Lf::GetIsMeshVisible(Runtime, Reference, bDetectBoundsCheckOnly);
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
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Reference, Pose, Transform"))
	static bool GetReferencePoseTransform_Concurrent(FTransform& OutRefPoseTransform,
	                                                 const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                 const FName BoneName,
	                                                 const ETurboSequence_TransformSpace_Lf MeshTransformSpace =
		                                                 ETurboSequence_TransformSpace_Lf::WorldSpace);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Get, Animation, Reference, Pose, Transform"))
	static bool GetReferencePoseTransform_RawID_Concurrent(FTransform& OutRefPoseTransform,
	                                                       int32 MeshID,
	                                                       const FName& BoneName,
	                                                       const ETurboSequence_TransformSpace_Lf& MeshTransformSpace =
		                                                       ETurboSequence_TransformSpace_Lf::WorldSpace);


	/**
	 * Gets the Number of GPU Bones from a Given LOD Index
	 * @param FromAsset From the Mesh Asset
	 * @param LodIndex The Lod Index of the LODs
	 * @return The Number of GPU Bones of this LOD
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Num GPU Bones", Keywords="Turbo, Sequence, TS, Get, Num, Bones"))
	static int32 GetNumOfGPUBones_Concurrent(const UTurboSequence_MeshAsset_Lf* FromAsset, const int32 LodIndex);

	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Mesh Asset", Keywords="Turbo, Sequence, TS, Get, Mesh, Asset"))
	static UTurboSequence_MeshAsset_Lf* GetMeshAsset_RawID_Concurrent(int32 MeshID);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Customize, Change, Mesh, Material"))
	static int32 CustomizeMesh_RawID_GameThread(int32 MeshID,
	                                           UTurboSequence_MeshAsset_Lf* TargetMesh,
	                                           const TArray<UMaterialInterface*>& TargetMaterials);

	/**
	 * Customizes the Mesh with a Target Spawn data
	 * @param MeshData The Mesh ID
	 * @param TargetMesh The Target Mesh Spawn Data, this is how the mesh should look finally
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Customize, Change, Mesh, Material"))
	static bool CustomizeMesh_GameThread(FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                     const FTurboSequence_MeshSpawnData_Lf TargetMesh);

	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Curve", Keywords="Turbo, Sequence, TS, Get, Animation, Curve"))
	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveValue_RawID_Concurrent(int32 MeshID,
		const FName& CurveName,
		UAnimSequence* Animation);

	/**
	 * Gets the Animation Curve Data from a Curve Name and from the current state of the Mesh Animation solving
	 * @param MeshData The Mesh ID
	 * @param CurveName The Curve Name Defined in the Animation
	 * @param Animation The Animation of the Curve
	 * @return The Curve Data of the given frame of the current Animation
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Curve", Keywords="Turbo, Sequence, TS, Get, Animation, Curve"))
	static FTurboSequence_PoseCurveData_Lf GetAnimationCurveValue_Concurrent(
		const FTurboSequence_MinimalMeshData_Lf& MeshData,
		const FName CurveName,
		UAnimSequence* Animation);

	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Footprint Asset", Keywords="Turbo, Sequence, TS, Get, Footpint, Asset"))
	static UTurboSequence_FootprintAsset_Lf* GetFootprintAsset_RawID_Concurrent(int32 MeshID);

	/**
	 * Gets the Footprint Asset from the Mesh
	 * @param MeshData The Mesh ID
	 * @return The Footprint Asset
	 */
	UFUNCTION(BlueprintPure, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Footprint Asset", Keywords="Turbo, Sequence, TS, Get, Footpint, Asset"))
	static UTurboSequence_FootprintAsset_Lf* GetFootprintAsset_Concurrent(
		const FTurboSequence_MinimalMeshData_Lf& MeshData);

	// /**
	//  * Collecting garbage of data which is not possible to shrink dynamically because the data is too much,
	//  * Stuff like:
	//  * - Animation Keyframe Data stored in the GPU
	//  *
	//  * Sometimes it makes sense to Collect is every frame it can help boosting FPS
	//  * @return True if Successful
	//  */
	// UFUNCTION(BlueprintCallable, Category="Turbo Sequence", meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Collect, Garbage, Clear"))
	// static bool CollectGarbage()
	// {
	// 	return FTurboSequence_Utility_Lf::ClearAnimationsFromLibrary(GlobalLibrary, Instance->GetThreadContext()->CriticalSection, GlobalLibrary_RenderThread);
	// }


	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Custom, Data, Rendering, Per Instance"))
	static bool SetCustomDataToInstance_RawID_Concurrent(int32 MeshID, uint8 CustomDataFractionIndex,
	                                                     float CustomDataValue);

	/**
	 * Sets Custom data for one Mesh which the material of this mesh can read
	 * @param MeshData The Mesh ID
	 * @param CustomDataFractionIndex The Custom Data Slice, NOTE: the first 4 Custom Data Items are reserved for the system and it only goes to max Index of 15 
	 * @param CustomDataValue The Value of the Custom Data as Float
	 * @return True if Successful
	 */
	UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
		meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, Custom, Data, Rendering, Per Instance"))
	static bool SetCustomDataToInstance_Concurrent(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                               const uint8 CustomDataFractionIndex, const float CustomDataValue);


	/**
	 * Sets the Turbo Sequence Mesh to an equallent UE Mesh using the Ts IK system, make sure the UE Mesh has the same skeleton
	 * @param TsMeshID The Mesh ID
	 * @param UEMesh The Unreal Engine Mesh Instance, SkeletalMesh or PoseableMesh
	 * @param BoneSetCallback
	 * @return Ture if Successful
	 */
	// UFUNCTION(BlueprintCallable, Category="Turbo Sequence",
	// 	meta=(ReturnDisplayName="Success", Keywords="Turbo, Sequence, TS, Set, IK, Bones, Transition, Fade, Lerp"))
	static bool SetTransitionTsMeshToUEMesh_Concurrent(const int32 TsMeshID, USkinnedMeshComponent* UEMesh,
	                                                   TFunction<void(const FName& BoneName, const int32 BoneIndex)>
	                                                   BoneSetCallback);
};
