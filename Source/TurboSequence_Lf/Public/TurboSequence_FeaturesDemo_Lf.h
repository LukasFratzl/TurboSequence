// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_Demo_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "GameFramework/Actor.h"
#include "TurboSequence_FeaturesDemo_Lf.generated.h"


USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	bool bEnable = true;

	UPROPERTY(EditAnywhere)
	TArray<FTransform> ActionTransforms;

	UPROPERTY(EditAnywhere)
	TArray<FTurboSequence_MeshSpawnData_Lf> Spawns;

	UPROPERTY(EditAnywhere)
	TArray<FTurboSequence_MinimalMeshData_Lf> MeshData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoBlending_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FTurboSequence_BoneLayer_Lf> BlendLayers;

	float RandomTimer = 0;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoTweaking_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FTurboSequence_AnimMinimalCollection_Lf TweakingAnimation;

	float RandomTimer = 0;
	float RandomAnimationData = 0;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoRootMotion_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoIK_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TMap<FName, float> IKWeights;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere)
	float MasterIKWeight = GET0_NUMBER;

	UPROPERTY(EditAnywhere)
	FTransform SpawnOffset = FTransform::Identity;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoLod_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoSockets_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere)
	FName SocketName = FName("Socket");
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoCustomization_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;

	float RandomTimer = 0;
	int16 RandomIndex = 0;

	UPROPERTY(EditAnywhere)
	TArray<FDemoMeshSpawnData_Lf> RootAssets;

	UPROPERTY(EditAnywhere)
	TArray<FDemoMeshSpawnData_Lf> CustomizableAssets;

	UPROPERTY(VisibleAnywhere)
	FDemoCustomizationContainer_Lf CategorizedRootData;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, FDemoCustomizationContainer_Lf> CategorizeCustomizableData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoBlendSpace_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBlendSpace> DemoBlendSpace;

	UPROPERTY(EditAnywhere)
	FTurboSequence_AnimMinimalBlendSpaceCollection_Lf TweakingBlendSpace;

	float RandomTimer = 0;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoCurves_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere)
	FTurboSequence_AnimMinimalCollection_Lf DemoAnimationData;

	UPROPERTY(EditAnywhere)
	FName CurveName = FName("Curve");

	UPROPERTY(EditAnywhere)
	FName MaterialParameterName = FName("bIsCurveValue");

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> RfMaterialInstance;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstanceDynamic> LfMaterialInstance;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoNormals_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoUpdateGroups_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;

	TArray<float> DeltaTime;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoHybridMode_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DemoAnimation;
};

UENUM(BlueprintType)
enum class EFeatureDemoEnableFeature_Lf : uint8
{
	SimpleBlending,
	LayerBlending,
	Tweaking,
	RootMotion,
	IK,
	LevelOfDetails,
	Sockets,
	Customization,
	BlendSpaces,
	//Additive,
	Curves,
	Normals,
	UpdateGroups,
	HybridMode
};


UCLASS()
class TURBOSEQUENCE_LF_API ATurboSequence_FeaturesDemo_Lf : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurboSequence_FeaturesDemo_Lf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	bool bUpdateManager = false;


	UPROPERTY(EditAnywhere)
	TArray<EFeatureDemoEnableFeature_Lf> EnableFlags;

	FORCEINLINE bool ShouldEnableFeature(const EFeatureDemoEnableFeature_Lf& Feature)
	{
		if (!EnableFlags.Num())
		{
			return true;
		}

		for (const EFeatureDemoEnableFeature_Lf& Other : EnableFlags)
		{
			if (Feature == Other)
			{
				return true;
			}
		}

		return false;
	}

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(EditAnywhere)
	FFeatureDemoBlending_Lf SimpleBlendingDemo = FFeatureDemoBlending_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoBlending_Lf LayerBlendingDemo = FFeatureDemoBlending_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoTweaking_Lf AnimationTweaksDemo = FFeatureDemoTweaking_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoRootMotion_Lf RootMotionDemo = FFeatureDemoRootMotion_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoIK_Lf IKDemo = FFeatureDemoIK_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoLod_Lf LodDemo = FFeatureDemoLod_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoSockets_Lf SocketsDemo = FFeatureDemoSockets_Lf();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> SocketsDemoMesh;

	UPROPERTY(EditAnywhere)
	FFeatureDemoCustomization_Lf CustomizationDemo = FFeatureDemoCustomization_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoBlendSpace_Lf BlendSpaceDemo = FFeatureDemoBlendSpace_Lf();

	inline static FVector3f LastDemoBlendSpacePosition = FVector3f::ZeroVector;

	UFUNCTION(BlueprintCallable)
	static FVector3f GetFeatureDemoBlendSpacePosition_TurboSequence_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoCurves_Lf CurveDemo = FFeatureDemoCurves_Lf();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> RfCurveDemoMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> LfCurveDemoMesh;

	UPROPERTY(EditAnywhere)
	FFeatureDemoNormals_Lf NormalsDemo = FFeatureDemoNormals_Lf();

	UPROPERTY(EditAnywhere)
	FFeatureDemoUpdateGroups_Lf UpdateGroupsDemo = FFeatureDemoUpdateGroups_Lf();

	int32 CurrentUpdateGroupIndex = 1;

	UPROPERTY(EditAnywhere)
	FFeatureDemoHybridMode_Lf HybridMode = FFeatureDemoHybridMode_Lf();
};
