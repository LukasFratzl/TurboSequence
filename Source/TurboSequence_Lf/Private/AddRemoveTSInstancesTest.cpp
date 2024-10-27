// Fill out your copyright notice in the Description page of Project Settings.


#include "AddRemoveTSInstancesTest.h"

#include "TurboSequence_Manager_Lf.h"


// Sets default values
AAddRemoveTSInstancesTest::AAddRemoveTSInstancesTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AAddRemoveTSInstancesTest::BeginPlay()
{
	Super::BeginPlay();

	int32 NumMeshesSpawned = 0;

	int32 AmountMeshSqrt = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(NumSpawnInstances)));
	if (NumSpawnInstances == 1)
	{
		AmountMeshSqrt = 1;
	}

	const float Offset = AmountMeshSqrt * DistanceBetweenMeshes * 0.5f;
	FVector TempLocationVector = FVector(Offset, Offset, 0);

	for (int32 X = 0; X < AmountMeshSqrt; ++X)
	{
		for (int32 Y = 0; Y < AmountMeshSqrt; ++Y)
		{

			const int32 RandSpawnData = FMath::RandRange(0, SpawnData.Num() - 1);

			FVector Location = FVector(X * -DistanceBetweenMeshes + TempLocationVector.X,Y * -DistanceBetweenMeshes + TempLocationVector.Y, 0);
			FTransform Transform = FTransform(Location);

			FTestInstance TestInstance;
			TestInstance.UsedSpawnData = SpawnData[RandSpawnData];
			TestInstance.SpawnTransform = Transform;

			TestInstance.MeshData = ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(TestInstance.UsedSpawnData, TestInstance.SpawnTransform, GetWorld());
			if (TestInstance.MeshData.IsMeshDataValid())
			{
				ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, TestInstance.MeshData);

				FTurboSequence_AnimPlaySettings_Lf AnimPlaySettings;
				ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(TestInstance.MeshData, AnimSequence, AnimPlaySettings);

				TestInstance.Bounds = ATurboSequence_Manager_Lf::GlobalLibrary.PerReferenceData[ATurboSequence_Manager_Lf::GetMeshAsset_RawID_Concurrent(TestInstance.MeshData.RootMotionMeshID)].FirstValidMeshLevelOfDetail->GetBounds();
				TestInstance.bSpawned = true;
				TestInstances.Add(TestInstance);

				NumMeshesSpawned++;
			}


		}
	}


	UE_LOG(LogTemp, Warning, TEXT("Spawned %d Meshes"), NumMeshesSpawned)

}

// Called every frame
void AAddRemoveTSInstancesTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TestInstances.Num())
	{

		const int32 NumInstances = TestInstances.Num();
		for (int32 i = 0; i < NumInstances; ++i)
		{
			FTestInstance& TestInstance = TestInstances[i];

			// Little Frustum Culling
			const FVector& MeshLocation = TestInstance.SpawnTransform.GetLocation();
			const FBoxSphereBounds& Bounds = TestInstance.Bounds;
			const FBox Box(MeshLocation - Bounds.BoxExtent, MeshLocation + Bounds.BoxExtent);

			bool bIsVisibleOnAnyCamera = false;
			for (const FCameraView_Lf& View : ATurboSequence_Manager_Lf::GlobalLibrary.CameraViews)
			{
				if (FTurboSequence_Helper_Lf::Box_Intersects_With_Frustum(Box, View.Planes_Internal,
																		  View.InterpolatedCameraTransform_Internal,
																		  Bounds.SphereRadius))
				{
					bIsVisibleOnAnyCamera = true;
					break;
				}

				// if (View.CameraTransform.GetRotation().GetForwardVector().Dot((MeshLocation - View.CameraTransform.GetLocation()).GetSafeNormal()) > 0)
				// {
				// 	bIsVisibleOnAnyCamera = true;
				// }
			}

			if (bIsVisibleOnAnyCamera)
			{
				if (!TestInstance.bSpawned)
				{
					TestInstance.MeshData = ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(TestInstance.UsedSpawnData, TestInstance.SpawnTransform, GetWorld());
					if (TestInstance.MeshData.IsMeshDataValid())
					{
						ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(0, TestInstance.MeshData);

						FTurboSequence_AnimPlaySettings_Lf AnimPlaySettings;
						ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(TestInstance.MeshData, AnimSequence, AnimPlaySettings);
						TestInstance.bSpawned = true;
					}
				}
			}
			else
			{
				if (TestInstance.bSpawned)
				{

					if (ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread(TestInstance.MeshData, GetWorld()))
					{
						ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(0, TestInstance.MeshData);
						TestInstance.MeshData = FTurboSequence_MinimalMeshData_Lf(false);
						TestInstance.bSpawned = false;
					}
				}
			}
		}

		FTurboSequence_UpdateContext_Lf UpdateContext;
		UpdateContext.GroupIndex = 0;
		ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), UpdateContext);
	}
}

