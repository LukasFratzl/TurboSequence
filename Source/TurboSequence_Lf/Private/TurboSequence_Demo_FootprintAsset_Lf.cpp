// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_Demo_FootprintAsset_Lf.h"

#include "TurboSequence_Helper_Lf.h"
#include "TurboSequence_Manager_Lf.h"

void UTurboSequence_Demo_FootprintAsset_Lf::OnManagerEndPlay_GameThread(const EEndPlayReason::Type EndPlayReason)
{
	Super::OnManagerEndPlay_GameThread(EndPlayReason);

	MeshesOpen.Empty();
	MeshesClosed.Empty();
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnSetMeshIsVisible_Concurrent(
	ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
	const bool bDefaultVisibility, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshIsVisible_Concurrent(IsVisibleOverride, bDefaultVisibility, MeshID, ThreadContext);

	if (MeshesOpen.Contains(MeshID) && MeshesOpen[MeshID].bIsInUERange && MeshesOpen[MeshID].
		FadeTimeRuntime <= 0)
	{
		IsVisibleOverride = ETurboSequence_IsVisibleOverride_Lf::ScaleToZero;
	}
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnSetMeshIsUpdatingLod_Concurrent(
	bool& bIsUpdatingLodOverride, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshIsUpdatingLod_Concurrent(bIsUpdatingLodOverride, MeshID, ThreadContext);

	if (MeshesOpen.Contains(MeshID) && MeshesOpen[MeshID].bIsInUERange && MeshesOpen[MeshID].
		FadeTimeRuntime <= 0)
	{
		bIsUpdatingLodOverride = false;
	}
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnSetMeshIsAnimated_Concurrent(
	ETurboSequence_IsAnimatedOverride_Lf& IsAnimatedOverride, const bool bDefaultIsAnimated, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshIsAnimated_Concurrent(IsAnimatedOverride, bDefaultIsAnimated, MeshID, ThreadContext);

	if (MeshesOpen.Contains(MeshID) && MeshesOpen[MeshID].bIsInUERange && MeshesOpen[MeshID].
		FadeTimeRuntime <= 0)
	{
		IsAnimatedOverride = ETurboSequence_IsAnimatedOverride_Lf::IsAnimated;
	}
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnAddedMeshInstance_GameThread(const int32 MeshID,
                                                                           const TObjectPtr<UTurboSequence_MeshAsset_Lf>
                                                                           &
                                                                           FromAsset)
{
	Super::OnAddedMeshInstance_GameThread(MeshID, FromAsset);

	FDemoMeshInstance_Lf& Instance = MeshesOpen.FindOrAdd(MeshID);
	Instance.bInit = false;
	Instance.bIsInUERange = false;
	Instance.FadeTimeRuntime = 0;
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnRemovedMeshInstance_GameThread(const int32 MeshID,
                                                                             const TObjectPtr<
	                                                                             UTurboSequence_MeshAsset_Lf>
                                                                             & FromAsset)
{
	Super::OnRemovedMeshInstance_GameThread(MeshID, FromAsset);

	if (MeshesOpen.Contains(MeshID))
	{
		MeshesOpen.Remove(MeshID);
	}
	if (MeshesClosed.Contains(MeshID))
	{
		if (IsValid(MeshesClosed[MeshID].Mesh))
		{
			MeshesClosed[MeshID].Mesh->Destroy();
		}
		MeshesClosed.Remove(MeshID);
	}
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnMeshPreSolveAnimationMeta_Concurrent(
	const int32 MeshID, const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnMeshPreSolveAnimationMeta_Concurrent(MeshID, ThreadContext);

	FDemoMeshInstance_Lf& MeshOpen = MeshesOpen[MeshID];
	const FTurboSequence_MinimalMeshData_Lf& MeshData =
		ATurboSequence_Manager_Lf::GetMeshDataFromMeshID_Concurrent(MeshID);

	const bool bIsInUERange = CanShowUEMesh(
		MeshID, ATurboSequence_Manager_Lf::GetMeshClosestCameraDistance_RawID_Concurrent(MeshID));
	if (bIsInUERange != MeshOpen.bIsInUERange || (!MeshOpen.bInit && bIsInUERange))
	{
		MeshOpen.FadeTimeRuntime = FadeTime;

		ThreadContext->LockThread();
		FDemoMeshInstance_Lf& MeshClosed = MeshesClosed.FindOrAdd(MeshData.RootMotionMeshID);
		ThreadContext->UnlockThread();
		MeshClosed.FadeTimeRuntime = FadeTime;
		MeshClosed.NumFrames = 0;
	}

	MeshOpen.FadeTimeRuntime -= LastDeltaTime;
	MeshOpen.bIsInUERange = bIsInUERange;
	MeshOpen.bInit = true;

	ThreadContext->LockThread();
	if (MeshesClosed.Contains(MeshData.RootMotionMeshID))
	{
		FDemoMeshInstance_Lf& MeshClosed = MeshesClosed[MeshData.RootMotionMeshID];
		MeshClosed.FadeTimeRuntime -= LastDeltaTime;
		MeshClosed.bIsInUERange = bIsInUERange;
		MeshClosed.bInit = true;
	}
	ThreadContext->UnlockThread();
}

void UTurboSequence_Demo_FootprintAsset_Lf::OnPostManagerUpdated_GameThread(const float DeltaTime)
{
	Super::OnPostManagerUpdated_GameThread(DeltaTime);

	LastDeltaTime = DeltaTime;

	TArray<int32> MeshIDsToRemove;
	for (TTuple<int32, FDemoMeshInstance_Lf>& Mesh : MeshesClosed)
	{
		const FTurboSequence_MinimalMeshData_Lf& MeshData =
			ATurboSequence_Manager_Lf::GetMeshDataFromMeshID_Concurrent(Mesh.Key);

		if (IsValid(MeshActor) && !IsValid(Mesh.Value.Mesh))
		{
			const FTransform& SpawnLocation =
				ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_RawID_Concurrent(Mesh.Key);
			Mesh.Value.Mesh = UTurboSequence_Helper_BlueprintFunctions_Lf::TurboSequence_GetWorldFromStaticFunction()->
				SpawnActor<AActor>(MeshActor, SpawnLocation);
		}

		if (IsValid(Mesh.Value.Mesh))
		{
			if (Mesh.Value.FadeTimeRuntime <= 0 && !Mesh.Value.bIsInUERange)
			{
				Mesh.Value.Mesh->Destroy();
				MeshIDsToRemove.Add(Mesh.Key);
				continue;
			}

			TArray<USkinnedMeshComponent*> SkinnedMeshRenderers;
			Mesh.Value.Mesh->GetComponents(USkinnedMeshComponent::StaticClass(), SkinnedMeshRenderers);

			if (!SkinnedMeshRenderers.Num())
			{
				continue;
			}

			ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(
				MeshData, SkinnedMeshRenderers[0]->GetComponentTransform());


			// 1 for the root mesh
			FadeMesh(MeshData.RootMotionMeshID, Mesh.Value, SkinnedMeshRenderers[0]);

			for (const int32 MeshID : MeshData.CustomizableMeshIDs)
			{
				for (USkinnedMeshComponent* MeshComponent : SkinnedMeshRenderers)
				{
					if (MeshComponent->GetSkinnedAsset() == ATurboSequence_Manager_Lf::GetMeshAsset_RawID_Concurrent(MeshID)
						->ReferenceMeshNative)
					{
						FadeMesh(MeshData.RootMotionMeshID, Mesh.Value, MeshComponent);
						break;
					}
				}
			}

			Mesh.Value.NumFrames++;
			Mesh.Value.NumFrames = FMath::Clamp(Mesh.Value.NumFrames, 0, 10);
		}
	}

	for (const int32 ToRemove : MeshIDsToRemove)
	{
		MeshesClosed.Remove(ToRemove);
	}
}

void UTurboSequence_Demo_FootprintAsset_Lf::FadeMesh(const int32 MeshID, const FDemoMeshInstance_Lf& Instance,
                                                     const TObjectPtr<USkinnedMeshComponent>& Mesh)
{
	if (IsValid(Mesh))
	{
		const bool FadeActive = Instance.FadeTimeRuntime > 0;
		Mesh->SetVisibility(!FadeActive || (Instance.NumFrames < 3 && !Instance.bIsInUERange));

		if (FadeActive)
		{
			float Percentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
				FMath::Max(Instance.FadeTimeRuntime, 0), 0, FadeTime);
			if (Instance.bIsInUERange)
			{
				Percentage = 1.0f - Percentage;
			}
			Percentage = FTurboSequence_Helper_Lf::Clamp01(Percentage);

			ATurboSequence_Manager_Lf::SetTransitionTsMeshToUEMesh(MeshID, Mesh, Percentage, LastDeltaTime);
		}
	}
}
