// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_Demo_Lf.h"

#include "TurboSequence_Manager_Lf.h"


// Sets default values
ATurboSequence_Demo_Lf::ATurboSequence_Demo_Lf()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.TickGroup = TG_StartPhysics;
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurboSequence_Demo_Lf::BeginPlay()
{
	Super::BeginPlay();

	BindToInput();

	if (AssetData.Num())
	{
		UpdateGroupIndex = QualityGroupIndex + 1;

		AssetDataRuntime.Empty();
		for (FDemoAssetData_Lf& Asset : AssetData)
		{
			Asset.CategorizedRootData.CategorizedData.Empty();
			for (const FDemoMeshSpawnData_Lf& RootAsset : Asset.RootAssets)
			{
				if (IsValid(RootAsset.Asset))
				{
					if (!Asset.bExcludeFromSystem && !RootAsset.bExclude)
					{
						Asset.CategorizedRootData.CategorizedData.Add(RootAsset);
					}
				}
			}
			Asset.CategorizeCustomizableData.Empty();
			for (const FDemoMeshSpawnData_Lf& CustomizableAsset : Asset.CustomizableAssets)
			{
				if (IsValid(CustomizableAsset.Asset))
				{
					if (!Asset.bExcludeFromSystem && !CustomizableAsset.bExclude)
					{
						if (!Asset.CategorizeCustomizableData.Contains(CustomizableAsset.CategoryName))
						{
							FDemoCustomizationContainer_Lf Data;
							Data.CategorizedData.Add(CustomizableAsset);
							Asset.CategorizeCustomizableData.Add(CustomizableAsset.CategoryName, Data);
						}
						else
						{
							Asset.CategorizeCustomizableData[CustomizableAsset.CategoryName].CategorizedData.Add(
								CustomizableAsset);
						}
					}
				}
			}

			if (!Asset.bExcludeFromSystem)
			{
				AssetDataRuntime.Add(&Asset);
			}
		}
		if (!AssetDataRuntime.Num())
		{
			return;
		}

		int32 AmountMeshSqrt = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(AmountOfMeshesToSpawn)));
		if (AmountOfMeshesToSpawn == 1)
		{
			AmountMeshSqrt = 1;
		}
		CurrentMeshCount_Internal = AmountMeshSqrt * AmountMeshSqrt;
		const float Offset = AmountMeshSqrt * DistanceBetweenMeshes * 0.5f;
		LastSpawnLocation_Internal = FVector(Offset, Offset, 0);

		if (CurrentMeshCount_Internal)
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Spawning -> %d Characters, in %d Frames"),
			       CurrentMeshCount_Internal, SpawnFrameDelay + AmountMeshSqrt);

			SpawnFrameDelay_Internal = SpawnFrameDelay;

			GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green,
			                                 FString::Printf(
				                                 TEXT("Spawning -> %d Characters, in %d Frames"),
				                                 CurrentMeshCount_Internal, SpawnFrameDelay + AmountMeshSqrt));

			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurboSequence_Demo_Lf::SpawnCharactersDelayed);
		}
	}
}

// Called every frame
void ATurboSequence_Demo_Lf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AssetDataRuntime.Num())
	{
		if (!ATurboSequence_Manager_Lf::GlobalLibrary.UpdateGroups.IsValidIndex(UpdateGroupIndex))
		{
			return;
		}

		UpdateGroupDeltaTimes.SetNum(ATurboSequence_Manager_Lf::GlobalLibrary.UpdateGroups.Num());

		for (float& Delta : UpdateGroupDeltaTimes)
		{
			Delta += DeltaTime;
		}

		const FVector& CameraLocation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
		                                            GetCameraLocation();
		const FRotator& CameraRotation = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->
		                                             GetCameraRotation();

		TMap<FTurboSequence_MinimalMeshData_Lf, bool> SwitchingGroups;


		FTurboSequence_UpdateContext_Lf UpdateContext = FTurboSequence_UpdateContext_Lf();
		UpdateContext.GroupIndex = UpdateGroupIndex;
		//UpdateContext.bCollectGarbageThisFrame = bCollectGarbageThisFrame;

		// Background Groups
		SolveGroup(UpdateGroupIndex, UpdateGroupDeltaTimes[UpdateGroupIndex], CameraRotation,
		           CameraLocation, SwitchingGroups);

		if (bUpdateManager)
		{
			// Solve Background Group
			ATurboSequence_Manager_Lf::SolveMeshes_GameThread(UpdateGroupDeltaTimes[UpdateGroupIndex], GetWorld(),
			                                                  UpdateContext);


			// Foreground Group
			SolveGroup(QualityGroupIndex, DeltaTime, CameraRotation, CameraLocation, SwitchingGroups);


			for (const TTuple<FTurboSequence_MinimalMeshData_Lf, bool>& Group : SwitchingGroups)
			{
				FDemoMeshWrapper_Lf& Mesh = Meshes[Group.Key];
				if (Group.Value)
				{
					ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(
						Mesh.CurrentUpdateGroupIndex, Mesh.MeshData);
					Mesh.CurrentUpdateGroupIndex = QualityGroupIndex;
					ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(
						Mesh.CurrentUpdateGroupIndex, Mesh.MeshData);
				}
				else
				{
					ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(
						QualityGroupIndex, Mesh.MeshData);
					Mesh.CurrentUpdateGroupIndex = Mesh.DefaultUpdateGroupIndex;
					ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(
						Mesh.CurrentUpdateGroupIndex, Mesh.MeshData);
				}
			}

			// High Quality Solving
			if (ATurboSequence_Manager_Lf::GlobalLibrary.UpdateGroups[QualityGroupIndex].RawIDs.Num())
			{
				UpdateContext.GroupIndex = QualityGroupIndex;

				ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), UpdateContext);
			}
		}

		UpdateGroupDeltaTimes[UpdateGroupIndex] = 0;

		UpdateGroupIndex++;
		UpdateGroupIndex = UpdateGroupIndex % ATurboSequence_Manager_Lf::GlobalLibrary.UpdateGroups.Num();
		UpdateGroupIndex = FMath::Clamp(UpdateGroupIndex, QualityGroupIndex + 1,
		                                ATurboSequence_Manager_Lf::GlobalLibrary.UpdateGroups.Num());
	}
}

void ATurboSequence_Demo_Lf::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ATurboSequence_Demo_Lf::SpawnCharactersDelayed()
{
	if (SpawnFrameDelay_Internal)
	{
		SpawnFrameDelay_Internal--;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurboSequence_Demo_Lf::SpawnCharactersDelayed);
		return;
	}

	const FVector& SpawnerLocation = GetActorLocation();
	//FCriticalSection CriticalSection;

	if (CurrentMeshCount_Internal > 0)
	{
		LastSpawnLocation_Internal += FVector(-DistanceBetweenMeshes, 0, 0);

		int32 AmountCharactersSqrt = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(AmountOfMeshesToSpawn)));
		if (AmountOfMeshesToSpawn == 1)
		{
			AmountCharactersSqrt = 1;
		}
		//const int32 MaxNumMeshes = AmountCharactersSqrt * AmountCharactersSqrt;

		for (int32 Y = 0; Y < AmountCharactersSqrt; ++Y)
		{
			FVector Location = FVector(LastSpawnLocation_Internal.X,
			                           Y * -DistanceBetweenMeshes + LastSpawnLocation_Internal.Y, 0) + SpawnerLocation;
			CurrentMeshCount_Internal--;

			FTransform Transform = FTransform(Location);

			FTurboSequence_MeshSpawnData_Lf SpawnData;
			int32 AssetIndex = FMath::RandRange(0, AssetDataRuntime.Num() - 1);

			FDemoAssetData_Lf* Asset = AssetDataRuntime[AssetIndex];

			GetRandomMeshSpawnData(SpawnData, Asset->CategorizedRootData, Asset->CategorizeCustomizableData,
			                       Asset->FootprintAsset);


			if (const FTurboSequence_MinimalMeshData_Lf& MeshData =
					ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(SpawnData, Transform, GetWorld());
				MeshData.IsMeshDataValid()) // Very Important to check here if it's valid
			{
				FDemoMeshWrapper_Lf Mesh = FDemoMeshWrapper_Lf();
				Mesh.MeshData = MeshData;
				Mesh.RandomRotationTimer = GET1_NUMBER;
				Mesh.AssetDataIndex = AssetIndex;

				const int32 GroupIndex = ATurboSequence_Manager_Lf::GetNumOfAllMeshes_Concurrent() /
					NumInstancesPerUpdateGroup + QualityGroupIndex + 1;

				Mesh.DefaultUpdateGroupIndex = GroupIndex;
				Mesh.CurrentUpdateGroupIndex = GroupIndex;

				ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(GroupIndex, MeshData);
				Meshes.Add(MeshData, Mesh);

				const FTransform& WorldTransformIncludingOffset = Asset->SpawnOffsetTransform *
					ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(Mesh.MeshData);
				ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(
					Mesh.MeshData, WorldTransformIncludingOffset);
			}
		}

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurboSequence_Demo_Lf::SpawnCharactersDelayed);
	}
	else
	{
		uint32 NumMeshes = ATurboSequence_Manager_Lf::GetNumOfAllMeshes_Concurrent();

		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Spawning -> %d Meshes"), NumMeshes);

		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green,
		                                 FString::Printf(TEXT("Spawning -> %d Meshes"), NumMeshes));
	}
}


void ATurboSequence_Demo_Lf::BindToInput()
{
	// Initialize our component
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		// Bind inputs here
		// InputComponent->BindAction("Jump", IE_Pressed, this, &AUnrealisticPawn::Jump);
		// etc...

		// Now hook up our InputComponent to one in a Player
		// Controller, so that input flows down to us
		EnableInput(GetWorld()->GetFirstPlayerController());

		// Bind an action to it
		InputComponent->BindAction
		(
			"Test_LeftMouseButton", // The input identifier (specified in DefaultInput.ini)
			IE_Pressed, // React when button pressed (or on release, etc., if desired)
			this, // The object instance that is going to react to the input
			&ATurboSequence_Demo_Lf::LeftMouseButtonPressed // The function that will fire when input is received
		);
	}
}

void ATurboSequence_Demo_Lf::LeftMouseButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Pressed"));
}

void ATurboSequence_Demo_Lf::SolveGroup(int32 GroupIndex,
                                        float DeltaTime, const FRotator& CameraRotation,
                                        const FVector& CameraLocation,
                                        TMap<FTurboSequence_MinimalMeshData_Lf, bool>& SwitchingGroups)
{
	const int32 NumMeshes = ATurboSequence_Manager_Lf::GetNumMeshDataInUpdateGroup_Concurrent(GroupIndex);
	if (!NumMeshes)
	{
		return;
	}
	FCriticalSection CriticalSection;

	int16 NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;
	int32 NumMeshesPerThread = FMath::CeilToInt(
		static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
	ParallelFor(NumThreads, [&](int32 ThreadsIndex)
	{
		const int32 MeshBaseIndex = ThreadsIndex * NumMeshesPerThread;
		const int32 MeshBaseNum = MeshBaseIndex + NumMeshesPerThread;

		for (int32 Index = MeshBaseIndex; Index < MeshBaseNum; ++Index)
		{
			if (Index >= NumMeshes)
			{
				break;
			}

			const FTurboSequence_MinimalMeshData_Lf& MeshData = ATurboSequence_Manager_Lf::GetMeshDataInUpdateGroupFromIndex_Concurrent(GroupIndex, Index);

			FDemoMeshWrapper_Lf& Mesh = Meshes[MeshData];
			//
			// SolveMesh(Mesh, CriticalSection, CameraRotation, CameraLocation, SwitchingGroups, DeltaTime);

			const FDemoAssetData_Lf* AssetCustomData = AssetDataRuntime[Mesh.AssetDataIndex];
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> MeshAsset =
				ATurboSequence_Manager_Lf::GetMeshAsset_RawID_Concurrent(
					Mesh.MeshData.RootMotionMeshID);

			// Here we make a simple Distance Updating in seconds, if DeltaTimeAccumulator is accumulated
			// higher than DistanceRatioSeconds we update the Mesh
			// this is truly optional and only here for optimization
			float CameraDistance = ATurboSequence_Manager_Lf::GetMeshClosestCameraDistance_Concurrent(
				Mesh.MeshData);
			float DistanceRatioSeconds = MeshAsset->bUseDistanceUpdating
				                             ? CameraDistance / 250000 * MeshAsset->DistanceUpdatingRatio
				                             : 0;

			Mesh.DeltaTimeAccumulator += DeltaTime;
			if (Mesh.DeltaTimeAccumulator > DistanceRatioSeconds)
			{
				if (AssetCustomData->bUseRootMotion)
				{
					ATurboSequence_Manager_Lf::MoveMeshWithRootMotion_Concurrent(
						Mesh.MeshData, Mesh.DeltaTimeAccumulator, true, false);
				}
				else if (AssetCustomData->bUseProgrammableSpeed)
				{
					const TObjectPtr<UAnimSequence> AnimSequence =
						ATurboSequence_Manager_Lf::GetHighestPriorityPlayingAnimation_Concurrent(Mesh.MeshData);

					if (IsValid(MeshAsset->AnimationLibrary))
					{
						FVector Speed = FVector::ZeroVector;
						for (FAnimationLibraryItem_Lf& AnimationLibraryItem : MeshAsset->AnimationLibrary->Animations)
						{
							if (AnimationLibraryItem.Animation == AnimSequence)
							{
								Speed = AnimationLibraryItem.AnimationSeed_CM_Per_Second;
								break;
							}
						}

						FTransform MeshTransform = ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(
							Mesh.MeshData);

						MeshTransform.SetLocation(
							MeshTransform.GetLocation() + MeshTransform.GetRotation().RotateVector(Speed * DeltaTime));

						ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(Mesh.MeshData, MeshTransform);
					}
				}

				if (AssetCustomData->bUseRandomRotation)
				{
					Mesh.RandomRotationTimer -= Mesh.DeltaTimeAccumulator;
					if (Mesh.RandomRotationTimer < 0)
					{
						Mesh.RandomRotationTimer = FMath::RandRange(2.0f, 7.0f);
						const float RandomYaw = FMath::RandRange(-180.0f, 180.0f);

						Mesh.RandomRotationYaw = RandomYaw;
					}

					FTransform MeshTransform = ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(
						Mesh.MeshData);
					FRotator Rotation = MeshTransform.Rotator();

					Rotation.Yaw = FRotator::NormalizeAxis(
						FMath::Lerp(Rotation.Yaw, static_cast<double>(Mesh.RandomRotationYaw),
						            static_cast<double>(Mesh.DeltaTimeAccumulator)));

					//MeshTransform.SetRotation(Rotation.Quaternion());
					ATurboSequence_Manager_Lf::SetMeshWorldSpaceLocationRotationScale_Concurrent(
						Mesh.MeshData, MeshTransform.GetLocation(), Rotation.Quaternion(), FVector::OneVector);
				}

				if (AssetCustomData->bUseRandomAnimation)
				{
					Mesh.RandomAnimationTimer -= Mesh.DeltaTimeAccumulator;
					if (Mesh.RandomAnimationTimer < 0)
					{
						if (IsValid(MeshAsset->AnimationLibrary)) // TODO: Remove
						{
							if (int32 NumAnimations = MeshAsset->AnimationLibrary->Animations.Num())
							{
								int32 RandomAnimation = FMath::RandRange(0, NumAnimations - 1);
								if (NumAnimations > 1)
								{
									while (ATurboSequence_Manager_Lf::GetHighestPriorityPlayingAnimation_Concurrent(
											Mesh.MeshData) == MeshAsset->AnimationLibrary->Animations[RandomAnimation].
										Animation)
									{
										RandomAnimation = FMath::RandRange(0, NumAnimations - 1);
									}
								}
								FTurboSequence_AnimPlaySettings_Lf PlaySettings = FTurboSequence_AnimPlaySettings_Lf();
								Mesh.CurrentAnimation_0 = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(
									Mesh.MeshData, MeshAsset->AnimationLibrary->Animations[RandomAnimation].Animation,
									PlaySettings);

								if (AssetCustomData->bUseLayer)
								{
									int32 RandomAnimation2 = FMath::RandRange(0, NumAnimations - 1);
									if (NumAnimations > 1)
									{
										while (
											ATurboSequence_Manager_Lf::GetHighestPriorityPlayingAnimation_Concurrent(
												Mesh.MeshData) == MeshAsset->AnimationLibrary->Animations[
												RandomAnimation2].Animation)
										{
											RandomAnimation2 = FMath::RandRange(0, NumAnimations - 1);
										}
									}
									PlaySettings.AnimationPlayTimeInSeconds = FMath::RandRange(0.0f, 0.5f);
									PlaySettings.BoneLayerMasks = AssetCustomData->BoneLayers;
									//PlaySettings.Animation = MeshAsset->AnimationLibrary->Animations[RandomAnimation2];
									Mesh.CurrentAnimation_1 = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(
										Mesh.MeshData,
										MeshAsset->AnimationLibrary->Animations[RandomAnimation2].Animation,
										PlaySettings);
								}
							}
						}

						Mesh.RandomAnimationTimer = FMath::RandRange(2.0f, 7.0f);
						Mesh.RandomAnimationData_0 = FMath::RandRange(0.9f, 1.75f);
						Mesh.RandomAnimationData_1 = FMath::RandRange(0.9f, 1.75f);
					}

					if (AssetCustomData->bUseCustomData)
					{
						Mesh.CustomDataTimer -= Mesh.DeltaTimeAccumulator;
						if (Mesh.CustomDataTimer < 0)
						{
							Mesh.CustomDataTimer = FMath::RandRange(2.0f, 5.0f);

							const FLinearColor& RandomColor = FLinearColor::MakeRandomColor();

							ATurboSequence_Manager_Lf::SetCustomDataToInstance_Concurrent(
								Mesh.MeshData, 4, RandomColor.R);
							ATurboSequence_Manager_Lf::SetCustomDataToInstance_Concurrent(
								Mesh.MeshData, 5, RandomColor.G);
							ATurboSequence_Manager_Lf::SetCustomDataToInstance_Concurrent(
								Mesh.MeshData, 6, RandomColor.B);
						}
					}
				}

				float HybridAnimationDistance = 0;
				if (const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset =
					ATurboSequence_Manager_Lf::GetFootprintAsset_Concurrent(Mesh.MeshData); IsValid(FootprintAsset))
				{
					HybridAnimationDistance = FootprintAsset->HybridModeAnimationDrawRangeUEInstance;
				}


				// IK is expensive on the CPU, we only do IK for 100 Meters Radius around the camera
				// and only if the mesh is visible by the Camera Frustum
				if (AssetCustomData->bUseIK && CameraDistance > HybridAnimationDistance && CameraDistance < 10000 &&
					ATurboSequence_Manager_Lf::GetIsMeshVisibleInCameraFrustum_Concurrent(Mesh.MeshData))
				{
					const FTransform& OffsetTransform = AssetCustomData->SpawnOffsetTransform;

					const FTransform& MeshTransform =
						ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(Mesh.MeshData) *
						OffsetTransform.Inverse();

					float Dot = FVector::DotProduct(CameraRotation.Vector(),
					                                MeshTransform.GetRotation().Vector());
					bool bIsInView = Dot < 0;

					Mesh.IKWeight = FTurboSequence_Helper_Lf::Clamp01(FMath::Lerp(
						Mesh.IKWeight, static_cast<float>(bIsInView),
						bIsInView ? 2.0f * Mesh.DeltaTimeAccumulator : 5.0f * Mesh.DeltaTimeAccumulator));


					// This Look At IK need to be done backwards like so many IKs because,
					// If I move first the head and add the movement from spine I end up with
					// a location and rotation offset on head
					// Another thing is we need to add DeltaTime into the IK Because GetIKTransform
					// is sometimes updating the Animation to stay up to date
					SolveLookAtIKBone(Mesh.MeshData, AssetCustomData->Spine1Bone, CameraLocation, Mesh.IKWeight,
					                  AssetCustomData->Spine1IKStiffness, DeltaTime,
					                  OffsetTransform);
					SolveLookAtIKBone(Mesh.MeshData, AssetCustomData->Spine2Bone, CameraLocation, Mesh.IKWeight,
					                  AssetCustomData->Spine2IKStiffness, DeltaTime,
					                  OffsetTransform);
					SolveLookAtIKBone(Mesh.MeshData, AssetCustomData->NeckBone, CameraLocation, Mesh.IKWeight,
					                  AssetCustomData->NeckIKStiffness, DeltaTime,
					                  OffsetTransform);
					SolveLookAtIKBone(Mesh.MeshData, AssetCustomData->HeadBone, CameraLocation, Mesh.IKWeight,
					                  AssetCustomData->HeadIKStiffness, DeltaTime,
					                  OffsetTransform);
				}

				if (CameraDistance < RadiusOfHighQualitySolving)
				{
					if (Mesh.CurrentUpdateGroupIndex == Mesh.DefaultUpdateGroupIndex)
					{
						CriticalSection.Lock();
						SwitchingGroups.Add(Mesh.MeshData, true);
						CriticalSection.Unlock();
					}
				}
				else
				{
					if (Mesh.CurrentUpdateGroupIndex == QualityGroupIndex)
					{
						CriticalSection.Lock();
						SwitchingGroups.Add(Mesh.MeshData, false);
						CriticalSection.Unlock();
					}
				}

				Mesh.DeltaTimeAccumulator = 0;
			}
		}
	}, EParallelForFlags::BackgroundPriority);
}

void ATurboSequence_Demo_Lf::GetRandomMeshSpawnData(FTurboSequence_MeshSpawnData_Lf& Data,
                                                    const FDemoCustomizationContainer_Lf& CategorizedRootData,
                                                    const TMap<FName, FDemoCustomizationContainer_Lf>&
                                                    CategorizeCustomizableData,
                                                    const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset)
{
	Data = FTurboSequence_MeshSpawnData_Lf();

	if (int32 RandomRootMesh = FMath::RandRange(0, CategorizedRootData.CategorizedData.Num() - 1);
		CategorizedRootData.CategorizedData.IsValidIndex(RandomRootMesh))
	{
		Data.RootMotionMesh.Mesh = CategorizedRootData.CategorizedData[RandomRootMesh].Asset;
		Data.RootMotionMesh.FootprintAsset = FootprintAsset;
		if (CategorizedRootData.CategorizedData[RandomRootMesh].Materials.Num())
		{
			TArray<FName> Names;
			CategorizedRootData.CategorizedData[RandomRootMesh].Materials.GetKeys(Names);
			int32 RandomRootMaterial = FMath::RandRange(
				0, CategorizedRootData.CategorizedData[RandomRootMesh].Materials.Num() - 1);
			Data.RootMotionMesh.OverrideMaterials = CategorizedRootData.CategorizedData[RandomRootMesh].Materials[
				Names[RandomRootMaterial]].Materials;
		}
	}
	for (const TTuple<FName, FDemoCustomizationContainer_Lf>& Customizable : CategorizeCustomizableData)
	{
		if (Customizable.Value.CategorizedData.Num())
		{
			int32 RandomMesh = FMath::RandRange(0, Customizable.Value.CategorizedData.Num() - 1);
			FTurboSequence_MeshMetaData_Lf MeshData = FTurboSequence_MeshMetaData_Lf();
			MeshData.Mesh = Customizable.Value.CategorizedData[RandomMesh].Asset;
			MeshData.FootprintAsset = FootprintAsset;
			if (Customizable.Value.CategorizedData[RandomMesh].Materials.Num())
			{
				TArray<FName> Names;
				Customizable.Value.CategorizedData[RandomMesh].Materials.GetKeys(Names);
				int32 RandomMaterial = FMath::RandRange(
					0, Customizable.Value.CategorizedData[RandomMesh].Materials.Num() - 1);
				MeshData.OverrideMaterials = Customizable.Value.CategorizedData[RandomMesh].Materials[Names[
					RandomMaterial]].Materials;
			}
			Data.CustomizableMeshes.Add(MeshData);
		}
	}
}

void ATurboSequence_Demo_Lf::SolveLookAtIKBone(const FTurboSequence_MinimalMeshData_Lf& MeshData, const FName& BoneName,
                                               const FVector& TargetLocation, float Weight, float OffsetWeight,
                                               float AnimationDeltaTime,
                                               const FTransform& StartOffsetTransform)
{
	FTransform IKTransform;
	ATurboSequence_Manager_Lf::GetIKTransform_Concurrent(IKTransform, MeshData, BoneName, AnimationDeltaTime,
	                                                     EBoneSpaces::WorldSpace);

	FTransform ReferencePoseTransform;
	ATurboSequence_Manager_Lf::GetReferencePoseTransform_Concurrent(ReferencePoseTransform, MeshData, BoneName,
	                                                                ETurboSequence_TransformSpace_Lf::ComponentSpace);
	ReferencePoseTransform *= StartOffsetTransform;

	const FQuat& WantedRotation = FRotationMatrix::MakeFromX(TargetLocation - IKTransform.GetLocation()).ToQuat();
	IKTransform.SetRotation(FQuat::Slerp(IKTransform.GetRotation(),
	                                     WantedRotation * ReferencePoseTransform.GetRotation(),
	                                     Weight * OffsetWeight));


	ATurboSequence_Manager_Lf::SetIKTransform_Concurrent(MeshData, BoneName, IKTransform, EBoneSpaces::WorldSpace);
}

void ATurboSequence_Demo_Lf::SolveBlueprintDemoMeshesInCpp(const TArray<FTurboSequence_MinimalMeshData_Lf>& MeshData,
                                                           TArray<FVector4>& AnimData, UAnimSequence* AnimA,
                                                           UAnimSequence* AnimB,
                                                           const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsA,
                                                           const FTurboSequence_AnimPlaySettings_Lf& AnimSettingsB,
                                                           const float DeltaTime, const bool bMultiThreaded)
{
	auto RunnerFunction = [&](const FTurboSequence_MinimalMeshData_Lf& MeshID, int32 Index)
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
		RanRotationTransform.SetRotation(FMath::RInterpConstantTo(CurrentRotation,
		                                                          FRotator(CurrentRotation.Pitch, AnimDataOut.Z,
		                                                                   CurrentRotation.Roll), DeltaTime,
		                                                          3).Quaternion());

		ATurboSequence_Manager_Lf::SetMeshWorldSpaceLocationRotationScale_Concurrent(
			MeshID, RanRotationTransform.GetLocation(), RanRotationTransform.GetRotation(),
			RanRotationTransform.GetScale3D());

		ATurboSequence_Manager_Lf::MoveMeshWithRootMotion_Concurrent(MeshID, DeltaTime, true, false);
	};

	const int32 NumMeshes = FMath::Min(MeshData.Num(), AnimData.Num());
	if (!NumMeshes)
	{
		return;
	}

	if (bMultiThreaded)
	{
		int16 NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;
		int32 NumMeshesPerThread_Background = FMath::CeilToInt(
			static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
		ParallelFor(NumThreads, [&](int32 ThreadsIndex)
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
