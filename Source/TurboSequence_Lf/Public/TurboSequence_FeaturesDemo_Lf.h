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

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	bool bEnable = true;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FTransform> ActionTransforms;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FTurboSequence_MeshSpawnData_Lf> Spawns;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FTurboSequence_MinimalMeshData_Lf> MeshData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoBlending_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FTurboSequence_BoneLayer_Lf> BlendLayers;

	float RandomTimer = 0;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoTweaking_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FTurboSequence_AnimMinimalCollection_Lf TweakingAnimation;

	float RandomTimer = 0;
	float RandomAnimationData = 0;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoRootMotion_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoIK_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TMap<FName, float> IKWeights;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	float MasterIKWeight = GET0_NUMBER;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FTransform SpawnOffset = FTransform::Identity;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoLod_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoSockets_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName SocketName = FName("Socket");
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoCustomization_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FTurboSequence_AnimMinimalCollection_Lf TargetAnimations;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FAnimationMetaData_Lf> ExtractedAnimationsData;

	float RandomTimer = 0;
	int16 RandomIndex = 0;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FDemoMeshSpawnData_Lf> RootAssets;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FDemoMeshSpawnData_Lf> CustomizableAssets;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	FDemoCustomizationContainer_Lf CategorizedRootData;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TMap<FName, FDemoCustomizationContainer_Lf> CategorizeCustomizableData;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoBlendSpace_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UBlendSpace> DemoBlendSpace;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FTurboSequence_AnimMinimalBlendSpaceCollection_Lf TweakingBlendSpace;

	float RandomTimer = 0;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoCurves_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FTurboSequence_AnimMinimalCollection_Lf DemoAnimationData;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName CurveName = FName("Curve");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName MaterialParameterName = FName("bIsCurveValue");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UMaterialInstanceDynamic> RfMaterialInstance;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UMaterialInstanceDynamic> LfMaterialInstance;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoNormals_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;
};

USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoUpdateGroups_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;

	TArray<float> DeltaTime;
};


USTRUCT()
struct TURBOSEQUENCE_LF_API FFeatureDemoHybridMode_Lf : public FFeatureDemoBase_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> DemoAnimation;
};

UENUM(BlueprintType)
enum class EFeatureDemoEnableFeature_Lf : uint8
{
	None,
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

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	bool bUpdateManager = false;


	UPROPERTY(EditAnywhere, Category="TurboSequence")
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

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<USceneComponent> Root = nullptr;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoBlending_Lf SimpleBlendingDemo = FFeatureDemoBlending_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoBlending_Lf LayerBlendingDemo = FFeatureDemoBlending_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoTweaking_Lf AnimationTweaksDemo = FFeatureDemoTweaking_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoRootMotion_Lf RootMotionDemo = FFeatureDemoRootMotion_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoIK_Lf IKDemo = FFeatureDemoIK_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoLod_Lf LodDemo = FFeatureDemoLod_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoSockets_Lf SocketsDemo = FFeatureDemoSockets_Lf();

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<UStaticMeshComponent> SocketsDemoMesh;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoCustomization_Lf CustomizationDemo = FFeatureDemoCustomization_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoBlendSpace_Lf BlendSpaceDemo = FFeatureDemoBlendSpace_Lf();

	inline static FVector3f LastDemoBlendSpacePosition = FVector3f::ZeroVector;

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	static FVector3f GetFeatureDemoBlendSpacePosition_TurboSequence_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoCurves_Lf CurveDemo = FFeatureDemoCurves_Lf();

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<UStaticMeshComponent> RfCurveDemoMesh;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<UStaticMeshComponent> LfCurveDemoMesh;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoNormals_Lf NormalsDemo = FFeatureDemoNormals_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoUpdateGroups_Lf UpdateGroupsDemo = FFeatureDemoUpdateGroups_Lf();

	int32 CurrentUpdateGroupIndex = 1;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FFeatureDemoHybridMode_Lf HybridMode = FFeatureDemoHybridMode_Lf();
};
