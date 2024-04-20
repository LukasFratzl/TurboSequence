// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_MeshTester_Lf.h"

#include "TurboSequence_Manager_Lf.h"


// Sets default values
ATurboSequence_MeshTester_Lf::ATurboSequence_MeshTester_Lf()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ATurboSequence_MeshTester_Lf::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATurboSequence_MeshTester_Lf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TestMesh(DeltaTime);
}

void ATurboSequence_MeshTester_Lf::TestMesh(float DeltaTime)
{
	if (SpawnData.GetHash() != LastSpawnData.GetHash())
	{
		LastSpawnData = SpawnData;

		if (CurrentMeshID.IsMeshDataValid())
		{
			ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(
				MeshUpdateContext.GroupIndex, CurrentMeshID);

			ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread(CurrentMeshID, GetWorld());
		}

		if (SpawnData.IsSpawnDataValid())
		{
			CurrentMeshID = ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
				SpawnData, GetActorTransform(), GetWorld());
			if (CurrentMeshID.IsMeshDataValid())
			{
				ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(
					MeshUpdateContext.GroupIndex, CurrentMeshID);

				CurrentAnimationID = ATurboSequence_Manager_Lf::PlayAnimation_Concurrent(
					CurrentMeshID, MeshAnimation, MeshAnimationSettings);
			}
		}
	}

	ATurboSequence_Manager_Lf::SolveMeshes_GameThread(DeltaTime, GetWorld(), MeshUpdateContext);
}
