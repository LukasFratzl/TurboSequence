// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

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

UENUM(BlueprintType)
enum class ETurboSequence_IsVisibleOverride_Lf : uint8
{
	Default,
	IsVisible,
	IsNotVisible,
	ScaleToZero
};

UENUM(BlueprintType)
enum class ETurboSequence_IsAnimatedOverride_Lf : uint8
{
	Default,
	IsAnimated,
	IsNotAnimated
};

UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_ThreadContext_Lf : public UObject
{
	GENERATED_BODY()

public:
	FCriticalSection CriticalSection;

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void LockThread()
	{
		CriticalSection.Lock();
	}

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	FName BoneLayerName = FName();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TArray<FTurboSequence_BoneLayer_Lf> BoneLayerMasks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	bool bAnimationTimeSelfManaged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float AnimationWeight = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float AnimationPlayTimeInSeconds = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float AnimationSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	ETurboSequence_AnimationForceMode_Lf ForceMode = ETurboSequence_AnimationForceMode_Lf::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float StartTransitionTimeInSeconds = GET1_NUMBER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float EndTransitionTimeInSeconds = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	ETurboSequence_RootMotionMode_Lf RootMotionMode = ETurboSequence_RootMotionMode_Lf::OnRootBoneAnimated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	ETurboSequence_ManagementMode_Lf AnimationManagementMode = ETurboSequence_ManagementMode_Lf::Auto;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float OverrideWeight = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float OverrideStartTime = -1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float OverrideEndTime = -1.0f;
};

//
USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_AnimMinimalData_Lf
{
	GENERATED_BODY()

	FTurboSequence_AnimMinimalData_Lf()
	{
	}

	explicit FTurboSequence_AnimMinimalData_Lf(bool bValid)
	{
		bIsValid = bValid;
	}

	~FTurboSequence_AnimMinimalData_Lf()
	{
	}

	uint32 AnimationID = GET0_NUMBER;
	int32 BelongsToMeshID = GET0_NUMBER;

protected:
	bool bIsValid = false;

public:
	FORCEINLINE bool IsAnimationValid() const
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

	explicit FTurboSequence_AnimMinimalCollection_Lf(bool bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	FTurboSequence_AnimMinimalData_Lf RootMotionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	TArray<FTurboSequence_AnimMinimalData_Lf> CustomizableMeshes;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	bool bIsValid = false;

public:
	FORCEINLINE bool IsAnimCollectionValid() const
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

	explicit FTurboSequence_AnimMinimalBlendSpace_Lf(bool bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<uint32> Samples;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UBlendSpace> BlendSpace;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	int32 BelongsToMeshID = GET0_NUMBER;

protected:
	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	bool bIsValid = false;

public:
	FORCEINLINE bool IsAnimBlendSpaceValid() const
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

	explicit FTurboSequence_AnimMinimalBlendSpaceCollection_Lf(bool bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	FTurboSequence_AnimMinimalBlendSpace_Lf RootMotionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	TArray<FTurboSequence_AnimMinimalBlendSpace_Lf> CustomizableMeshes;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	bool bIsValid = false;

public:
	FORCEINLINE bool IsAnimCollectionValid() const
	{
		return bIsValid;
	}
};


USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_MeshMetaData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshAsset_Lf> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TArray<TObjectPtr<UMaterialInterface>> OverrideMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TObjectPtr<class UTurboSequence_FootprintAsset_Lf> FootprintAsset;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_MeshSpawnData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	FTurboSequence_MeshMetaData_Lf RootMotionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TArray<FTurboSequence_MeshMetaData_Lf> CustomizableMeshes;

	uint32 GetHash() const // TODO: Make GET and SET to assign the the hash only when values are changing
	{
		uint32 Hash = GET0_NUMBER;

		Hash = HashCombineFast(GetTypeHash(RootMotionMesh.Mesh), Hash);
		for (const TObjectPtr<UMaterialInterface>& OverrideMaterial : RootMotionMesh.OverrideMaterials)
		{
			Hash = HashCombineFast(GetTypeHash(OverrideMaterial), Hash);
		}
		Hash = HashCombineFast(GetTypeHash(RootMotionMesh.FootprintAsset), Hash);

		for (const FTurboSequence_MeshMetaData_Lf& CustomizableMeshMeta : CustomizableMeshes)
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

	FORCEINLINE bool operator==(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return Equals(Other);
	}

	FORCEINLINE bool operator!=(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return !Equals(Other);
	}

	FORCEINLINE bool Equals(const FTurboSequence_MeshSpawnData_Lf& Other) const
	{
		return GetHash() == Other.GetHash();
	}

	FORCEINLINE bool IsSpawnDataValid() const
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

	explicit FTurboSequence_MinimalMeshData_Lf(bool bValid)
	{
		bIsValid = bValid;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	int32 RootMotionMeshID = GET0_NUMBER;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	TArray<int32> CustomizableMeshIDs;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	bool bIsValid = false;

public:
	FORCEINLINE bool IsMeshDataValid() const
	{
		return bIsValid && RootMotionMeshID > INDEX_NONE;
	}

	FORCEINLINE bool operator==(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return Equals(Other);
	}

	FORCEINLINE bool operator!=(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return !Equals(Other);
	}

	FORCEINLINE bool Equals(const FTurboSequence_MinimalMeshData_Lf& Other) const
	{
		return RootMotionMeshID == Other.RootMotionMeshID;
	}

	FORCEINLINE FString ToString() const
	{
		FString String = FString::Printf(
			TEXT("bIsValid -> %s | Root Mesh ID -> %d"), IsMeshDataValid() ? TEXT("True") : TEXT("False"),
			RootMotionMeshID);
		for (int32 MeshID : CustomizableMeshIDs)
		{
			String += FString::Printf(TEXT(" | Customizable ID -> %d"), MeshID);
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

	FTurboSequence_PoseCurveData_Lf(const TObjectPtr<UAnimSequence>& CurveAnimation, const FName& CurveID,
	                                float CurveFrame0)
		: CurveAnimation(CurveAnimation),
		  CurveName(CurveID),
		  CurveFrame_0(CurveFrame0)
	{
	}

	~FTurboSequence_PoseCurveData_Lf()
	{
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	TObjectPtr<UAnimSequence> CurveAnimation = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	FName CurveName = FName("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	float CurveFrame_0 = GET0_NUMBER;

	FORCEINLINE bool IsCurveValid() const
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

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TArray<int32> RawIDs;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TMap<int32, int32> RawIDData;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TMap<FTurboSequence_MinimalMeshData_Lf, FIntVector2> MeshIDToMinimal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="TurboSequence")
	TArray<FTurboSequence_MinimalMeshData_Lf> RawMinimalData;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_CameraInfo_Lf
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	FMinimalViewInfo ViewInfo = FMinimalViewInfo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	// The size of the primary viewport subregion allocated to this player. 0-1
	FVector2D LocalPlayerSize = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	float Fov = GET0_NUMBER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TEnumAsByte<enum EAspectRatioAxisConstraint> AspectRatioAxisConstraint = EAspectRatioAxisConstraint::AspectRatio_MAX;
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_LF_API FTurboSequence_UpdateContext_Lf
{
	GENERATED_BODY()

	FTurboSequence_UpdateContext_Lf()
	{
	}

	~FTurboSequence_UpdateContext_Lf()
	{
	}

	FTurboSequence_UpdateContext_Lf(const int32 GroupIndex)
		: GroupIndex(GroupIndex)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	int32 GroupIndex = GET0_NUMBER;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TArray<FTurboSequence_CameraInfo_Lf> CustomCameraInfo;
};
