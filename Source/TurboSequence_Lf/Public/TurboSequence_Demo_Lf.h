// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

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

	void SolveGroup(int32 GroupIndex, float DeltaTime, const FRotator& CameraRotation, const FVector& CameraLocation,
	                TMap<FTurboSequence_MinimalMeshData_Lf, bool>& SwitchingGroups);

	const uint8 QualityGroupIndex = 10;

	/*
    < - - - - - - - - - - - - - - - - - - - - >
                    DEMO LOGIC
    < - - - - - - - - - - - - - - - - - - - - >
*/
	UPROPERTY(EditAnywhere)
	bool bUpdateManager = false;

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


	TMap<FTurboSequence_MinimalMeshData_Lf, FDemoMeshWrapper_Lf> Meshes;

	static void GetRandomMeshSpawnData(FTurboSequence_MeshSpawnData_Lf& Data,
	                                   const FDemoCustomizationContainer_Lf& CategorizedRootData,
	                                   const TMap<FName, FDemoCustomizationContainer_Lf>& CategorizeCustomizableData,
	                                   const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset);

	static void SolveLookAtIKBone(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                              const FName& BoneName, const FVector& TargetLocation,
	                              float Weight, float OffsetWeight, float AnimationDeltaTime,
	                              const FTransform& StartOffsetTransform);

	UFUNCTION(BlueprintCallable)
	static void SolveBlueprintDemoMeshesInCpp(const TArray<FTurboSequence_MinimalMeshData_Lf>& MeshData,
	                                          UPARAM(ref) TArray<FVector4>& AnimData, UAnimSequence* AnimA,
	                                          UAnimSequence* AnimB,
	                                          const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsA,
	                                          const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsB,
	                                          const float DeltaTime, const bool bMultiThreaded);
};
