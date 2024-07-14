// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_MeshActorConnection_Lf.h"

#include "TurboSequence_Manager_Lf.h"


// Sets default values for this component's properties
UTurboSequence_MeshActorConnection_Lf::UTurboSequence_MeshActorConnection_Lf()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTurboSequence_MeshActorConnection_Lf::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UTurboSequence_MeshActorConnection_Lf::TickComponent(float DeltaTime, ELevelTick TickType,
                                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (GetTsMeshData().IsMeshDataValid() && IsValid(FootprintAsset))
	{

		for (USkinnedMeshComponent* MeshComponent : SkinnedMeshComponents)
		{
			if (FootprintAsset->SyncData.Contains(MeshComponent->GetSkinnedAsset()))
			{
				const FTurboSequence_MeshSyncData_Lf& Data = FootprintAsset->SyncData[MeshComponent->GetSkinnedAsset()];
				if (Data.bIsWorldTransformSync)
				{
					ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(
						GetTsMeshData(), MeshComponent->GetComponentTransform());
				}
			}
		}
	}
}

void UTurboSequence_MeshActorConnection_Lf::OnMeshDataIDSend_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData)
{
	if (MeshData.IsMeshDataValid())
	{
		ActorTSMeshDataID = MeshData;
		FootprintAsset = ATurboSequence_Manager_Lf::GetFootprintAsset_Concurrent(MeshData);
		GetOwner()->GetComponents(USkinnedMeshComponent::StaticClass(), SkinnedMeshComponents);
	}
}

void UTurboSequence_MeshActorConnection_Lf::OnFootprintAssetTick_GameThread(const float DeltaTime)
{
}

