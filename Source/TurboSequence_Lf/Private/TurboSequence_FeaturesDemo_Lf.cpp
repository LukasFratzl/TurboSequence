// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_FeaturesDemo_Lf.h"

#include "TurboSequence_Demo_Lf.h"
#include "TurboSequence_Manager_Lf.h"


// Sets default values
ATurboSequence_FeaturesDemo_Lf::ATurboSequence_FeaturesDemo_Lf()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;

	Root = CreateDefaultSubobject<USceneComponent>("Root Component");
	SetRootComponent(Root);

	SocketsDemoMesh = CreateDefaultSubobject<UStaticMeshComponent>("Demo Sockets Mesh");
	SocketsDemoMesh->SetupAttachment(Root);

	RfCurveDemoMesh = CreateDefaultSubobject<UStaticMeshComponent>("Demo Curve Right Foot Mesh");
	RfCurveDemoMesh->SetupAttachment(Root);

	LfCurveDemoMesh = CreateDefaultSubobject<UStaticMeshComponent>("Demo Curve Left Foot Mesh");
	LfCurveDemoMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ATurboSequence_FeaturesDemo_Lf::BeginPlay()
{
	Super::BeginPlay();

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::SimpleBlending) && SimpleBlendingDemo.bEnable &&
		SimpleBlendingDemo.Spawns.Num())
	{
		SimpleBlendingDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				SimpleBlendingDemo.Spawns[0], SimpleBlendingDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, SimpleBlendingDemo.MeshData[0]);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::LayerBlending) && LayerBlendingDemo.bEnable &&
		LayerBlendingDemo.Spawns.Num())
	{
		LayerBlendingDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				LayerBlendingDemo.Spawns[0], LayerBlendingDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, LayerBlendingDemo.MeshData[0]);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::Tweaking) && AnimationTweaksDemo.bEnable &&
		AnimationTweaksDemo.Spawns.Num())
	{
		AnimationTweaksDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				AnimationTweaksDemo.Spawns[0], AnimationTweaksDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, AnimationTweaksDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = AnimationTweaksDemo.DemoAnimation;
		AnimationTweaksDemo.TweakingAnimation = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(
			AnimationTweaksDemo.MeshData[0], AnimationTweaksDemo.DemoAnimation, AnimationPlaySettings);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::RootMotion) && RootMotionDemo.bEnable && RootMotionDemo.Spawns
		.Num())
	{
		RootMotionDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				RootMotionDemo.Spawns[0], RootMotionDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, RootMotionDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = RootMotionDemo.DemoAnimation;
		ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(RootMotionDemo.MeshData[0], RootMotionDemo.DemoAnimation,
		                                                    AnimationPlaySettings);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::IK) && IKDemo.bEnable && IKDemo.Spawns.Num())
	{
		IKDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				IKDemo.Spawns[0], IKDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, IKDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = IKDemo.DemoAnimation;
		ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(IKDemo.MeshData[0], IKDemo.DemoAnimation,
		                                                    AnimationPlaySettings);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::LevelOfDetails) && LodDemo.bEnable && LodDemo.Spawns.Num() >=
		2)
	{
		int8 Index = 0;
		for (const FTurboSequence_MeshSpawnData_Lf& Spawn : LodDemo.Spawns)
		{
			const FTurboSequence_MinimalMeshData_Lf& MeshData =
				ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
					Spawn, LodDemo.ActionTransforms[Index], GetWorld());
			ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, MeshData);

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			//AnimationPlaySettings.Animation = LodDemo.DemoAnimation;
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshData, LodDemo.DemoAnimation, AnimationPlaySettings);

			Index++;
			LodDemo.MeshData.Add(MeshData);
		}
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::Sockets) && SocketsDemo.bEnable && SocketsDemo.Spawns.Num())
	{
		SocketsDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				SocketsDemo.Spawns[0], SocketsDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, SocketsDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = SocketsDemo.DemoAnimation;
		ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(SocketsDemo.MeshData[0], SocketsDemo.DemoAnimation,
		                                                    AnimationPlaySettings);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::Customization) && CustomizationDemo.bEnable &&
		CustomizationDemo.RootAssets.Num())
	{
		CustomizationDemo.CategorizedRootData.CategorizedData.Empty();
		for (const FDemoMeshSpawnData_Lf& RootAsset : CustomizationDemo.RootAssets)
		{
			if (IsValid(RootAsset.Asset))
			{
				if (!RootAsset.bExclude)
				{
					CustomizationDemo.CategorizedRootData.CategorizedData.Add(RootAsset);
				}
			}
		}
		CustomizationDemo.CategorizeCustomizableData.Empty();
		for (const FDemoMeshSpawnData_Lf& CustomizableAsset : CustomizationDemo.CustomizableAssets)
		{
			if (IsValid(CustomizableAsset.Asset))
			{
				if (!CustomizableAsset.bExclude)
				{
					if (!CustomizationDemo.CategorizeCustomizableData.Contains(CustomizableAsset.CategoryName))
					{
						FDemoCustomizationContainer_Lf Data;
						Data.CategorizedData.Add(CustomizableAsset);
						CustomizationDemo.CategorizeCustomizableData.Add(CustomizableAsset.CategoryName, Data);
					}
					else
					{
						CustomizationDemo.CategorizeCustomizableData[CustomizableAsset.CategoryName].CategorizedData.
							Add(CustomizableAsset);
					}
				}
			}
		}

		for (const FTransform& SpawnTransform : CustomizationDemo.ActionTransforms)
		{
			FTurboSequence_MeshSpawnData_Lf SpawnData;

			ATurboSequence_Demo_Lf::GetRandomMeshSpawnData(SpawnData, CustomizationDemo.CategorizedRootData,
			                                               CustomizationDemo.CategorizeCustomizableData, nullptr);

			const FTurboSequence_MinimalMeshData_Lf& MeshData =
				ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(SpawnData, SpawnTransform, GetWorld());
			ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, MeshData);

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			//AnimationPlaySettings.Animation = CustomizationDemo.DemoAnimation;
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshData, CustomizationDemo.DemoAnimation,
			                                                    AnimationPlaySettings);

			CustomizationDemo.MeshData.Add(MeshData);
		}
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::BlendSpaces) && BlendSpaceDemo.bEnable && BlendSpaceDemo.
		Spawns.Num())
	{
		BlendSpaceDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				BlendSpaceDemo.Spawns[0], BlendSpaceDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, BlendSpaceDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		BlendSpaceDemo.TweakingBlendSpace = ATurboSequence_Manager_Lf::PlayBlendSpace_Concurrent(
			BlendSpaceDemo.MeshData[0], BlendSpaceDemo.DemoBlendSpace, AnimationPlaySettings);
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::Curves) && CurveDemo.bEnable && CurveDemo.Spawns.Num())
	{
		CurveDemo.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				CurveDemo.Spawns[0], CurveDemo.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, CurveDemo.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = CurveDemo.DemoAnimation;
		CurveDemo.DemoAnimationData = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(
			CurveDemo.MeshData[0], CurveDemo.DemoAnimation, AnimationPlaySettings);

		if (IsValid(RfCurveDemoMesh))
		{
			CurveDemo.RfMaterialInstance = UMaterialInstanceDynamic::Create(RfCurveDemoMesh->GetMaterial(0), nullptr);
			RfCurveDemoMesh->SetMaterial(0, CurveDemo.RfMaterialInstance);
		}

		if (IsValid(LfCurveDemoMesh))
		{
			CurveDemo.LfMaterialInstance = UMaterialInstanceDynamic::Create(LfCurveDemoMesh->GetMaterial(0), nullptr);
			LfCurveDemoMesh->SetMaterial(0, CurveDemo.LfMaterialInstance);
		}
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::Normals) && NormalsDemo.bEnable && NormalsDemo.Spawns.Num() >=
		2)
	{
		int8 Index = 0;
		for (const FTurboSequence_MeshSpawnData_Lf& Spawn : NormalsDemo.Spawns)
		{
			const FTurboSequence_MinimalMeshData_Lf& MeshData =
				ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
					Spawn, NormalsDemo.ActionTransforms[Index], GetWorld());
			ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, MeshData);

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			//AnimationPlaySettings.Animation = NormalsDemo.DemoAnimation;
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshData, NormalsDemo.DemoAnimation,
			                                                    AnimationPlaySettings);

			Index++;
			NormalsDemo.MeshData.Add(MeshData);
			//UE_LOG(LogTemp, Warning, TEXT("%s"), *MeshData.ToString())
		}
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::UpdateGroups) && UpdateGroupsDemo.bEnable && UpdateGroupsDemo.
		Spawns.Num() >= 1)
	{
		int32 GrouIndex = 1;
		//int8 Index = 0;
		for (const FTransform& SpawnTransform : UpdateGroupsDemo.ActionTransforms)
		{
			const FTurboSequence_MinimalMeshData_Lf& MeshData =
				ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
					UpdateGroupsDemo.Spawns[0], SpawnTransform, GetWorld());
			ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(GrouIndex, MeshData);

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(MeshData, UpdateGroupsDemo.DemoAnimation,
			                                                    AnimationPlaySettings);

			UpdateGroupsDemo.MeshData.Add(MeshData);
			UpdateGroupsDemo.DeltaTime.Add(0);
			GrouIndex++;
		}
	}

	if (ShouldEnableFeature(EFeatureDemoEnableFeature_Lf::HybridMode) && HybridMode.bEnable && HybridMode.Spawns
		.Num())
	{
		HybridMode.MeshData.Add(
			ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				HybridMode.Spawns[0], HybridMode.ActionTransforms[0], GetWorld()));
		ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, HybridMode.MeshData[0]);

		FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
		//AnimationPlaySettings.Animation = RootMotionDemo.DemoAnimation;
		ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(HybridMode.MeshData[0], HybridMode.DemoAnimation,
		                                                    AnimationPlaySettings);
	}
}

// Called every frame
void ATurboSequence_FeaturesDemo_Lf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FCriticalSection CriticalSection;

	if (SimpleBlendingDemo.bEnable && SimpleBlendingDemo.MeshData.Num() && SimpleBlendingDemo.Spawns[0].RootMotionMesh.
		Mesh->AnimationLibrary->Animations.Num() > 1)
	{
		SimpleBlendingDemo.RandomTimer -= DeltaTime;
		if (SimpleBlendingDemo.RandomTimer < 0)
		{
			SimpleBlendingDemo.RandomTimer = FMath::RandRange(3.0f, 4.0f);

			int32 RandomAnimation = 0;
			while (SimpleBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations[RandomAnimation].
				Animation ==
				ATurboSequence_Manager_Lf::GetHighestPriorityPlayingAnimation_Concurrent(
					SimpleBlendingDemo.MeshData[0]))
			{
				RandomAnimation = FMath::RandRange(
					0, SimpleBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations.Num() - 1);
			}

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			//AnimationPlaySettings.Animation = SimpleBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations[RandomAnimation];
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(SimpleBlendingDemo.MeshData[0],
			                                                    SimpleBlendingDemo.Spawns[0].RootMotionMesh.Mesh->
			                                                    AnimationLibrary->Animations[RandomAnimation].Animation,
			                                                    AnimationPlaySettings);
		}
	}

	if (LayerBlendingDemo.bEnable && LayerBlendingDemo.MeshData.Num() && LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh
		->AnimationLibrary->Animations.Num() > 1)
	{
		LayerBlendingDemo.RandomTimer -= DeltaTime;
		if (LayerBlendingDemo.RandomTimer < 0)
		{
			LayerBlendingDemo.RandomTimer = FMath::RandRange(3.0f, 4.0f);

			int32 RandomAnimation = 0;
			while (LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations[RandomAnimation].
				Animation ==
				ATurboSequence_Manager_Lf::GetHighestPriorityPlayingAnimation_Concurrent(LayerBlendingDemo.MeshData[0]))
			{
				RandomAnimation = FMath::RandRange(
					0, LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations.Num() - 1);
			}

			FTurboSequence_AnimPlaySettings_Lf AnimationPlaySettings = FTurboSequence_AnimPlaySettings_Lf();
			//AnimationPlaySettings.Animation = LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations[RandomAnimation];
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(LayerBlendingDemo.MeshData[0],
			                                                    LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->
			                                                    AnimationLibrary->Animations[RandomAnimation].Animation,
			                                                    AnimationPlaySettings);


			AnimationPlaySettings.BoneLayerMasks = LayerBlendingDemo.BlendLayers;
			RandomAnimation = FMath::RandRange(
				0, LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations.Num() - 1);

			//AnimationPlaySettings.Animation = LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->AnimationLibrary->Animations[RandomAnimation];
			ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(LayerBlendingDemo.MeshData[0],
			                                                    LayerBlendingDemo.Spawns[0].RootMotionMesh.Mesh->
			                                                    AnimationLibrary->Animations[RandomAnimation].Animation,
			                                                    AnimationPlaySettings);
		}
	}

	if (AnimationTweaksDemo.bEnable && AnimationTweaksDemo.MeshData.Num())
	{
		AnimationTweaksDemo.RandomTimer -= DeltaTime;
		if (AnimationTweaksDemo.RandomTimer < 0)
		{
			AnimationTweaksDemo.RandomTimer = FMath::RandRange(1.0f, 3.0f);

			AnimationTweaksDemo.RandomAnimationData = FMath::RandRange(0.5f, 3.0f);
		}

		if (AnimationTweaksDemo.TweakingAnimation.IsAnimCollectionValid())
		{
			FTurboSequence_AnimPlaySettings_Lf Settings;
			ATurboSequence_Manager_Lf::GetAnimationSettings_Concurrent(
				Settings, AnimationTweaksDemo.TweakingAnimation.RootMotionMesh);

			Settings.AnimationSpeed = FMath::Lerp(
				Settings.AnimationSpeed,
				AnimationTweaksDemo.RandomAnimationData, 1.25f * DeltaTime);

			Settings.AnimationWeight = FMath::Lerp(
				Settings.AnimationWeight,
				FMath::Frac(AnimationTweaksDemo.RandomAnimationData * 7.0f) * 0.25f + 0.5f, 1.25f * DeltaTime);


			ATurboSequence_Manager_Lf::TweakAnimation_Concurrent(Settings,
			                                                     AnimationTweaksDemo.TweakingAnimation.RootMotionMesh);
		}
	}

	if (RootMotionDemo.bEnable && RootMotionDemo.MeshData.Num())
	{
		ATurboSequence_Manager_Lf::MoveMeshWithRootMotion_Concurrent(RootMotionDemo.MeshData[0], DeltaTime, true,
		                                                             false);
	}

	if (IKDemo.bEnable && IKDemo.MeshData.Num())
	{
		const FVector& CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
		                                            GetCameraLocation();
		const FRotator& CameraRotation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
		                                             GetCameraRotation();

		const FTransform& OffsetTransform = IKDemo.SpawnOffset;

		const FTransform& MeshTransform =
			ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(IKDemo.MeshData[0]) * OffsetTransform.
			Inverse();

		float Dot = FVector::DotProduct(CameraRotation.Vector(), MeshTransform.GetRotation().Vector());
		bool bIsInView = Dot < 0;
		float WeightAlpha = 5.0f * DeltaTime;
		if (bIsInView)
		{
			WeightAlpha = 2.0f * DeltaTime;
		}

		IKDemo.MasterIKWeight = FTurboSequence_Helper_Lf::Clamp01(
			FMath::Lerp(IKDemo.MasterIKWeight, static_cast<float>(bIsInView), WeightAlpha));


		// This Look At IK need to be done backwards like so many IKs because,
		// If I move first the head and add the movement from spine I end up with
		// a location and rotation offset on head
		// Another thing is we need to add DeltaTime into the IK Because GetIKTransform
		// is sometimes updating the Animation to stay up to date
		for (const TTuple<FName, float>& IKWeight : IKDemo.IKWeights)
		{
			ATurboSequence_Demo_Lf::SolveLookAtIKBone(IKDemo.MeshData[0], IKWeight.Key, CameraLocation,
			                                          IKDemo.MasterIKWeight, IKWeight.Value, DeltaTime,
			                                          OffsetTransform);
		}
	}

	if (SocketsDemo.bEnable && SocketsDemo.MeshData.Num() && IsValid(SocketsDemoMesh))
	{
		FTransform SocketsTransform;
		ATurboSequence_Manager_Lf::GetSocketTransform_Concurrent(SocketsTransform, SocketsDemo.MeshData[0],
		                                                         SocketsDemo.SocketName, DeltaTime);
		SocketsDemoMesh->SetWorldTransform(SocketsTransform);
	}

	if (CustomizationDemo.bEnable && CustomizationDemo.MeshData.Num())
	{
		CustomizationDemo.RandomTimer -= DeltaTime;
		if (CustomizationDemo.RandomTimer < 0)
		{
			CustomizationDemo.RandomTimer = 0.5f; //FMath::RandRange(0.25f, 1.0f);

			FTurboSequence_MeshSpawnData_Lf CustomizableData;
			ATurboSequence_Demo_Lf::GetRandomMeshSpawnData(CustomizableData, CustomizationDemo.CategorizedRootData,
			                                               CustomizationDemo.CategorizeCustomizableData, nullptr);

			ATurboSequence_Manager_Lf::CustomizeMesh_GameThread(
				CustomizationDemo.MeshData[CustomizationDemo.RandomIndex], CustomizableData);

			CustomizationDemo.RandomIndex++;
			CustomizationDemo.RandomIndex = CustomizationDemo.RandomIndex % CustomizationDemo.MeshData.Num();
		}
	}

	if (BlendSpaceDemo.bEnable && BlendSpaceDemo.MeshData.Num() && BlendSpaceDemo.TweakingBlendSpace.
		IsAnimCollectionValid())
	{
		BlendSpaceDemo.RandomTimer -= DeltaTime;
		if (BlendSpaceDemo.RandomTimer < 0)
		{
			BlendSpaceDemo.RandomTimer = 3; //FMath::RandRange(3.0f, 3.0f);

			float RandomX = FMath::Clamp(FMath::RandRange(-150.0f, 150.0f), -100.0f, 100.0f);

			const FVector3f& Position = FVector3f(RandomX, 0, 0);
			LastDemoBlendSpacePosition = Position;

			ATurboSequence_Manager_Lf::TweakBlendSpace_Concurrent(BlendSpaceDemo.TweakingBlendSpace, Position);
		}
	}

	if (CurveDemo.bEnable && CurveDemo.MeshData.Num())
	{
		FTurboSequence_PoseCurveData_Lf CurveData = ATurboSequence_Manager_Lf::GetAnimationCurveValue_Concurrent(
			CurveDemo.MeshData[0], CurveDemo.CurveName, CurveDemo.DemoAnimation);

		if (IsValid(RfCurveDemoMesh))
		{
			//UE_LOG(LogTemp, Warning, TEXT("%f"), CurveData.CurveFrame_0);
			CurveDemo.RfMaterialInstance->SetScalarParameterValue(CurveDemo.MaterialParameterName,
			                                                      CurveData.CurveFrame_0);
		}

		if (IsValid(LfCurveDemoMesh))
		{
			CurveDemo.LfMaterialInstance->SetScalarParameterValue(CurveDemo.MaterialParameterName,
			                                                      1.0f - CurveData.CurveFrame_0);
		}
	}

	if (UpdateGroupsDemo.bEnable && UpdateGroupsDemo.MeshData.Num())
	{
		for (float& AccumulatedDeltaTime : UpdateGroupsDemo.DeltaTime)
		{
			AccumulatedDeltaTime += DeltaTime;
		}

		CurrentUpdateGroupIndex = FMath::Clamp(CurrentUpdateGroupIndex, 1, UpdateGroupsDemo.MeshData.Num());

		int32 DeltaTimeIndex = CurrentUpdateGroupIndex - 1;

		FTurboSequence_UpdateContext_Lf UpdateContext = FTurboSequence_UpdateContext_Lf();
		UpdateContext.GroupIndex = CurrentUpdateGroupIndex;
		//UpdateContext.bCollectGarbageThisFrame = bCollectGarbageThisFrame;

		ATurboSequence_Manager_Lf::SolveMeshes_GameThread(UpdateGroupsDemo.DeltaTime[DeltaTimeIndex], GetWorld(),
		                                                  UpdateContext);
		UpdateGroupsDemo.DeltaTime[DeltaTimeIndex] = 0;

		CurrentUpdateGroupIndex++;
		CurrentUpdateGroupIndex = CurrentUpdateGroupIndex % (UpdateGroupsDemo.MeshData.Num() + 1);
	}

	if (bUpdateManager)
	{
		FTurboSequence_UpdateContext_Lf UpdateContext = FTurboSequence_UpdateContext_Lf();
		UpdateContext.GroupIndex = 0;
		//UpdateContext.bCollectGarbageThisFrame = bCollectGarbageThisFrame;

		ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), UpdateContext);
	}
}

FVector3f ATurboSequence_FeaturesDemo_Lf::GetFeatureDemoBlendSpacePosition_TurboSequence_Lf()
{
	return LastDemoBlendSpacePosition;
}
