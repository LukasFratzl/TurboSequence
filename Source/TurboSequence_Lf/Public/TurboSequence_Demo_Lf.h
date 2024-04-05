// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "TurboSequence_Manager_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "GameFramework/Actor.h"
#include "TurboSequence_Demo_Lf.generated.h"

struct FDemoMeshWrapper_Lf
{
	FTurboSequence_MinimalMeshData_Lf MeshData;
	float RandomRotationTimer;
	float RandomRotationYaw;
	float RandomAnimationTimer;
	float DeltaTimeAccumulator;
	float IKWeight;
	int32 AssetDataIndex;
	//TObjectPtr<UTurboSequence_MeshAsset_Lf> AssetToUse;

	int32 DefaultUpdateGroupIndex;
	int32 CurrentUpdateGroupIndex;

	FTurboSequence_AnimMinimalCollection_Lf CurrentAnimation_0;
	FTurboSequence_AnimMinimalCollection_Lf CurrentAnimation_1;
	float RandomAnimationData_0;
	float RandomAnimationData_1;

	FTurboSequence_AnimMinimalBlendSpaceCollection_Lf CurrentBlendSpace;
	FVector3f RandomBlendSpacePosition;

	float CustomDataTimer;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoMaterialData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<UMaterialInterface>> Materials;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoMeshSpawnData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName CategoryName;

	UPROPERTY(EditAnywhere)
	bool bExclude = false;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset;

	UPROPERTY(EditAnywhere)
	TMap<FName, FDemoMaterialData_Lf> Materials;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoCustomizationContainer_Lf
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<FDemoMeshSpawnData_Lf> CategorizedData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoAssetData_Lf
{
	GENERATED_BODY()

	FDemoAssetData_Lf()
	{
	}

	~FDemoAssetData_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bExcludeFromSystem = false;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseRootMotion = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseProgrammableSpeed = false;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseRandomRotation = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseRandomAnimation = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseIK = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	FName HeadBone = FName("head");

	UPROPERTY(EditAnywhere, Category="Demo", meta=(ClampMin="0", ClampMax="1"))
	float HeadIKStiffness = 1.0f;

	UPROPERTY(EditAnywhere, Category="Demo")
	FName NeckBone = FName("neck_01");

	UPROPERTY(EditAnywhere, Category="Demo", meta=(ClampMin="0", ClampMax="1"))
	float NeckIKStiffness = 0.75f;

	UPROPERTY(EditAnywhere, Category="Demo")
	FName Spine2Bone = FName("spine_02");

	UPROPERTY(EditAnywhere, Category="Demo", meta=(ClampMin="0", ClampMax="1"))
	float Spine2IKStiffness = 0.5f;

	UPROPERTY(EditAnywhere, Category="Demo")
	FName Spine1Bone = FName("spine_01");

	UPROPERTY(EditAnywhere, Category="Demo", meta=(ClampMin="0", ClampMax="1"))
	float Spine1IKStiffness = 0.25f;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseLayer = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseTweaks = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	bool bUseCustomData = true;

	UPROPERTY(EditAnywhere, Category="Demo")
	TArray<FTurboSequence_BoneLayer_Lf> BoneLayers;

	UPROPERTY(EditAnywhere, Category="Demo")
	FTransform SpawnOffsetTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere)
	TArray<FDemoMeshSpawnData_Lf> RootAssets;

	UPROPERTY(EditAnywhere)
	TArray<FDemoMeshSpawnData_Lf> CustomizableAssets;

	FDemoCustomizationContainer_Lf CategorizedRootData;
	TMap<FName, FDemoCustomizationContainer_Lf> CategorizeCustomizableData;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset;
};


UCLASS()
class TURBOSEQUENCE_LF_API ATurboSequence_Demo_Lf : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurboSequence_Demo_Lf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void SpawnCharactersDelayed();
	void BindToInput();
	void LeftMouseButtonPressed();

	void SolveGroup(const int32& GroupIndex, const float& DeltaTime, const FRotator& CameraRotation, const FVector& CameraLocation, TMap<uint32, bool>& SwitchingGroups);

	void SolveMesh(FDemoMeshWrapper_Lf& Mesh, FCriticalSection& CriticalSection, const FRotator& CameraRotation, const FVector& CameraLocation, TMap<uint32, bool>& SwitchingGroups, const float& DeltaTime);

	const uint8 QualityGroupIndex = 10;

	/*
    < - - - - - - - - - - - - - - - - - - - - >
                    DEMO LOGIC
    < - - - - - - - - - - - - - - - - - - - - >
*/
	UPROPERTY(EditAnywhere)
	bool bUpdateManager = false;

	//UPROPERTY(EditAnywhere)
	//bool bCollectGarbageThisFrame = false;

	UPROPERTY(EditAnywhere)
	int32 NumInstancesPerUpdateGroup = 21000;

	UPROPERTY(EditAnywhere)
	int32 RadiusOfHighQualitySolving = 25000;

	int32 UpdateGroupIndex = 0;

	TArray<float> UpdateGroupDeltaTimes;

	int32 CurrentMeshCount_Internal = 0;
	FVector LastSpawnLocation_Internal = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category="Demo")
	int32 AmountOfMeshesToSpawn = 0;

	UPROPERTY(EditAnywhere, Category="Demo")
	uint16 SpawnFrameDelay = 10;

	uint16 SpawnFrameDelay_Internal = 0;

	UPROPERTY(EditAnywhere, Category="Demo")
	float DistanceBetweenMeshes = 300;;

	UPROPERTY(EditAnywhere, Category="Demo")
	TArray<FDemoAssetData_Lf> AssetData;

	TArray<FDemoAssetData_Lf*> AssetDataRuntime;

	UPROPERTY(EditAnywhere, Category="Test")
	bool bUseTestRotationTransform = false;

	UPROPERTY(EditAnywhere, Category="Test")
	FTransform TestRotationTransform = FTransform::Identity;

	//UPROPERTY(EditAnywhere, Category="Test")
	//TObjectPtr<UBlendSpace> TestBlendSpace = nullptr;

	//UPROPERTY(EditAnywhere, Category="Test", meta=(ClampMin=-100, ClampMax=100))
	//float TestBlendSpaceSlider = 0;

	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<UStaticMeshComponent> TestCube;


	TMap<uint32, FDemoMeshWrapper_Lf> Meshes;

	static void GetRandomMeshSpawnData(FTurboSequence_MeshSpawnData_Lf& Data, const FDemoCustomizationContainer_Lf& CategorizedRootData, const TMap<FName, FDemoCustomizationContainer_Lf>& CategorizeCustomizableData, const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset)
	{
		Data = FTurboSequence_MeshSpawnData_Lf();

		if (const int32& RandomRootMesh = FMath::RandRange(0, CategorizedRootData.CategorizedData.Num() - 1); CategorizedRootData.CategorizedData.IsValidIndex(RandomRootMesh))
		{
			Data.RootMotionMesh.Mesh = CategorizedRootData.CategorizedData[RandomRootMesh].Asset;
			Data.RootMotionMesh.FootprintAsset = FootprintAsset;
			if (CategorizedRootData.CategorizedData[RandomRootMesh].Materials.Num())
			{
				TArray<FName> Names;
				CategorizedRootData.CategorizedData[RandomRootMesh].Materials.GetKeys(Names);
				const int32& RandomRootMaterial = FMath::RandRange(0, CategorizedRootData.CategorizedData[RandomRootMesh].Materials.Num() - 1);
				Data.RootMotionMesh.OverrideMaterials = CategorizedRootData.CategorizedData[RandomRootMesh].Materials[Names[RandomRootMaterial]].Materials;
			}
		}
		for (const TTuple<FName, FDemoCustomizationContainer_Lf>& Customizable : CategorizeCustomizableData)
		{
			if (Customizable.Value.CategorizedData.Num())
			{
				const int32& RandomMesh = FMath::RandRange(0, Customizable.Value.CategorizedData.Num() - 1);
				FMeshMetaData_Lf MeshData = FMeshMetaData_Lf();
				MeshData.Mesh = Customizable.Value.CategorizedData[RandomMesh].Asset;
				MeshData.FootprintAsset = FootprintAsset;
				if (Customizable.Value.CategorizedData[RandomMesh].Materials.Num())
				{
					TArray<FName> Names;
					Customizable.Value.CategorizedData[RandomMesh].Materials.GetKeys(Names);
					const int32& RandomMaterial = FMath::RandRange(0, Customizable.Value.CategorizedData[RandomMesh].Materials.Num() - 1);
					MeshData.OverrideMaterials = Customizable.Value.CategorizedData[RandomMesh].Materials[Names[RandomMaterial]].Materials;
				}
				Data.CustomizableMeshes.Add(MeshData);
			}
		}
	}

	static FORCEINLINE_DEBUGGABLE void SolveLookAtIKBone(const FTurboSequence_MinimalMeshData_Lf& MeshData, const FName& BoneName, const FVector& TargetLocation, const float& Weight, const float& OffsetWeight, const float& AnimationDeltaTime, const FTransform& StartOffsetTransform)
	{
		FTransform IKTransform;
		ATurboSequence_Manager_Lf::GetIKTransform_Concurrent(IKTransform, MeshData, BoneName, AnimationDeltaTime, EBoneSpaces::WorldSpace);

		FTransform ReferencePoseTransform;
		ATurboSequence_Manager_Lf::GetReferencePoseTransform_Concurrent(ReferencePoseTransform, MeshData, BoneName, ETurboSequence_TransformSpace_Lf::ComponentSpace);
		ReferencePoseTransform *= StartOffsetTransform;

		const FQuat& WantedRotation = FRotationMatrix::MakeFromX(TargetLocation - IKTransform.GetLocation()).ToQuat();
		IKTransform.SetRotation(FQuat::Slerp(IKTransform.GetRotation(), WantedRotation * ReferencePoseTransform.GetRotation(), Weight * OffsetWeight));


		ATurboSequence_Manager_Lf::SetIKTransform_Concurrent(MeshData, BoneName, IKTransform, EBoneSpaces::WorldSpace);
	}

	UFUNCTION(BlueprintCallable)
	static void SolveBlueprintDemoMeshesInCpp(const TArray<FTurboSequence_MinimalMeshData_Lf>& MeshData, UPARAM(ref) TArray<FVector4>& AnimData, UAnimSequence* AnimA, UAnimSequence* AnimB, const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsA, const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsB, const float DeltaTime, const bool bMultiThreaded)
	{
		auto RunnerFunction = [&](const FTurboSequence_MinimalMeshData_Lf& MeshID, const int32& Index)
		{
			FVector4& AnimDataOut = AnimData[Index];

			//AnimDataOut = AnimationData;

			AnimDataOut.X -= DeltaTime;
			if (AnimDataOut.X < 0)
			{
				AnimDataOut.X = FMath::RandRange(3.0f, 7.0f);

				if (FMath::RandBool())
				{
					ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshID, AnimA, AnimSettingsA);
					ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshID, AnimB, AnimSettingsB);
				}
				else
				{
					ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshID, AnimB, AnimSettingsA);
					ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshID, AnimA, AnimSettingsB);
				}
			}

			AnimDataOut.Y -= DeltaTime;
			if (AnimDataOut.Y < 0)
			{
				AnimDataOut.Y = FMath::RandRange(3.0f, 7.0f);
				AnimDataOut.Z = FMath::RandRange(-180.0, 180.0);
			}
			FTransform RanRotationTransform = ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(MeshID);
			const FRotator CurrentRotation = RanRotationTransform.GetRotation().Rotator();
			RanRotationTransform.SetRotation(FMath::RInterpConstantTo(CurrentRotation, FRotator(CurrentRotation.Pitch, AnimDataOut.Z, CurrentRotation.Roll), DeltaTime, 3).Quaternion());

			ATurboSequence_Manager_Lf::SetMeshWorldSpaceLocationRotationScale_Concurrent(MeshID, RanRotationTransform.GetLocation(), RanRotationTransform.GetRotation(), RanRotationTransform.GetScale3D());

			ATurboSequence_Manager_Lf::MoveMeshWithRootMotion_Concurrent(MeshID, DeltaTime, true, false);
		};

		const int32 NumMeshes = FMath::Min(MeshData.Num(), AnimData.Num());
		if (!NumMeshes)
		{
			return;
		}

		if (bMultiThreaded)
		{
			const int16& NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;
			const int32& NumMeshesPerThread_Background = FMath::CeilToInt(
				static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
			ParallelFor(NumThreads, [&](const int32& ThreadsIndex)
			{
				const int32 MeshBaseIndex = ThreadsIndex * NumMeshesPerThread_Background;
				const int32 MeshBaseNum = MeshBaseIndex + NumMeshesPerThread_Background;

				for (int32 Index = MeshBaseIndex; Index < MeshBaseNum; ++Index)
				{
					if (Index >= NumMeshes)
					{
						break;
					}

					RunnerFunction(MeshData[Index], Index);
				}
			}, EParallelForFlags::BackgroundPriority);
		}
		else
		{
			for (int32 Index = 0; Index < NumMeshes; ++Index)
			{
				RunnerFunction(MeshData[Index], Index);
			}
		}
	}
};
