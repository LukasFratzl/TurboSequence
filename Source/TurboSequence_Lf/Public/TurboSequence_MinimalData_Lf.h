// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "UObject/Object.h"

#include "TurboSequence_MinimalData_Lf.generated.h"

/*	==============================================================================================================
												DATA
	==============================================================================================================	*/

UENUM(BlueprintType)
enum class ETurboSequence_AnimationForceMode_Lf : uint8
{
	None,
	PerLayer,
	AllLayers
};

UENUM(BlueprintType)
enum class ETurboSequence_RootMotionMode_Lf : uint8
{
	None,
	OnRootBoneAnimated,
	Force
};

UENUM(BlueprintType)
enum class ETurboSequence_ManagementMode_Lf : uint8
{
	Auto,
	SelfManaged
};

UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_ThreadContext_Lf : public UObject
{
	GENERATED_BODY()

public:
	FCriticalSection CriticalSection;

	UFUNCTION(BlueprintCallable)
	void LockThread()
	{
		CriticalSection.Lock();
	}

	UFUNCTION(BlueprintCallable)
	void UnlockThread()
	{
		CriticalSection.Unlock();
	}
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_BoneLayer_Lf
{
	GENERATED_BODY()

	FTurboSequence_BoneLayer_Lf()
	{
	}

	~FTurboSequence_BoneLayer_Lf()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneLayerName = FName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 BoneDepth = GET2_NUMBER;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimPlaySettings_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimPlaySettings_Lf()
	{
	}

	~FTurboSequence_AnimPlaySettings_Lf()
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTurboSequence_BoneLayer_Lf> BoneLayerMasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAnimationTimeSelfManaged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationPlayTimeInSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AnimationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETurboSequence_AnimationForceMode_Lf ForceMode = ETurboSequence_AnimationForceMode_Lf::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTransitionTimeInSeconds = GET1_NUMBER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndTransitionTimeInSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETurboSequence_RootMotionMode_Lf RootMotionMode = ETurboSequence_RootMotionMode_Lf::OnRootBoneAnimated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ETurboSequence_ManagementMode_Lf AnimationManagementMode = ETurboSequence_ManagementMode_Lf::Auto;
};

//
USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimMinimalData_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimMinimalData_Lf()
	{
	}

	explicit FTurboSequence_AnimMinimalData_Lf(const bool& bValid)
	{
		bIsValid = bValid;
	}

	~FTurboSequence_AnimMinimalData_Lf()
	{
	}

	uint32 AnimationID = GET0_NUMBER;
	uint32 BelongsToMeshID = GET0_NUMBER;

protected:
	bool bIsValid = false;

public:
	bool IsAnimationValid() const
	{
		return bIsValid;
	}
};


USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimMinimalCollection_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimMinimalCollection_Lf()
	{
	}

	~FTurboSequence_AnimMinimalCollection_Lf()
	{
	}

	explicit FTurboSequence_AnimMinimalCollection_Lf(const bool& bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTurboSequence_AnimMinimalData_Lf RootMotionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTurboSequence_AnimMinimalData_Lf> CustomizableMeshes;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsValid = false;

public:
	const bool& IsAnimCollectionValid() const
	{
		return bIsValid;
	}
};


USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimMinimalBlendSpace_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimMinimalBlendSpace_Lf()
	{
	}

	~FTurboSequence_AnimMinimalBlendSpace_Lf()
	{
	}

	explicit FTurboSequence_AnimMinimalBlendSpace_Lf(const bool& bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(EditAnywhere)
	TArray<uint32> Samples;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlendSpace> BlendSpace;

	UPROPERTY(EditAnywhere)
	uint32 BelongsToMeshID = GET0_NUMBER;

protected:
	UPROPERTY(VisibleAnywhere)
	bool bIsValid = false;

public:
	const bool& IsAnimBlendSpaceValid() const
	{
		return bIsValid;
	}
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimMinimalBlendSpaceCollection_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimMinimalBlendSpaceCollection_Lf()
	{
	}

	~FTurboSequence_AnimMinimalBlendSpaceCollection_Lf()
	{
	}

	explicit FTurboSequence_AnimMinimalBlendSpaceCollection_Lf(const bool& bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTurboSequence_AnimMinimalBlendSpace_Lf RootMotionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTurboSequence_AnimMinimalBlendSpace_Lf> CustomizableMeshes;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsValid = false;

public:
	const bool& IsAnimCollectionValid() const
	{
		return bIsValid;
	}
};


USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FMeshMetaData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTurboSequence_MeshAsset_Lf> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TObjectPtr<UMaterialInterface>> OverrideMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UTurboSequence_FootprintAsset_Lf> FootprintAsset;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_MeshSpawnData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeshMetaData_Lf RootMotionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FMeshMetaData_Lf> CustomizableMeshes;

	uint32 GetHash() const // TODO: Make GET and SET to assign the the hash only when values are changing
	{
		uint32 Hash = GET0_NUMBER;

		Hash = HashCombineFast(GetTypeHash(RootMotionMesh.Mesh), Hash);
		for (const TObjectPtr<UMaterialInterface>& OverrideMaterial : RootMotionMesh.OverrideMaterials)
		{
			Hash = HashCombineFast(GetTypeHash(OverrideMaterial), Hash);
		}
		Hash = HashCombineFast(GetTypeHash(RootMotionMesh.FootprintAsset), Hash);

		for (const FMeshMetaData_Lf& CustomizableMeshMeta : CustomizableMeshes)
		{
			Hash = HashCombineFast(GetTypeHash(CustomizableMeshMeta.Mesh), Hash);
			for (const TObjectPtr<UMaterialInterface>& OverrideMaterial : CustomizableMeshMeta.OverrideMaterials)
			{
				Hash = HashCombineFast(GetTypeHash(OverrideMaterial), Hash);
			}
			Hash = HashCombineFast(GetTypeHash(CustomizableMeshMeta.FootprintAsset), Hash);
		}

		return Hash;
	}

	bool operator==(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return !Equals(Other);
	}

	bool Equals(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return GetHash() == Other.GetHash();
	}

	bool IsSpawnDataValid() const
	{
		return IsValid(RootMotionMesh.Mesh);
	}
};
#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FTurboSequence_MeshSpawnData_Lf& MetaData);
#else // optimize by inlining in shipping and development builds
FORCEINLINE_DEBUGGABLE uint32 GetTypeHash(const FTurboSequence_MeshSpawnData_Lf& MetaData)
{
	return MetaData.GetHash();
}
#endif

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_MinimalMeshData_Lf
{
	GENERATED_BODY()

	FTurboSequence_MinimalMeshData_Lf()
	{
	}

	~FTurboSequence_MinimalMeshData_Lf()
	{
	}

	explicit FTurboSequence_MinimalMeshData_Lf(const bool& bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int64 RootMotionMeshID = GET0_NUMBER;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int64> CustomizableMeshIDs;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsValid = false;

public:
	const bool& IsMeshDataValid() const
	{
		return bIsValid;
	}

	bool operator==(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return Equals(Other);
	}

	bool operator!=(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return !Equals(Other);
	}

	bool Equals(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return RootMotionMeshID == Other.RootMotionMeshID;
	}

	FString ToString() const
	{
		FString String = FString::Printf(TEXT("bIsValid -> %s | Root Mesh ID -> %lld"), IsMeshDataValid() ? TEXT("True") : TEXT("False"), RootMotionMeshID);
		for (const int64& MeshID : CustomizableMeshIDs)
		{
			String += FString::Printf(TEXT(" | Customizable ID -> %lld"), MeshID);
		}
		return String;
	}
};
#if UE_BUILD_DEBUG
uint32 GetTypeHash(const FTurboSequence_MinimalMeshData_Lf& MeshData);
#else // optimize by inlining in shipping and development builds
FORCEINLINE_DEBUGGABLE uint32 GetTypeHash(const FTurboSequence_MinimalMeshData_Lf& MeshData)
{
	return MeshData.RootMotionMeshID;
}
#endif


USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_PoseCurveData_Lf
{
	GENERATED_BODY()

	FTurboSequence_PoseCurveData_Lf()
	{
	}

	FTurboSequence_PoseCurveData_Lf(const TObjectPtr<UAnimSequence>& CurveAnimation, const FName& CurveID, const float& CurveFrame0)
		: CurveAnimation(CurveAnimation),
		  CurveName(CurveID),
		  CurveFrame_0(CurveFrame0)
	{
	}

	~FTurboSequence_PoseCurveData_Lf()
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimSequence> CurveAnimation = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName CurveName = FName("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurveFrame_0 = GET0_NUMBER;

	bool IsCurveValid() const
	{
		return IsValid(CurveAnimation);
	}
};


UENUM(BlueprintType)
enum class ETurboSequence_TransformSpace_Lf : uint8
{
	BoneSpace,
	ComponentSpace,
	WorldSpace
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_UpdateGroup_Lf
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<uint32> RawIDs;

	UPROPERTY(VisibleAnywhere)
	TMap<uint32, int32> RawIDData;

	UPROPERTY(VisibleAnywhere)
	TMap<FTurboSequence_MinimalMeshData_Lf, FIntVector2> MeshIDToMinimal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FTurboSequence_MinimalMeshData_Lf> RawMinimalData;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_UpdateContext_Lf
{
	GENERATED_BODY()

	FTurboSequence_UpdateContext_Lf()
	{
		//ThreadContext = NewObject<UTurboSequence_ThreadContext_Lf>();
	}

	~FTurboSequence_UpdateContext_Lf()
	{
	}

	FTurboSequence_UpdateContext_Lf(const int32 GroupIndex)
		: GroupIndex(GroupIndex)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GroupIndex = GET0_NUMBER;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//bool bCollectGarbageThisFrame = true;
};
