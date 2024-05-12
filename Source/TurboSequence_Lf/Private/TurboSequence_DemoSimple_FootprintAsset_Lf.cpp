// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_DemoSimple_FootprintAsset_Lf.h"

#include "TurboSequence_Manager_Lf.h"

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnSetMeshIsVisible_Concurrent(
	ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride, const bool bDefaultVisibility, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	if (MeshDataCollection.Contains(MeshID) && IsValid(MeshDataCollection[MeshID]) && MeshDataCollection[MeshID]->
		IsVisible())
	{
		IsVisibleOverride = ETurboSequence_IsVisibleOverride_Lf::IsNotVisible;
	}

	Super::OnSetMeshIsVisible_Concurrent(IsVisibleOverride, bDefaultVisibility, MeshID, ThreadContext);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride,
	const int32 MeshID, const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	if (MeshDataCollection.Contains(MeshID) && IsValid(MeshDataCollection[MeshID]) && MeshDataCollection[MeshID]->
		IsVisible())
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
				MeshDataCollection.Add(MeshID, MeshComponent);
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
		MeshDataCollection[MeshID]->GetOwner()->Destroy();

		MeshDataCollection.Remove(MeshID);
	}

	Super::OnRemovedMeshInstance_GameThread(MeshID, FromAsset);
}

void UTurboSequence_DemoSimple_FootprintAsset_Lf::OnManagerUpdated_GameThread(const float DeltaTime)
{
	for (const TTuple<uint32, TObjectPtr<USkinnedMeshComponent>> Mesh : MeshDataCollection)
	{
		if (IsValid(Mesh.Value))
		{
			const float CameraDistance =
				ATurboSequence_Manager_Lf::GetMeshClosestCameraDistance_RawID_Concurrent(Mesh.Key);

			const bool bShouldDrawUEMesh = CanShowUEMesh(CameraDistance);

			Mesh.Value->SetVisibility(bShouldDrawUEMesh);

			if (bShouldDrawUEMesh)
			{
				ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_RawID_Concurrent(
					Mesh.Key, Mesh.Value->GetComponentTransform());
			}
			else
			{
				Mesh.Value->SetWorldTransform(ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_RawID_Concurrent(Mesh.Key));
			}
		}
	}

	Super::OnManagerUpdated_GameThread(DeltaTime);
}
