// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "TurboSequence_AnimLibrary_Lf.h"
#include "TurboSequence_GlobalData_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "UObject/Object.h"
#include "TurboSequence_MeshAsset_Lf.generated.h"


USTRUCT()
struct TURBOSEQUENCE_LF_API FMeshData_Lf
{
	GENERATED_BODY()

	FMeshData_Lf()
	{
	}

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	int32 NumVertices = GET0_NUMBER;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TMap<int32, int32> CPUBoneToGPUBoneIndicesMap;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FMeshDataOrderView_Lf
{
	GENERATED_BODY()

	FMeshDataOrderView_Lf()
	{
	}

	UPROPERTY(VisibleAnywhere, SaveGame, Category="TurboSequence")
	TArray<int32> StaticMeshIndices;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FMeshItem_Lf
{
	GENERATED_BODY()

	FMeshItem_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="Instance",
		meta=(ToolTip=
			"The Static Mesh for this Level Of Detail, when you replace it with a different mesh, you need to re-bake the Mesh Asset"
			, ShortTooltip=
			"The Static Mesh for this Level Of Detail, when you replace it with a different mesh, you need to re-bake the Mesh Asset"
		))
	// The Static Mesh for this Level Of Detail, when you replace it with a different mesh, you need to re-bake the Mesh Asset
	TObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, Category="Settings",
		meta=(ToolTip=
			"Does the Mesh needs to be Animated because on 300 meters it may does not make a difference for the view",
			ShortTooltip=
			"Does the Mesh needs to be Animated because on 300 meters it may does not make a difference for the view"))
	// Does the Mesh needs to be Animated because on 300 meters it may does not make a difference for the view
	bool bIsAnimated = true;

	UPROPERTY(EditAnywhere, Category="Settings",
		meta=(ToolTip="Does the Mesh needs to be shown in the level, useful for debugging Level of Details",
			ShortTooltip="Does the Mesh needs to be shown in the level, useful for debugging Level of Details"))
	// Does the Mesh needs to be shown in the level, useful for debugging Level of Details
	bool bShowMesh = true;

	UPROPERTY(EditAnywhere, Category="Settings",
		meta=(ToolTip=
			"Does the Mesh needs to be enabled for frustum culling, it makes sense to not cull the LOD 0 because of the shadows"
			, ShortTooltip=
			"Does the Mesh needs to be enabled for frustum culling, it makes sense to not cull the LOD 0 because of the shadows"
		))
	// Does the Mesh needs to be enabled for frustum culling, it makes sense to not cull the LOD 0 because of the shadows
	bool bIsFrustumCullingEnabled = true;

	UPROPERTY(EditAnywhere, Category="Settings",
		meta=(ToolTip=
			"Does the Mesh needs to be part of the system, useful if the poly count is too high or to low, in this case the mesh can getting hidden"
			, ShortTooltip=
			"Does the Mesh needs to be part of the system, useful if the poly count is too high or to low, in this case the mesh can getting hidden"
		))
	// Does the Mesh needs to be part of the system, useful if the poly count is too high or to low, in this case the mesh can getting hidden
	bool bExcludeLodFromSystem = false;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_MeshAsset_Lf : public UObject
{
	GENERATED_BODY()

public:
	UTurboSequence_MeshAsset_Lf();

	/*	==============================================================================================================
												GAMEPLAY
	==============================================================================================================	*/
	UPROPERTY(EditAnywhere, Category="Global")
	bool bExcludeFromSystem = false;

	UPROPERTY(EditAnywhere, Category="Global")
	TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData;

	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<UNiagaraSystem> RendererSystem;

	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<USkeletalMesh> ReferenceMeshNative;

	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<USkeletalMesh> ReferenceMeshEdited;

	UPROPERTY(EditAnywhere, Category="Reference")
	TObjectPtr<UTexture2DArray> MeshDataTexture;


	UPROPERTY(EditAnywhere, Category="Optimization",
		meta=(ClampMin = "0.001", ClampMax = "0.2", ToolTip=
			"Turbo Sequence makes linear Keyframe Reduction, 1 Keyframe happens in this interval, ( Quality | Memory Usage ) <- -> ( Low Memory Usage )"
			, ShortTooltip=
			"Turbo Sequence makes linear Keyframe Reduction, 1 Keyframe happens in this interval, ( Quality | Memory Usage ) <- -> ( Low Memory Usage )"
		))
	// Turbo Sequence makes linear Keyframe Reduction, 1 Keyframe happens in this interval, ( Quality | Memory Usage ) <- -> ( Low Memory Usage )
	float TimeBetweenAnimationLibraryFrames = 0.05f;

	UPROPERTY(EditAnywhere, Category="Optimization",
		meta=(ToolTip="If Enabled Instances close the camera update more frequent", ShortTooltip=
			"If Enabled Instances close the camera update more frequent"))
	// If Enabled Instances close the camera update more frequent
	bool bUseDistanceUpdating = true;


	UPROPERTY(EditAnywhere, Category="Optimization",
		meta=(ClampMin = "0.05", ClampMax = "1.9", ToolTip=
			"Lower means More Solver Ticks relative to the distance of the Instance", ShortTooltip=
			"Lower means More Solver Ticks relative to the distance of the Instance"))
	// Lower means More Solver Ticks relative to the distance of the Instance
	float DistanceUpdatingRatio = 0.25f;

	UPROPERTY(EditAnywhere, Category="Lod",
		meta=(ClampMin = "0", ClampMax = "10000", ToolTip=
			"Draw Range of the Highest Detail LOD, 0 Means it's disabled | Otherwise Average range is from 2000 - 6000",
			ShortTooltip=
			"Draw Range of the Highest Detail LOD, 0 Means it's disabled | Otherwise Average range is from 2000 - 6000"
		))
	// Draw Range of the Highest Detail LOD, 0 Means it's disabled | Otherwise Average range is from 2000 - 6000
	int32 HighestDetailDrawDistance = 0;

	UPROPERTY(EditAnywhere, Category="Lod",
		meta=(ClampMin = "500", ClampMax = "10000", ToolTip=
			"Higher Value means More Spread-out LODs, means MinLOD = AutoLodRatio * LOD_Percentage", ShortTooltip=
			"Higher Value means More Spread-out LODs, means MinLOD = AutoLodRatio * LOD_Percentage"))
	// Higher Value means More Spread-out LODs, means MinLOD = AutoLodRatio * LOD_Percentage
	int32 AutoLodRatio = 3000;

	UPROPERTY(EditAnywhere, Category="Lod")
	TArray<FMeshItem_Lf> InstancedMeshes;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UAnimSequence> OverrideDefaultAnimation;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UTurboSequence_AnimLibrary_Lf> AnimationLibrary;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	ETurboSequence_MeshDataMode_Lf MeshDataMode = ETurboSequence_MeshDataMode_Lf::UV;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TArray<FMeshData_Lf> MeshData;

	int32 MeshDataCustomData = GET0_NUMBER;

	UPROPERTY(VisibleAnywhere, SaveGame, Category="TurboSequence")
	TArray<FMeshDataOrderView_Lf> MeshDataOrderView;

	const FReferenceSkeleton& GetReferenceSkeleton() const
	{
		return ReferenceMeshNative->GetRefSkeleton();
	}

	TObjectPtr<USkeleton> GetSkeleton() const
	{
		return ReferenceMeshNative->GetSkeleton();
	}

	bool IsMeshAssetValid() const
	{
		if (!IsValid(this))
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT("Can't create Mesh Instance, the Asset you use is not valid...."));
			return false;
		}

		if (!IsValid(GetSkeleton()))
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT("Can't create Mesh Instance, the Asset you use has no Skeleton assigned...."));
			return false;
		}

		if (!MeshData.Num())
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT("Can't create Mesh Instance, the Asset you use has no Mesh Data, please bake the mesh fist...."
			       ));
			return false;
		}

		if (!InstancedMeshes.Num())
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Can't create Mesh Instance, the Asset you use has no LODs...."));
			return false;
		}

		if (bExcludeFromSystem)
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "Can't create Mesh Instance, the Asset you use is excluded from the system because bExcludeFromSystem is true"
			       ));
			return false;
		}

		return true;
	}

	/*	==============================================================================================================
												UI
	==============================================================================================================	*/

	UPROPERTY(EditAnywhere, Category="UI")
	int32 MaxLevelOfDetails = 10;

	/*	==============================================================================================================
												COMPATIBILITY
	==============================================================================================================	*/

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	bool bNeedGeneratedNextEngineStart = false;
};
