// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "Animation/BlendSpace.h"
#include "Misc/Optional.h"

#include "TurboSequence_Data_Lf.generated.h"


/*	==============================================================================================================
												RENDERING
	==============================================================================================================	*/

USTRUCT()
struct TURBOSEQUENCE_LF_API FCameraView_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Aspect")
	FVector2f ViewportSize = FVector2f::ZeroVector;

	TOptional<EAspectRatioAxisConstraint> AspectRatioAxisConstraint;

	UPROPERTY(EditAnywhere, Category="Aspect")
	float Fov = GET60_NUMBER;

	UPROPERTY(EditAnywhere, Category="Aspect")
	float NearClipPlane = GET1_NUMBER;

	UPROPERTY(EditAnywhere, Category="Aspect")
	float FarClipPlane = GET1000_NUMBER;

	UPROPERTY(EditAnywhere, Category="Aspect")
	bool bIsPerspective = true;

	UPROPERTY(EditAnywhere, Category="Aspect")
	float OrthoWidth = GET10_NUMBER;

	UPROPERTY(EditAnywhere, Category="Transform")
	FTransform CameraTransform;

	FPlane Planes_Internal[GET6_NUMBER];
	FTransform InterpolatedCameraTransform_Internal;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FRenderData_Lf
{
	GENERATED_BODY()

	explicit FRenderData_Lf(const FName& EmitterName, const FName& ParticleIDName, const FName& PositionName, const FName& RotationName,
	                        const FName& ScaleName, const FString& MeshName, const FString& MaterialsName,
	                        const FName& LodName, const FName& CustomDataName, const FName& ParticleRemoveName)
		: EmitterName(EmitterName),
		  IDName(ParticleIDName),
		  PositionName(PositionName),
		  RotationName(RotationName),
		  ScaleName(ScaleName),
		  MeshName(MeshName),
		  MaterialsName(MaterialsName),
		  LodName(LodName),
		  CustomDataName(CustomDataName),
		  ParticleRemoveName(ParticleRemoveName)
	{
	}

	FRenderData_Lf()
	{
	}

	~FRenderData_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UNiagaraSystem> RenderReference = nullptr;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	// ID
	TMap<int32, int32> InstanceMap; // < MeshID | Renderer Instance Index >
	//TMap<int32, int32> InstanceMapCopy; // < MeshID | Renderer Instance Index > Needed to Keep the Indices correct
	//TArray<int32> MeshIDsToRemove;
	//TArray<int32> ParticleIDs; // < Unique ID > -> Used internally for Niagara finding the Index
	//TMap<int32, int32> ParticleIDMap; // < MeshID | UniqueID >

	// Transform
	TArray<FVector> ParticlePositions;
	TArray<FVector3f> ParticleScales;
	TArray<FVector4f> ParticleRotations;

	// Culling and Visibility
	TArray<uint8> ParticleLevelOfDetails; // Index 32 -> Not Visible
	//TArray<bool> ParticlesToRemove;
	// TArray<int32> ParticlesToRemoveIndices;

	// Custom Data
	TArray<float> ParticleCustomData;

	// Bounds Checking
	FVector MinBounds = FVector::ZeroVector;
	FVector MaxBounds = FVector::ZeroVector;

	bool bChangedCollectionSizeThisFrame = false;
	bool bChangedCollectionSizePreviousFrame = false;
	bool bChangedPositionCollectionThisFrame = false;
	bool bChangedRotationCollectionThisFrame = false;
	bool bChangedScaleCollectionThisFrame = false;
	bool bChangedLodCollectionThisFrame = false;
	bool bChangedCustomDataCollectionThisFrame = false;


private:
	FName EmitterName;
	FName IDName;
	FName PositionName;
	FName RotationName;
	FName ScaleName;
	FString MeshName;
	FString MaterialsName;
	FName LodName;
	FName CustomDataName;
	FName ParticleRemoveName;

	// int32 UniqueID = 0;

public:
	// FORCEINLINE int32 GetUniqueID() const
	// {
	// 	return UniqueID;
	// }
	//
	// FORCEINLINE void IncrementUniqueID()
	// {
	// 	UniqueID++;
	// }

	FORCEINLINE FName& GetEmitterName()
	{
		return EmitterName;
	}

	FORCEINLINE FName& GetPositionName()
	{
		return PositionName;
	}

	FORCEINLINE FName& GetParticleIDName()
	{
		return IDName;
	}

	FORCEINLINE FName& GetRotationName()
	{
		return RotationName;
	}

	FORCEINLINE FName& GetScaleName()
	{
		return ScaleName;
	}

	FORCEINLINE FString& GetMeshName()
	{
		return MeshName;
	}

	FORCEINLINE FString& GetMaterialsName()
	{
		return MaterialsName;
	}

	FORCEINLINE FName& GetLodName()
	{
		return LodName;
	}

	FORCEINLINE FName& GetCustomDataName()
	{
		return CustomDataName;
	}

	FORCEINLINE FName& GetParticleRemoveName()
	{
		return ParticleRemoveName;
	}
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FCPUAnimationPose_Lf
{
	GENERATED_BODY()

	int32 BelongsToKeyframe = GET0_NUMBER;

	FAnimPose_Lf Pose;

	//TArray<FVector4f> RawData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationLibraryDataAllocationItem_Lf
{
	GENERATED_BODY()

	FVector4f Data = FVector4f::Zero();

	int32 ColIndex = GET0_NUMBER;
	int32 RowIndex = GET0_NUMBER;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationLibraryData_Lf
{
	GENERATED_BODY()

	int32 MaxFrames = GET0_NUMBER;

	uint16 NumBones = GET0_NUMBER;

	// < Keyframe Index | Pose >
	TMap<int32, FCPUAnimationPose_Lf> AnimPoses;

	TArray<int32> KeyframesFilled;
	int32 IndexInCollection = GET0_NUMBER;

	TMap<FName, int16> BoneNameToAnimationBoneIndex;


	int32 AnimationDensity = GET0_NUMBER;

	FAnimPoseEvaluationOptions_Lf PoseOptions;

	bool bHasPoseData = false;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationMetaData_RenderThread_Lf
{
	GENERATED_BODY()

	FAnimationMetaData_RenderThread_Lf()
	{
	}

	~FAnimationMetaData_RenderThread_Lf()
	{
	}

	int32 GPUAnimationIndex_0 = GET0_NUMBER;

	uint16 FinalAnimationWeight = GET0_NUMBER;

	uint16 LayerMaskIndex = GET0_NUMBER;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationMetaData_Lf
{
	GENERATED_BODY()

	FAnimationMetaData_Lf()
	{
	}

	~FAnimationMetaData_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimSequence* Animation = nullptr;

	UPROPERTY(EditAnywhere, Category="Settings")
	FTurboSequence_AnimPlaySettings_Lf Settings;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float AnimationTime = GET0_NUMBER;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float FinalAnimationWeight = GET1_NUMBER;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float AnimationWeightTime = 0.25f;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float AnimationWeightStartTime = 0.25f;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float AnimationRemoveTime = 0.25f;

	UPROPERTY(EditAnywhere, Category="Current Frame")
	float AnimationRemoveStartTime = 0.25f;

	UPROPERTY(EditAnywhere, Category="Animation")
	bool bIsLoop = false;

	UPROPERTY(EditAnywhere, Category="Animation")
	bool bIsOldAnimation = false;

	UPROPERTY(EditAnywhere, Category="Animation")
	uint16 LayerMaskIndex = GET0_NUMBER;

	bool bNeedRebuildAnimationLayers = false;
	// < Hash >
	uint32 AnimationLayerHash = GET0_NUMBER;

	uint32 AnimationGroupLayerHash = GET0_NUMBER;

	FUintVector AnimationLibraryHash = FUintVector::ZeroValue;

	float AnimationNormalizedTime = GET0_NUMBER;
	float AnimationMaxPlayLength = GET1_NUMBER;
	int32 CurrentFrame = GET0_NUMBER;

	bool bIsRootBoneAnimation = false;
	bool bIsSelfManagedAnimation = false;

	int32 CPUAnimationIndex_0 = GET0_NUMBER;

	uint32 AnimationID = GET0_NUMBER;

	FORCEINLINE uint32 SetHash(uint32 SecurityIndex) const
	{
		uint32 Hash = GET0_NUMBER;
		Hash = HashCombine(Hash, GetTypeHash(FGuid::NewGuid()));
		Hash = HashCombine(Hash, GetTypeHash(SecurityIndex));
		return Hash;
	}

	FORCEINLINE void SetAnimationID(const TMap<uint32, int32>& InputCollection, int32 BelongsToMeshID)
	{
		uint32 SecurityNumber = FMath::RandRange(GET0_NUMBER, INT32_MAX);
		// Set the hash
		AnimationID = SetHash(SecurityNumber);
		// We don't want a 0 Hash because it's the Instance Return type when the function cancel unexpected
		// Here we check if the hash is unique, if not we run the loop again,
		// usually on the 1st try it already pass the check fine
		while (InputCollection.Contains(AnimationID) || (!InputCollection.Contains(AnimationID) && AnimationID <=
			GET0_NUMBER))
		{
			SecurityNumber = FMath::RandRange(GET0_NUMBER, INT32_MAX);
			SecurityNumber++;
			AnimationID = SetHash(SecurityNumber);
		}

		AnimationID = HashCombine(AnimationID, BelongsToMeshID);
	}

	FORCEINLINE void SetAnimationID(const uint32 ID)
	{
		AnimationID = ID;
	}
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationGroup_Lf
{
	GENERATED_BODY()

	int16 NumAnimationsInGroup = GET0_NUMBER;

	float TotalAnimWeightRuntime = GET0_NUMBER;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FIKBoneData_Lf
{
	GENERATED_BODY()

	FTransform IKWriteTransform = FTransform::Identity;
	bool bIsInUsingWriteDataThisFrame = false;
	uint8 AliveCount = GET0_NUMBER;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FRenderingMaterialKeyValue_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UMaterialInterface> MaterialKey;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UMaterialInterface> MaterialValue;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FRenderingMaterialItem_Lf
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<UNiagaraComponent> NiagaraRenderer;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FRenderingMaterialKeyValue_Lf> Materials;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FRenderingMaterialMap_Lf
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TMap<uint32, FRenderingMaterialItem_Lf> NiagaraRenderer;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationBlendSpaceData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	// < Animation ID | Sampler Index >
	TMap<uint32, int32> Points;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FVector3f CurrentPosition = FVector3f::ZeroVector;

	TArray<FBlendSampleData> CachedBlendSampleData;
	FBlendFilter BlendFilter;

	float LongestPlayLength = GET0_NUMBER;
	float CurrentTime = GET0_NUMBER;

	FAnimTickRecord Tick;
	FMarkerTickRecord Record;
	FAnimNotifyQueue NotifyQueue;
	FDeltaTimeRecord DeltaTimeRecord = FDeltaTimeRecord();
};

/*	==============================================================================================================
												REFERENCE
	==============================================================================================================	*/

USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshReferenceLodElement_Lf
{
	GENERATED_BODY()

	FSkinnedMeshReferenceLodElement_Lf()
	{
	}

	~FSkinnedMeshReferenceLodElement_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UStaticMesh> Mesh;

	uint32 SkinWeightOffset = GET0_NUMBER;
	uint8 MeshIndex = GET0_NUMBER;
	uint8 GPUMeshIndex = GET0_NUMBER;
	uint8 CollectionIndex = GET0_NUMBER;

	uint32 MinLodDistance = GET0_NUMBER;
	uint32 MaxLodDistance = GET0_NUMBER;

	bool bIsRenderStateValid = false;
	bool bIsAnimated = true;
	bool bIsFrustumCullingEnabled = true;

	// CPU Indices, GPU Indices > -> Only contains bones with skin weight and Lod with mesh
	TMap<uint16, uint16> CPUBoneToGPUBoneIndicesMap;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshReference_RenderThread_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshAsset_Lf> DataAsset;

	// CPU Indices, GPU Indices > -> Only contains bones with skin weight and Lod with mesh
	TMap<uint16, uint16> FirstLodGPUBonesCollection;

	int32 ReferenceCollectionIndex = GET0_NUMBER;

	// Since we have Async Chunked dynamic loading of all data we need to store skin weights locally
	// in the reference to async chunked load it into the GPU
	TArray<FVector4f> LocalMeshSkinWeights;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshReference_Lf : public FSkinnedMeshReference_RenderThread_Lf
{
	GENERATED_BODY()

	FSkinnedMeshReference_Lf()
	{
	}

	~FSkinnedMeshReference_Lf()
	{
	}

	explicit FSkinnedMeshReference_Lf(const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset)
	{
		DataAsset = Asset;
	}

	FORCEINLINE bool operator==(const FSkinnedMeshReference_Lf& Rhs) const
	{
		return this->DataAsset == Rhs.DataAsset;
	}

	FORCEINLINE bool operator!=(const FSkinnedMeshReference_Lf& Rhs) const
	{
		return !(*this == Rhs);
	}

	FORCEINLINE uint32 GetClassHash() const
	{
		return GetTypeHash(DataAsset);
	}

	TMap<uint8, FSkinnedMeshReferenceLodElement_Lf> LevelOfDetails;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UStaticMesh> FirstValidMeshLevelOfDetail;

	uint16 NumCPUBones = GET0_NUMBER;

	uint16 NumFirstLodGPUBones = GET0_NUMBER;
	uint8 NumLevelOfDetailsWithMesh = GET0_NUMBER;

	FTransform FirstReferenceBone;

	TArray<FTransform> ComponentSpaceRestPose;

	// < Material Hash | Data >
	TMap<uint32, FRenderData_Lf> RenderData;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UWorld> FromWorld = nullptr;
};

/*	==============================================================================================================
												RUNTIME
	==============================================================================================================	*/

USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshRuntime_RenderThread_Lf
{
	GENERATED_BODY()

	FSkinnedMeshRuntime_RenderThread_Lf()
	{
	}

	~FSkinnedMeshRuntime_RenderThread_Lf()
	{
	}

	explicit FSkinnedMeshRuntime_RenderThread_Lf(int32 WantedMeshID,
	                                             const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset)
	{
		MeshID = WantedMeshID;
		DataAsset = Asset;
	}

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshAsset_Lf> DataAsset;

	bool bIsVisible = true;
	bool bIsVisibleInFrustum = true;
	ETurboSequence_IsVisibleOverride_Lf EIsVisibleOverride = ETurboSequence_IsVisibleOverride_Lf::Default;
	ETurboSequence_IsAnimatedOverride_Lf EIsAnimatedOverride = ETurboSequence_IsAnimatedOverride_Lf::Default;
	uint8 CurrentGPUMeshIndex = GET0_NUMBER;

	TArray<FAnimationMetaData_RenderThread_Lf> AnimationMetaData_RenderThread;

	bool bIKDataInUse = false;
	TMap<uint16, FIKBoneData_Lf> IKData;

protected:
	int32 MeshID = GET0_NUMBER;

public:
	FORCEINLINE int32 GetMeshID() const
	{
		return MeshID;
	}
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshRuntime_Lf : public FSkinnedMeshRuntime_RenderThread_Lf
{
	GENERATED_BODY()

	FSkinnedMeshRuntime_Lf()
	{
	}

	~FSkinnedMeshRuntime_Lf()
	{
	}

	explicit FSkinnedMeshRuntime_Lf(const TMap<int32, FSkinnedMeshRuntime_Lf>& InputCollection,
	                                const TMap<int32, bool>& BlacklistedMeshIDs,
	                                const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
	                                const int32 OverrideMeshID = INDEX_NONE)
	{
		if (OverrideMeshID > INDEX_NONE && !InputCollection.Contains(OverrideMeshID) && !BlacklistedMeshIDs.Contains(OverrideMeshID))
		{
			MeshID = OverrideMeshID;
		}
		else
		{
			MeshID = FMath::RandRange(0, INT32_MAX - 1);
			MeshID++;
			while ((InputCollection.Contains(MeshID) || BlacklistedMeshIDs.Contains(MeshID)) || ((!InputCollection.Contains(MeshID) && !BlacklistedMeshIDs.Contains(MeshID)) && MeshID < GET0_NUMBER))
			{
				MeshID = FMath::RandRange(0, INT32_MAX - 1);
				MeshID++;
			}
		}

		DataAsset = Asset;
	}

	// uint32 SetHash(uint32 SecurityIndex) const
	// {
	// 	uint32 Hash = GET0_NUMBER;
	// 	Hash = HashCombine(Hash, GetTypeHash(FGuid::NewGuid()));
	// 	Hash = HashCombine(Hash, GetTypeHash(SecurityIndex));
	// 	return Hash;
	// }

	FORCEINLINE bool operator==(const FSkinnedMeshRuntime_Lf& Rhs) const
	{
		return this->MeshID == Rhs.MeshID;
	}

	FORCEINLINE bool operator!=(const FSkinnedMeshRuntime_Lf& Rhs) const
	{
		return !(*this == Rhs);
	}

	FTransform WorldSpaceTransform = FTransform::Identity;

	TArray<FAnimationMetaData_Lf> AnimationMetaData;
	// < Animation ID | Animation Index >
	TMap<uint32, int32> AnimationIDs;

	// < Animation Group Layer Hash | Animation Group >
	TMap<uint32, FAnimationGroup_Lf> AnimationGroups;

	TMap<TObjectPtr<UBlendSpace>, FAnimationBlendSpaceData_Lf> AnimationBlendSpaceMetaData;

	int16 LodIndex = GET0_NUMBER;
	bool bIsDistanceUpdatingThisFrame = true;

	float ClosestCameraDistance = GET0_NUMBER;
	float DeltaTimeAccumulator = GET0_NUMBER;

	uint32 MaterialsHash = GET0_NUMBER;

	bool bForceVisibilityUpdatingThisFrame = false;

	int64 LastFrameAnimationSolved = GET0_NUMBER;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<AActor> HybridMeshInstance;

	bool bSpawnedHybridActor = false;

	int32 UpdateGroupIndex = INDEX_NONE;
};


/*	==============================================================================================================
												GLOBAL
	==============================================================================================================	*/

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationBlendLayerMask_Lf
{
	GENERATED_BODY()

	// We need a way to track how many animations are using this animation layer
	int32 AnimationLayerCounter = GET0_NUMBER;

	// < Skeleton Layer of Max CPU Bones * 0xFF >
	TArray<uint16> RawAnimationLayers;

	// After Creating the Layer, We can Hash the Value
	// < Hash >
	uint32 AnimationLayerHash = GET0_NUMBER;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FAsyncTextureGenerationChunk_Lf
{
	GENERATED_BODY()

	TArray<FUintVector3> LodDimensions;

	int32 NumPixelsPerIteration = GET0_NUMBER;

	int32 NumPixelsComputed = GET0_NUMBER;

	TMap<int32, bool> TestValue;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshAsset_Lf> DataAsset;


	FORCEINLINE void Init(const TObjectPtr<UTextureRenderTarget2DArray> Tex, int32 MaxPixelIndex,
	                      int16 NumRowsFree, int16 Index)
	{
		int32 CeilRows = FMath::CeilToInt(
			static_cast<float>(LodDimensions[Index].X + MaxPixelIndex) / static_cast<float>(Tex->SizeY)) + NumRowsFree;
		int32 ChunkSizeY = CeilRows * Tex->SizeX;

		LodDimensions[Index].Z = CeilRows;

		LodDimensions[Index].Y = ChunkSizeY;
	}
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshGlobalLibrary_RenderThread_Lf
{
	GENERATED_BODY()

	FSkinnedMeshGlobalLibrary_RenderThread_Lf()
	{
		SkinWeightParams = FSettingsComputeShader_Params_Lf();
		BoneTransformParams = FMeshUnitComputeShader_Params_Lf();
		AnimationLibraryParams = FSettingsComputeShader_Params_Lf();
	}

	~FSkinnedMeshGlobalLibrary_RenderThread_Lf()
	{
	}


	FSettingsComputeShader_Params_Lf SkinWeightParams;
	FMeshUnitComputeShader_Params_Lf BoneTransformParams;

	FSettingsComputeShader_Params_Lf AnimationLibraryParams;

	uint32 NumMeshesVisibleCurrentFrame = GET0_NUMBER;
	uint32 NumIKPixelCurrentFrame = GET0_NUMBER;
	uint32 NumAnimationsCurrentFrame = GET0_NUMBER;

	TMap<int32, FSkinnedMeshRuntime_RenderThread_Lf> RuntimeSkinnedMeshes;
	TArray<int32> RuntimeSkinnedMeshesHashMap;

	uint32 AnimationLibraryMaxNum = GET0_NUMBER;

	// < MeshID | Index >
	TMap<int32, int32> MeshIDToGlobalIndex;

	TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_RenderThread_Lf> PerReferenceData;
	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<TObjectPtr<UTurboSequence_MeshAsset_Lf>> PerReferenceDataKeys;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FSkinnedMeshGlobalLibrary_Lf
{
	GENERATED_BODY()

	FSkinnedMeshGlobalLibrary_Lf()
	{
	}

	~FSkinnedMeshGlobalLibrary_Lf()
	{
	}

	TMap<int32, FSkinnedMeshRuntime_Lf> RuntimeSkinnedMeshes;
	TArray<int32> RuntimeSkinnedMeshesHashMap;
	TMap<int32, bool> BlackListedMeshIDs;

	// < MeshID | Data >
	TMap<int32, FTurboSequence_MinimalMeshData_Lf> MeshIDToMinimalData;

	TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_Lf> PerReferenceData;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<TObjectPtr<UTurboSequence_MeshAsset_Lf>> PerReferenceDataKeys;

	// -> Key -> < USkeleton | UTurboSequence_MeshAsset_Lf | UAnimSequence >
	TMap<FUintVector, FAnimationLibraryData_Lf> AnimationLibraryData;
	uint32 AnimationLibraryMaxNum = GET0_NUMBER;
	TArray<FVector4f> AnimationLibraryDataAllocatedThisFrame;
	// // Sum of -> ( Values * Library Hash, Mesh Bones ) is the Keyframe index
	// // We need this construct to easy determinate the index when we remove an animation from the GPU
	// // We need an alpha type to copy the new Library Data over
	// TMap<FUintVector, int32> AnimationLibraryAlphaKeyframeDimensions;

	TArray<FCameraView_Lf> CameraViews;

	TArray<FAnimationBlendLayerMask_Lf> AnimationBlendLayerMasks;
	// Contains the Runtime ID which is having dirty Animation Data
	TMap<FUintVector2, bool> AnimationBlendLayerMasksRuntimeDirty;

	uint16 MaxNumCPUBones = GET0_NUMBER;
	uint16 MaxNumGPUBones = GET0_NUMBER;
	uint16 MaxNumLevelOfDetailsWithMesh = GET0_NUMBER;
	bool bMaxNumCPUBonesChanged = false;

	TArray<FTurboSequence_UpdateGroup_Lf> UpdateGroups;

	// < MeshID | Index >
	TMap<int32, int32> MeshIDToGlobalIndex;

	int16 NumGroupsUpdatedThisFrame = GET0_NUMBER;
};
