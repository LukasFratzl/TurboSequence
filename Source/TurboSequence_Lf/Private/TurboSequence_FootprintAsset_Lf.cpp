// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_FootprintAsset_Lf.h"

#include "TurboSequence_Manager_Lf.h"

UTurboSequence_FootprintAsset_Lf::UTurboSequence_FootprintAsset_Lf()
{
}

void UTurboSequence_FootprintAsset_Lf::TurboSequence_Default_HybridModeUEInstanceAddRemove_Concurrent_Lf(
	int32 MeshID, UTurboSequence_ThreadContext_Lf* ThreadContext)
{
	if (ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
	{
		const FSkinnedMeshRuntime_Lf& Runtime = ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes[MeshID];

		bool bIsInTSMeshDrawRange = !IsValid(Runtime.FootprintAsset) ||
		(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance >= Runtime.FootprintAsset->
			HybridModeMeshDrawRangeUEInstance);

		bool bIsInTSAnimationRange = !IsValid(Runtime.FootprintAsset) ||
		(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance >= Runtime.FootprintAsset->
			HybridModeAnimationDrawRangeUEInstance);


		if (!DefaultAnimationTransitions.Contains(MeshID) || (DefaultAnimationTransitions.Contains(MeshID) &&
			DefaultAnimationTransitions[MeshID] <= 0.005f))
		{
			if (bIsInTSAnimationRange && bIsInTSMeshDrawRange)
			{
				// We Remove Hybrid Data
				if (ATurboSequence_Manager_Lf::GlobalLibrary.MeshIDToMinimalData.Contains(Runtime.GetMeshID()))
				{
					const FTurboSequence_MinimalMeshData_Lf& MeshData = ATurboSequence_Manager_Lf::GlobalLibrary.
						MeshIDToMinimalData[Runtime.GetMeshID()];
					const FSkinnedMeshRuntime_Lf& RootMeshRuntime = ATurboSequence_Manager_Lf::GlobalLibrary.
						RuntimeSkinnedMeshes[MeshData.RootMotionMeshID];
					FSkinnedMeshReference_Lf& RootMeshReference = ATurboSequence_Manager_Lf::GlobalLibrary.
						PerReferenceData[RootMeshRuntime.DataAsset];

					if (RootMeshRuntime.bSpawnedHybridActor)
					{
						FSkinnedMeshReference_Lf& Reference = ATurboSequence_Manager_Lf::GlobalLibrary.PerReferenceData[
							Runtime.DataAsset];
						const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.
							LodIndex];
						FTurboSequence_Utility_Lf::UpdateRenderInstanceLod_Concurrent(Reference, Runtime, LodElement,
							LodElement.bIsRenderStateValid);

						ThreadContext->LockThread();
						RootMeshReference.HybridMeshManagementData.FindOrAdd(RootMeshRuntime.GetMeshID(), false);
						ThreadContext->UnlockThread();
					}
				}
			}
			else if (!bIsInTSAnimationRange || !bIsInTSMeshDrawRange)
			{
				//bIsOutsideTSRange = true;

				// We Add Hybrid Data
				if (ATurboSequence_Manager_Lf::GlobalLibrary.MeshIDToMinimalData.Contains(Runtime.GetMeshID()))
				{
					const FTurboSequence_MinimalMeshData_Lf& MeshData = ATurboSequence_Manager_Lf::GlobalLibrary.
						MeshIDToMinimalData[Runtime.GetMeshID()];
					const FSkinnedMeshRuntime_Lf& RootMeshRuntime = ATurboSequence_Manager_Lf::GlobalLibrary.
						RuntimeSkinnedMeshes[MeshData.RootMotionMeshID];
					FSkinnedMeshReference_Lf& RootMeshReference = ATurboSequence_Manager_Lf::GlobalLibrary.
						PerReferenceData[RootMeshRuntime.DataAsset];

					if (!RootMeshRuntime.bSpawnedHybridActor)
					{
						ThreadContext->LockThread();
						RootMeshReference.HybridMeshManagementData.FindOrAdd(RootMeshRuntime.GetMeshID(), true);
						ThreadContext->UnlockThread();
					}
				}
			}
		}
	}
}


AActor* UTurboSequence_FootprintAsset_Lf::TurboSequence_Default_HybridModeUEInstanceSpawn_GameThread_Lf(
	const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld)
{
	if (ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshData.RootMotionMeshID))
	{
		DefaultAnimationTransitions.FindOrAdd(MeshData.RootMotionMeshID, GET0_NUMBER);
		for (int32 MeshID : MeshData.CustomizableMeshIDs)
		{
			DefaultAnimationTransitions.FindOrAdd(MeshID, GET0_NUMBER);
		}

		if (FSkinnedMeshRuntime_Lf& Runtime = ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes[MeshData.
			RootMotionMeshID]; !IsValid(Runtime.HybridMeshInstance) && IsValid(Runtime.FootprintAsset))
		{
			const FTransform& SpawnTransform = Runtime.WorldSpaceTransform;

			Runtime.HybridMeshInstance = InWorld->SpawnActor<AActor>(Runtime.FootprintAsset->UnrealEngineMeshInstance,
			                                                         SpawnTransform);

			return Runtime.HybridMeshInstance;
		}
		else if (IsValid(Runtime.HybridMeshInstance))
		{
			return Runtime.HybridMeshInstance;
		}
	}

	return nullptr;
}

void UTurboSequence_FootprintAsset_Lf::TurboSequence_Default_HybridModeUEInstanceDestroy_GameThread_Lf(
	const FTurboSequence_MinimalMeshData_Lf& MeshData, const bool bSetTSMeshToUEMeshTransform)
{
	if (ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshData.RootMotionMeshID))
	{
		if (DefaultAnimationTransitions.Contains(MeshData.RootMotionMeshID))
		{
			DefaultAnimationTransitions.Remove(MeshData.RootMotionMeshID);
		}
		for (int32 MeshID : MeshData.CustomizableMeshIDs)
		{
			if (DefaultAnimationTransitions.Contains(MeshID))
			{
				DefaultAnimationTransitions.Remove(MeshID);
			}
		}

		if (FSkinnedMeshRuntime_Lf& Runtime = ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes[MeshData.
			RootMotionMeshID]; IsValid(Runtime.HybridMeshInstance))
		{
			if (bSetTSMeshToUEMeshTransform)
			{
				const TObjectPtr<USkinnedMeshComponent> SkinnedMeshComponent = Runtime.HybridMeshInstance->
					FindComponentByClass<USkinnedMeshComponent>();

				FTransform SpawnTransform;
				if (IsValid(SkinnedMeshComponent))
				{
					SpawnTransform = SkinnedMeshComponent->GetComponentTransform();
				}
				else
				{
					SpawnTransform = Runtime.HybridMeshInstance->GetActorTransform();
				}

				ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(MeshData, SpawnTransform);
			}

			Runtime.HybridMeshInstance->Destroy();
			Runtime.HybridMeshInstance = nullptr;
		}
	}
}

void UTurboSequence_FootprintAsset_Lf::TurboSequence_Default_HybridModeUEInstanceTick_GameThread_Lf(
	const FTurboSequence_MinimalMeshData_Lf& MeshData, float DeltaTime)
{
	if (ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshData.RootMotionMeshID))
	{
		if (const FSkinnedMeshRuntime_Lf& Runtime = ATurboSequence_Manager_Lf::GlobalLibrary.RuntimeSkinnedMeshes[
			MeshData.
			RootMotionMeshID]; IsValid(Runtime.HybridMeshInstance))
		{
			TArray<UActorComponent*> SkinnedMeshComponents;
			Runtime.HybridMeshInstance->GetComponents(USkinnedMeshComponent::StaticClass(), SkinnedMeshComponents);

			if (!SkinnedMeshComponents.Num())
			{
				return;
			}

			TObjectPtr<USkinnedMeshComponent> RootComponent = Cast<USkinnedMeshComponent>(
				SkinnedMeshComponents[GET0_NUMBER]);
			for (UActorComponent* SkinnedMeshComponent : SkinnedMeshComponents)
			{
				if (const TObjectPtr<USkinnedMeshComponent> Component = Cast<
						USkinnedMeshComponent>(SkinnedMeshComponent); Component->GetSkinnedAsset() == Runtime.DataAsset
					->
					ReferenceMeshNative)
				{
					RootComponent = Component;
					break;
				}
			}


			bool bIsInUEMeshDrawRange = !IsValid(Runtime.FootprintAsset) ||
			(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance < Runtime.FootprintAsset->
				HybridModeMeshDrawRangeUEInstance);

			bool bIsInUEAnimationRange = !IsValid(Runtime.FootprintAsset) || (IsValid(Runtime.FootprintAsset) && Runtime
				.ClosestCameraDistance < Runtime.FootprintAsset->HybridModeAnimationDrawRangeUEInstance);

			FTransform SpawnTransform;
			if (IsValid(RootComponent))
			{
				SpawnTransform = RootComponent->GetComponentTransform();
			}
			else
			{
				SpawnTransform = Runtime.HybridMeshInstance->GetActorTransform();
			}

			constexpr float MaxTransitionTime = 5;

			float UETransitionPercentage_RootBone = bIsInUEAnimationRange;
			if (DefaultAnimationTransitions.Contains(MeshData.RootMotionMeshID))
			{
				UETransitionPercentage_RootBone = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
					DefaultAnimationTransitions[MeshData.RootMotionMeshID], GET0_NUMBER, MaxTransitionTime);
			}

			const FTransform& CurrentTransform =
				ATurboSequence_Manager_Lf::GetMeshWorldSpaceTransform_Concurrent(MeshData);

			SpawnTransform.SetScale3D(FMath::Lerp(CurrentTransform.GetScale3D(), SpawnTransform.GetScale3D(),
			                                      UETransitionPercentage_RootBone));
			SpawnTransform.SetLocation(FMath::Lerp(CurrentTransform.GetLocation(), SpawnTransform.GetLocation(),
			                                       UETransitionPercentage_RootBone));
			SpawnTransform.SetRotation(FQuat::Slerp(CurrentTransform.GetRotation(), SpawnTransform.GetRotation(),
			                                        UETransitionPercentage_RootBone));

			ATurboSequence_Manager_Lf::SetMeshWorldSpaceTransform_Concurrent(MeshData, SpawnTransform);

			//Runtime.HybridMeshInstance->SetActorHiddenInGame(!bIsInUEMeshDrawRange);

			auto RunnerFunction = [&](const TObjectPtr<USkinnedMeshComponent> SkinnedMeshComponent, const uint32 MeshID)
			{
				// if (const TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent = Cast<USkeletalMeshComponent>(SkinnedMeshComponent); IsValid(SkeletalMeshComponent))
				// {
				// 	SkeletalMeshComponent->SetVisibleFlag()
				// }
				if (IsValid(SkinnedMeshComponent))
				{
					SkinnedMeshComponent->SetVisibility(bIsInUEMeshDrawRange);
				}

				if (!bIsInUEMeshDrawRange)
				{
					float UETransitionPercentage = bIsInUEAnimationRange;
					if (DefaultAnimationTransitions.Contains(MeshID))
					{
						if (bIsInUEAnimationRange)
						{
							DefaultAnimationTransitions[MeshID] += DeltaTime;
						}
						else
						{
							DefaultAnimationTransitions[MeshID] -= DeltaTime;
						}

						DefaultAnimationTransitions[MeshID] = FMath::Clamp(
							DefaultAnimationTransitions[MeshID], 0.0f, MaxTransitionTime);

						UETransitionPercentage = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(
							DefaultAnimationTransitions[MeshID], GET0_NUMBER, MaxTransitionTime);
					}

					if ((bIsInUEAnimationRange || UETransitionPercentage > 0.005f) && IsValid(SkinnedMeshComponent) &&
						IsValid(
							SkinnedMeshComponent->GetSkinnedAsset()))
					{
						const FReferenceSkeleton& ReferenceSkeleton =
							FTurboSequence_Utility_Lf::GetReferenceSkeleton_Raw(
								SkinnedMeshComponent->GetSkinnedAsset());

						int32 NumBones = ReferenceSkeleton.GetNum();
						for (int32 BoneIdx = GET0_NUMBER; BoneIdx < NumBones; ++BoneIdx)
						{
							const FName& BoneName = SkinnedMeshComponent->GetBoneName(BoneIdx);

							FTransform TurboSequenceIKTransform;
							ATurboSequence_Manager_Lf::GetIKTransform_RawID_Concurrent(
								TurboSequenceIKTransform, MeshID, BoneName, DeltaTime);

							const FTransform& SkeletalMeshIKTransform = SkinnedMeshComponent->GetBoneTransform(BoneIdx);

							TurboSequenceIKTransform.SetScale3D(FMath::Lerp(
								TurboSequenceIKTransform.GetScale3D(), SkeletalMeshIKTransform.GetScale3D(),
								UETransitionPercentage));
							TurboSequenceIKTransform.SetLocation(FMath::Lerp(
								TurboSequenceIKTransform.GetLocation(), SkeletalMeshIKTransform.GetLocation(),
								UETransitionPercentage));
							TurboSequenceIKTransform.SetRotation(FQuat::Slerp(
								TurboSequenceIKTransform.GetRotation(), SkeletalMeshIKTransform.GetRotation(),
								UETransitionPercentage));

							ATurboSequence_Manager_Lf::SetIKTransform_RawID_Concurrent(
								MeshID, BoneName,
								TurboSequenceIKTransform);
						}
					}
				}
			};

			RunnerFunction(RootComponent, MeshData.RootMotionMeshID);

			for (int32 MeshID : MeshData.CustomizableMeshIDs)
			{
				const FSkinnedMeshRuntime_Lf& RuntimeOther = ATurboSequence_Manager_Lf::GlobalLibrary.
					RuntimeSkinnedMeshes[
						MeshID];

				for (UActorComponent* SMComponent : SkinnedMeshComponents)
				{
					if (const TObjectPtr<USkinnedMeshComponent> Component = Cast<USkinnedMeshComponent>(SMComponent)
						;
						Component->GetSkinnedAsset() == RuntimeOther.DataAsset->ReferenceMeshNative)
					{
						RunnerFunction(Component, MeshID);
						break;
					}
				}
			}
		}
	}
}
