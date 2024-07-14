// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_DemoSimple_FootprintAsset_Lf.h"

#include "TurboSequence_Manager_Lf.h"

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnSetMeshIsVisible_Concurrent(
	ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride, const bool bDefaultVisibility, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	if (MeshDataCollection.Contains(MeshID) && IsValid(MeshDataCollection[MeshID].Mesh) && MeshDataCollection[MeshID].
		bIsUEVisible)
	{
		IsVisibleOverride = ETurboSequence_IsVisibleOverride_Lf::ScaleToZero;
	}

	Super::OnSetMeshIsVisible_Concurrent(IsVisibleOverride, bDefaultVisibility, MeshID, ThreadContext);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride,
	const int32 MeshID, const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	if (MeshDataCollection.Contains(MeshID) && IsValid(MeshDataCollection[MeshID].Mesh) && MeshDataCollection[MeshID].
		bIsUEVisible)
	{
		bIsUpdatingLodOverride = false;
	}

	Super::OnSetMeshIsUpdatingLod_Concurrent(bIsUpdatingLodOverride, MeshID, ThreadContext);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnAddedMeshInstance_GameThread(const int32 MeshID,
	const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset)
{
	if (!MeshDataCollection.Contains(MeshID) && IsValid(UEMeshClass))
	{
		const TObjectPtr<AActor> Mesh =
			UTurboSequence_Helper_BlueprintFunctions_Lf::TurboSequence_GetWorldFromStaticFunction()->SpawnActor<AActor>(
				UEMeshClass, ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_RawID_Concurrent(MeshID)
			);

		if (IsValid(Mesh))
		{
			const TObjectPtr<USkinnedMeshComponent> MeshComponent = Cast<USkinnedMeshComponent>(
				Mesh->GetComponentByClass(USkinnedMeshComponent::StaticClass()));

			if (IsValid(MeshComponent))
			{
				FDemoMeshData_Lf MeshData;
				MeshData.Mesh = MeshComponent;;
				MeshData.FrameDelay = 0;
				MeshDataCollection.Add(MeshID, MeshData);

				MeshData.ActorConnection = Cast<UTurboSequence_MeshActorConnection_Lf>(Mesh->
					GetComponentByClass(
						UTurboSequence_MeshActorConnection_Lf::StaticClass()));

				if (IsValid(MeshData.ActorConnection))
				{
					MeshData.ActorConnection->OnMeshDataIDSend_GameThread(
						ATurboSequence_Manager_Lf::GetMeshDataFromMeshID_Concurrent(MeshID));
				}
			}
		}
	}

	Super::OnAddedMeshInstance_GameThread(MeshID, FromAsset);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnRemovedMeshInstance_GameThread(const int32 MeshID,
	const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset)
{
	if (MeshDataCollection.Contains(MeshID))
	{
		if (IsValid(MeshDataCollection[MeshID].Mesh))
		{
			MeshDataCollection[MeshID].Mesh->GetOwner()->Destroy();
		}

		MeshDataCollection.Remove(MeshID);
	}

	Super::OnRemovedMeshInstance_GameThread(MeshID, FromAsset);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnPostManagerUpdated_GameThread(const float DeltaTime)
{
	for (TTuple<int32, FDemoMeshData_Lf>& Mesh : MeshDataCollection)
	{
		if (IsValid(Mesh.Value.Mesh))
		{
			if (IsValid(Mesh.Value.ActorConnection))
			{
				Mesh.Value.ActorConnection->OnFootprintAssetTick_GameThread(DeltaTime);
			}

			const float CameraDistance =
				ATurboSequence_Manager_Lf::GetMeshClosestCameraDistance_RawID_Concurrent(Mesh.Key);

			Mesh.Value.bIsUEVisible = CanShowUEMesh(Mesh.Key, CameraDistance);


			if (Mesh.Value.bIsUEVisible)
			{
				ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_RawID_Concurrent(
					Mesh.Key, Mesh.Value.Mesh->GetComponentTransform());

				Mesh.Value.Mesh->SetVisibility(true);
				Mesh.Value.FrameDelay = 0;
			}
			else
			{
				FTransform Transform = ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_RawID_Concurrent(Mesh.Key);
				Transform.SetScale3D(Mesh.Value.Mesh->GetComponentScale());
				ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_RawID_Concurrent(
					Mesh.Key, Transform);

				if (Mesh.Value.FrameDelay < 3)
				{
					Mesh.Value.Mesh->SetVisibility(true);
					Mesh.Value.FrameDelay++;
				}
				else
				{
					Mesh.Value.Mesh->SetVisibility(false);
				}
			}
		}
	}

	Super::OnPostManagerUpdated_GameThread(DeltaTime);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnManagerEndPlay_GameThread(const EEndPlayReason::Type EndPlayReason)
{
	MeshDataCollection.Empty();

	Super::OnManagerEndPlay_GameThread(EndPlayReason);
}
