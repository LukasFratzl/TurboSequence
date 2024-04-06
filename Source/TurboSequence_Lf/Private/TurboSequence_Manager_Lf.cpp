// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_Manager_Lf.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "TurboSequence_Lf.h"
#include "AssetRegistry/AssetRegistryModule.h"
//#include "Kismet/KismetRenderingLibrary.h"


DECLARE_STATS_GROUP(TEXT("TurboSequenceManager_Lf"), STATGROUP_TurboSequenceManager_Lf, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Solve_TurboSequenceMeshes_Lf"), STAT_Solve_TurboSequenceMeshes_Lf,
                   STATGROUP_TurboSequenceManager_Lf);
DECLARE_CYCLE_STAT(TEXT("Add_TurboSequenceMeshInstances_Lf"), Add_TurboSequenceMeshInstances_Lf,
                   STATGROUP_TurboSequenceManager_Lf);


// Sets default values
ATurboSequence_Manager_Lf::ATurboSequence_Manager_Lf()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	// Setup root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);
}

// Called when the game starts or when spawned
void ATurboSequence_Manager_Lf::BeginPlay()
{
	Super::BeginPlay();

	//Instance = this;
}

void ATurboSequence_Manager_Lf::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	CleanManager_GameThread(true);
}

// Called every frame
void ATurboSequence_Manager_Lf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	for (TTuple<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_Lf>& ReferenceData : GlobalLibrary.
	     PerReferenceData)
	{
		FSkinnedMeshReference_Lf& Reference = ReferenceData.Value;

		if (!IsValid(Reference.DataAsset))
		{
			continue;
		}

		const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Reference.DataAsset;

		for (TTuple<uint32, FRenderData_Lf>& RenderData : Reference.RenderData)
		{
			const TObjectPtr<UNiagaraComponent> NiagaraComponent = Instance->NiagaraComponents[Asset].
				NiagaraRenderer[RenderData.Key].NiagaraRenderer;

			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(
				NiagaraComponent, RenderData.Value.GetParticleRemoveName(), RenderData.Value.ParticlesToRemove);
			RenderData.Value.ParticlesToRemove.Empty();
			if (RenderData.Value.bCollectionDirty)
			{
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayUInt8(
					NiagaraComponent, RenderData.Value.GetLodName(), RenderData.Value.ParticleLevelOfDetails);
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayFloat(
					NiagaraComponent, RenderData.Value.GetCustomDataName(), RenderData.Value.ParticleCustomData);

				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(
					NiagaraComponent, RenderData.Value.GetPositionName(), RenderData.Value.ParticlePositions);
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector4(
					NiagaraComponent, RenderData.Value.GetRotationName(), RenderData.Value.ParticleRotations);
				UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
					NiagaraComponent, RenderData.Value.GetScaleName(), RenderData.Value.ParticleScales);
			}

			const FBox RendererBounds = FBox(RenderData.Value.MinBounds, RenderData.Value.MaxBounds);
			NiagaraComponent->SetEmitterFixedBounds(RenderData.Value.GetEmitterName(), RendererBounds);
			// The solver is Assigning the new bounds every iteration, we only need to reset it here
			// but some small objects having visibility bugs when viewed close if the bounds is too small,
			// so we add a default camera bounds extend to it for the next frame
			FTurboSequence_Utility_Lf::UpdateCameraRendererBounds(RenderData.Value, GlobalLibrary.CameraViews,
			                                                      GET100_NUMBER * GET10_NUMBER);


			RenderData.Value.bCollectionDirty = false;
		}
	}


	if (GlobalLibrary.NumGroupsUpdatedThisFrame && IsValid(GlobalData) && IsValid(GlobalData->AnimationLibraryTexture))
	{
		GlobalLibrary_RenderThread.AnimationLibraryParams.ShaderID = GetTypeHash(GlobalData);
		GlobalLibrary_RenderThread.AnimationLibraryParams.bIsAdditiveWrite = true;
		GlobalLibrary_RenderThread.AnimationLibraryParams.bUse32BitTexture = false;

		FSettingsCompute_Shader_Execute_Lf::Dispatch(GlobalLibrary_RenderThread.AnimationLibraryParams, GlobalData->AnimationLibraryTexture);
			
		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread = GlobalLibrary_RenderThread;
		ENQUEUE_RENDER_COMMAND(TurboSequence_ClearAnimLibraryInput_Lf)(
		[&Library_RenderThread](FRHICommandListImmediate& RHICmdList)
		{
			Library_RenderThread.AnimationLibraryParams.SettingsInput.Reset();
			Library_RenderThread.AnimationLibraryParams.AdditiveWriteBaseIndex = Library_RenderThread.AnimationLibraryMaxNum;
		});
		
		GlobalLibrary_RenderThread.BoneTransformParams.AnimationLibraryTexture = GlobalData->AnimationLibraryTexture;
		
		FMeshUnit_Compute_Shader_Execute_Lf::Dispatch(
			[&](FRHICommandListImmediate& RHICmdList)
			{
				SolveMeshes_RenderThread(RHICmdList);
			},
			GlobalLibrary_RenderThread.BoneTransformParams,
			Instance->GlobalData->TransformTexture, /*Instance->GlobalData->CustomDataTexture,*/
			[&](TArray<float>& DebugValue)
			{
				if (DebugValue.Num() && FMath::IsNearlyEqual(DebugValue[GET0_NUMBER], 777.0f, 0.1f))
				{
					UE_LOG(LogTurboSequence_Lf, Warning, TEXT(" ----- > Start Stack < -----"))
					for (const float& Value : DebugValue)
					{
						UE_LOG(LogTurboSequence_Lf, Warning, TEXT("%f"), Value);
					}
					UE_LOG(LogTurboSequence_Lf, Warning, TEXT(" ----- > End Stack < -----"))
				}
			});
	}
	
	GlobalLibrary.NumGroupsUpdatedThisFrame = GET0_NUMBER;

	if (GlobalLibrary.RuntimeSkinnedMeshes.Num())
	{
		// Otherwise the Critical Section grow in size until we run out of memory when we don't refresh it...
		Instance->RefreshThreadContext_GameThread();
	}
}

FTurboSequence_MinimalMeshData_Lf ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
	const FTurboSequence_MeshSpawnData_Lf FromSpawnData, const FTransform SpawnTransform,
	UWorld* InWorld)
{
	if (!IsValid(FromSpawnData.RootMotionMesh.Mesh))
	{
		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT("Can't create Mesh Instance, the FTurboSequence_MeshSpawnData_Lf has no Mesh assigned...."));
		return FTurboSequence_MinimalMeshData_Lf(false);
	}

	if (!IsValid(InWorld))
	{
		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Can't create Mesh Instance, the World is not a valid world...."));
		return FTurboSequence_MinimalMeshData_Lf(false);
	}

	const uint32 RootMotionMeshID = AddSkinnedMeshInstance_GameThread(
		FromSpawnData.RootMotionMesh.Mesh, SpawnTransform, InWorld, FromSpawnData.RootMotionMesh.OverrideMaterials,
		FromSpawnData.RootMotionMesh.FootprintAsset);

	if (!RootMotionMeshID)
	{
		return FTurboSequence_MinimalMeshData_Lf(false);
	}


	FTurboSequence_MinimalMeshData_Lf Data = FTurboSequence_MinimalMeshData_Lf(true);
	Data.RootMotionMeshID = RootMotionMeshID;

	for (const FMeshMetaData_Lf& MeshData : FromSpawnData.CustomizableMeshes)
	{
		if (const uint32 MeshID = AddSkinnedMeshInstance_GameThread(MeshData.Mesh, SpawnTransform, InWorld,
		                                                            MeshData.OverrideMaterials,
		                                                            MeshData.FootprintAsset))
		{
			Data.CustomizableMeshIDs.Add(MeshID);
		}
	}
	GlobalLibrary.MeshIDToMinimalData.Add(RootMotionMeshID, Data);
	for (const int64& MeshID : Data.CustomizableMeshIDs)
	{
		GlobalLibrary.MeshIDToMinimalData.Add(MeshID, Data);
	}

	return Data;
}

bool ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld)
{
	bool bSuccess = false;

	if (RemoveSkinnedMeshInstance_GameThread(MeshData.RootMotionMeshID, InWorld))
	{
		bSuccess = true;

		if (GlobalLibrary.MeshIDToMinimalData.Contains(MeshData.RootMotionMeshID))
		{
			GlobalLibrary.MeshIDToMinimalData.Remove(MeshData.RootMotionMeshID);
		}
	}

	for (const int64& MeshID : MeshData.CustomizableMeshIDs)
	{
		if (RemoveSkinnedMeshInstance_GameThread(MeshID, InWorld))
		{
			bSuccess = true;

			if (GlobalLibrary.MeshIDToMinimalData.Contains(MeshID))
			{
				GlobalLibrary.MeshIDToMinimalData.Remove(MeshID);
			}
		}
	}

	return bSuccess;
}

uint32 ATurboSequence_Manager_Lf::AddSkinnedMeshInstance_GameThread(
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset, const FTransform& SpawnTransform,
	const TObjectPtr<UWorld> InWorld,
	const TArray<TObjectPtr<UMaterialInterface>>& OverrideMaterials,
	const TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset)
{
	SCOPE_CYCLE_COUNTER(Add_TurboSequenceMeshInstances_Lf);
	{
		if (!IsValid(FromAsset->GlobalData))
		{
			const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
				"AssetRegistry");

			TArray<FAssetData> GlobalAssetData;
			AssetRegistry.Get().GetAssetsByClass(
				FTopLevelAssetPath(UTurboSequence_GlobalData_Lf::StaticClass()->GetPathName()), GlobalAssetData);
			if (GlobalAssetData.Num())
			{
				FTurboSequence_Helper_Lf::SortAssetsByPathName(GlobalAssetData);

				FromAsset->GlobalData = Cast<UTurboSequence_GlobalData_Lf>(GlobalAssetData[GET0_NUMBER].GetAsset());
			}

			if (!IsValid(FromAsset->GlobalData))
			{
				UE_LOG(LogTurboSequence_Lf, Warning,
				       TEXT("Can't create Mesh Instance, there is no global data available...."));
				UE_LOG(LogTurboSequence_Lf, Error,
				       TEXT(
					       "Can not find the Global Data asset -> This is really bad, without it Turbo Sequence does not work, you can recover it by creating an UTurboSequence_GlobalData_Lf Data Asset, Right click in the content browser anywhere in the Project, select Data Asset and choose UTurboSequence_GlobalData_Lf, save it and restart the editor"
				       ));
				return GET0_NUMBER;
			}
		}

		if (!IsValid(FromAsset->GlobalData->TransformTexture))
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "Can't create Mesh Instance, The Transform Texture is missing in the global settings... that's pretty bad"
			       ));
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "Can not find Transform Texture, it should at .../Plugins/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths, if it's not there please create a default Render Target 2D Array Texture and assign the reference in the TurboSequence Lf -> Reference Paths Project settings and open ../Plugins/TurboSequence_Lf/Resources/MF_TurboSequence_PositionOffset_Lf and assign it into the Texture Object with the Transform Texture Comment"
			       ));
			return GET0_NUMBER;
		}

		if (!IsValid(InWorld))
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Can't create Mesh Instance, the World is not valid..."));
			return GET0_NUMBER;
		}

		if (!IsValid(Instance))
		{
			Instance = Cast<ATurboSequence_Manager_Lf>(
				UGameplayStatics::GetActorOfClass(InWorld, StaticClass()));

			if (!IsValid(Instance)) // Is not in the map, create it
			{
				Instance = Cast<ATurboSequence_Manager_Lf>(
					InWorld->SpawnActor(StaticClass()));

				if (!IsValid(Instance))
				{
					UE_LOG(LogTurboSequence_Lf, Warning,
					       TEXT(
						       "Can't create Mesh Instance because Instance is not valid, make sure to have a ATurboSequence_Manager_Lf in the map"
					       ));
					return GET0_NUMBER;
				}
			}
		}

		if (IsValid(Instance) && !IsValid(Instance->GetRootComponent()))
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "Can't create Mesh Instance, make sure to have ATurboSequence_Manager_Lf as a blueprint in the map"
			       ));
			return GET0_NUMBER;
		}

		Instance->GlobalData = FromAsset->GlobalData;

		if (!FromAsset->IsMeshAssetValid())
		{
			return GET0_NUMBER;
		}

		FCriticalSection CriticalSection;
		if (!GlobalLibrary.PerReferenceData.Contains(FromAsset))
		{
			GlobalLibrary_RenderThread.SkinWeightParams.ShaderID = GetTypeHash(Instance);
			GlobalLibrary_RenderThread.BoneTransformParams.ShaderID = GetTypeHash(Instance->GlobalData);

			FTurboSequence_Utility_Lf::CreateAsyncChunkedMeshData(FromAsset, Instance->GlobalData, GlobalLibrary,
			                                                      GlobalLibrary_RenderThread, CriticalSection);
		}

		FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[FromAsset];

		TArray<TObjectPtr<UMaterialInterface>> Materials;
		Materials.SetNum(OverrideMaterials.Num());
		const uint8& NumOverrideMaterials = OverrideMaterials.Num();
		for (uint8 MaterialIdx = GET0_NUMBER; MaterialIdx < NumOverrideMaterials; ++MaterialIdx)
		{
			Materials[MaterialIdx] = OverrideMaterials[MaterialIdx];
		}
		if (!Materials.Num() && IsValid(Reference.FirstValidMeshLevelOfDetail))
		{
			const TArray<FStaticMaterial>& MeshMaterials = Reference.FirstValidMeshLevelOfDetail->GetStaticMaterials();
			const uint8& NumMeshMaterialsMaterials = MeshMaterials.Num();
			Materials.SetNum(NumMeshMaterialsMaterials);
			for (uint8 MaterialIdx = GET0_NUMBER; MaterialIdx < NumMeshMaterialsMaterials; ++MaterialIdx)
			{
				Materials[MaterialIdx] = MeshMaterials[MaterialIdx].MaterialInterface;
			}
		}

		const uint32& MaterialsHash = FTurboSequence_Helper_Lf::GetArrayHash(Materials);
		if (!Instance->NiagaraComponents.Contains(FromAsset) || (Instance->NiagaraComponents.Contains(FromAsset) && !
			Instance->NiagaraComponents[FromAsset].NiagaraRenderer.Contains(MaterialsHash)))
		{
			FTurboSequence_Utility_Lf::CreateRenderer(Reference, FromAsset->GlobalData, FromAsset->RendererSystem,
			                                          Reference.LevelOfDetails, Instance->GetRootComponent(),
			                                          Instance->NiagaraComponents, FromAsset, Materials, MaterialsHash);
		}

		// Now it's time to add the actual instance
		FSkinnedMeshRuntime_Lf Runtime = FSkinnedMeshRuntime_Lf(GlobalLibrary.RuntimeSkinnedMeshes, FromAsset);
		FSkinnedMeshRuntime_RenderThread_Lf Runtime_RenderThread = FSkinnedMeshRuntime_RenderThread_Lf(
			Runtime.GetMeshID(), FromAsset);
		Runtime_RenderThread.bIsVisible = true;
		Runtime.WorldSpaceTransform = SpawnTransform;
		Runtime.MaterialsHash = MaterialsHash;
		Runtime.bForceVisibilityUpdatingThisFrame = true;
		if (IsValid(FootprintAsset) && !FootprintAsset->bExcludeFromSystem)
		{
			Runtime.FootprintAsset = FootprintAsset;
		}

		// We need to define the start LOD so in this case we make a basic LOD calculation which requires
		// the Camera transforms to be Updated, when the simulation is running the SolveMeshes_GameThread
		// function is handling the CameraViews
		if (!GlobalLibrary.CameraViews.Num() || (GlobalLibrary.CameraViews.Num() && GlobalLibrary.CameraViews[
			GET0_NUMBER].CameraTransform.Equals(
			FTransform::Identity)))
		{
			FTurboSequence_Utility_Lf::UpdateCameras(GlobalLibrary.CameraViews, InWorld);
			uint8 CameraIndex = GET0_NUMBER;
			for (FCameraView_Lf& View : GlobalLibrary.CameraViews)
			{
				FTurboSequence_Helper_Lf::GetCameraFrustumPlanes_ObjectSpace(
					View.Planes_Internal, View.Fov, View.ViewportSize, View.AspectRatioAxisConstraint, View.NearClipPlane, View.FarClipPlane,
					!View.bIsPerspective, View.OrthoWidth);

				View.InterpolatedCameraTransform_Internal = View.CameraTransform;

				FTransform& LastTransform = LastFrameCameraTransforms.FindOrAdd(CameraIndex);
				LastTransform = View.CameraTransform;
				CameraIndex++;
			}
		}
		const FTransform& InstanceTransform =
			FTurboSequence_Utility_Lf::GetWorldSpaceTransformIncludingOffsets(Runtime);

		FTurboSequence_Utility_Lf::AddRenderInstance(Reference, Runtime, CriticalSection, InstanceTransform);


		Runtime.LodIndex = INDEX_NONE;
		Runtime.bIsVisible = true;


		FTurboSequence_Utility_Lf::UpdateCullingAndLevelOfDetail(Runtime, Reference,
		                                                         GlobalLibrary.CameraViews,
		                                                         Instance->GetThreadContext(), false, GlobalLibrary);

		CriticalSection.Lock();
		GlobalLibrary.MeshIDToGlobalIndex.Add(Runtime.GetMeshID(), GlobalLibrary.RuntimeSkinnedMeshes.Num());
		GlobalLibrary.RuntimeSkinnedMeshesHashMap.Add(Runtime.GetMeshID());
		GlobalLibrary.RuntimeSkinnedMeshes.Add(Runtime.GetMeshID(), Runtime);
		CriticalSection.Unlock();

		FTurboSequence_AnimPlaySettings_Lf PlaySetting = FTurboSequence_AnimPlaySettings_Lf();
		PlaySetting.ForceMode = ETurboSequence_AnimationForceMode_Lf::AllLayers;
		PlaySetting.RootMotionMode = ETurboSequence_RootMotionMode_Lf::None;

		PlayAnimation_RawID_Concurrent(Runtime.GetMeshID(), FromAsset->OverrideDefaultAnimation, PlaySetting);

		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread = GlobalLibrary_RenderThread;
		ENQUEUE_RENDER_COMMAND(TurboSequence_AddRenderInstance_Lf)(
			[&Library_RenderThread, Runtime_RenderThread](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.MeshIDToGlobalIndex.Add(Runtime_RenderThread.GetMeshID(),
				                                             Library_RenderThread.RuntimeSkinnedMeshes.Num());

				Library_RenderThread.RuntimeSkinnedMeshes.Add(Runtime_RenderThread.GetMeshID(), Runtime_RenderThread);
				Library_RenderThread.RuntimeSkinnedMeshesHashMap.Add(Runtime_RenderThread.GetMeshID());
			});

		return Runtime.GetMeshID();
	}
}

bool ATurboSequence_Manager_Lf::RemoveSkinnedMeshInstance_GameThread(const int64& MeshID,
                                                                     const TObjectPtr<UWorld> InWorld)
{
	if (!IsValid(InWorld))
	{
		return false;
	}

	if (!IsValid(Instance))
	{
		return false;
	}

	if (!GlobalLibrary.RuntimeSkinnedMeshes.Contains(MeshID))
	{
		return false;
	}

	FCriticalSection CriticalSection;
	FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread = GlobalLibrary_RenderThread;

	FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[MeshID];
	FTurboSequence_Utility_Lf::ClearAnimations(CriticalSection, Runtime, GlobalLibrary, Library_RenderThread,
	                                           ETurboSequence_AnimationForceMode_Lf::AllLayers,
	                                           TArray<FTurboSequence_BoneLayer_Lf>(),
	                                           [](const FAnimationMetaData_Lf& Animation)
	                                           {
		                                           return true;
	                                           });
	// Since we have the first animation as rest pose
	FTurboSequence_Utility_Lf::RemoveAnimation(Runtime, CriticalSection, GlobalLibrary, Library_RenderThread,
	                                           GET0_NUMBER);
	FTurboSequence_Utility_Lf::UpdateAnimationLayerMasks(CriticalSection, GlobalLibrary, Library_RenderThread);

	FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
	FTurboSequence_Utility_Lf::RemoveRenderInstance(Reference, Runtime, CriticalSection);

	if (const FRenderData_Lf& RenderData = Reference.RenderData[Runtime.MaterialsHash]; !RenderData.InstanceMap.Num())
	{
		FTurboSequence_Utility_Lf::CleanNiagaraRenderer(Instance->NiagaraComponents, Reference, Runtime);
	}
	GlobalLibrary.RuntimeSkinnedMeshes.Remove(Runtime.GetMeshID());
	GlobalLibrary.RuntimeSkinnedMeshesHashMap.Remove(Runtime.GetMeshID());
	GlobalLibrary.MeshIDToMinimalData.Remove(Runtime.GetMeshID());

	const int32 GlobalIndex = GlobalLibrary.MeshIDToGlobalIndex[Runtime.GetMeshID()];
	GlobalLibrary.MeshIDToGlobalIndex.Remove(Runtime.GetMeshID());
	for (TTuple<uint32, int32>& Index : GlobalLibrary.MeshIDToGlobalIndex)
	{
		if (Index.Value > GlobalIndex)
		{
			Index.Value--;
		}
	}

	const uint32 MeshID_RenderThread = Runtime.GetMeshID();
	ENQUEUE_RENDER_COMMAND(TurboSequence_RemoveRenderInstance_Lf)(
		[&Library_RenderThread, MeshID_RenderThread](FRHICommandListImmediate& RHICmdList)
		{
			const int32 GlobalIndex = Library_RenderThread.MeshIDToGlobalIndex[MeshID_RenderThread];
			Library_RenderThread.MeshIDToGlobalIndex.Remove(MeshID_RenderThread);
			for (TTuple<uint32, int32>& Index : Library_RenderThread.MeshIDToGlobalIndex)
			{
				if (Index.Value > GlobalIndex)
				{
					Index.Value--;
				}
			}

			Library_RenderThread.RuntimeSkinnedMeshes.Remove(MeshID_RenderThread);
			Library_RenderThread.RuntimeSkinnedMeshesHashMap.Remove(MeshID_RenderThread);
		});


	// Time to remove the reference, we assume here the instance has a mesh,
	if (!Reference.RenderData.Num())
	{
		GlobalLibrary.PerReferenceData.Remove(Runtime.DataAsset);
		GlobalLibrary.PerReferenceDataKeys.Remove(Runtime.DataAsset);

		const TObjectPtr<UTurboSequence_MeshAsset_Lf> DataAsset_RenderThread = Runtime.DataAsset;
		ENQUEUE_RENDER_COMMAND(TurboSequence_RemoveRenderThreadReference_Lf)(
			[&Library_RenderThread, DataAsset_RenderThread](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.PerReferenceDataKeys.Remove(DataAsset_RenderThread);
				Library_RenderThread.PerReferenceData.Remove(DataAsset_RenderThread);

				const int16 NumReferences = Library_RenderThread.PerReferenceData.Num();
				for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
				{
					FSkinnedMeshReference_RenderThread_Lf& ReferenceOther = Library_RenderThread.PerReferenceData[
						Library_RenderThread.PerReferenceDataKeys[i]];

					ReferenceOther.ReferenceCollectionIndex = i;
				}
			});

		const int16 NumReferences = GlobalLibrary.PerReferenceData.Num();
		for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
		{
			FSkinnedMeshReference_Lf& ReferenceOther = GlobalLibrary.PerReferenceData[GlobalLibrary.PerReferenceDataKeys
				[i]];

			ReferenceOther.ReferenceCollectionIndex = i;
		}

		FTurboSequence_Utility_Lf::RefreshAsyncChunkedMeshData(Instance->GlobalData, GlobalLibrary,
		                                                       GlobalLibrary_RenderThread, CriticalSection);
	}

	return true;
}


/**
 * @brief Solves all meshes at once, call it only in the game thread
 * @param DeltaTime The DeltaTime in seconds
 * @param InWorld The world, we need it for the cameras and culling
 * @param UpdateContext The Update Context for useful stuff like the Group Index
 */
void ATurboSequence_Manager_Lf::SolveMeshes_GameThread(const float& DeltaTime, UWorld* InWorld, FTurboSequence_UpdateContext_Lf UpdateContext)
{
	SCOPE_CYCLE_COUNTER(STAT_Solve_TurboSequenceMeshes_Lf);
	{
		// When we are actually have any data, otherwise there is nothing to compute and we can return
		if (!GlobalLibrary.RuntimeSkinnedMeshes.Num())
		{
			bMeshesSolvingAtTheMoment = false;
			return;
		}

		bMeshesSolvingAtTheMoment = true;

		if (!IsValid(Instance))
		{
			return;
		}

		FTurboSequence_Utility_Lf::UpdateCameras_1(GlobalLibrary.CameraViews, LastFrameCameraTransforms, InWorld,
		                                           DeltaTime);

		if (!GlobalLibrary.CameraViews.Num())
		{
			return;
		}

		if (!GlobalLibrary.UpdateGroups.IsValidIndex(UpdateContext.GroupIndex))
		{
			return;
		}

		const int64& CurrentFrameCount = UKismetSystemLibrary::GetFrameCount();

		const TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext = Instance->GetThreadContext();

		FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread = GlobalLibrary_RenderThread;

		bool bMeshIsVisible = false;
		//TArray<FInstanceQueue_Lf> QueuedMeshes;
		const int32 NumMeshes = GlobalLibrary.UpdateGroups[UpdateContext.GroupIndex].RawIDs.Num();
		const int16& NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;
		const int32& NumMeshesPerThread = FMath::CeilToInt(
			static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));
		ParallelFor(NumThreads, [&](const int32& ThreadsIndex)
		{
			const int32 MeshBaseIndex = ThreadsIndex * NumMeshesPerThread;
			const int32 MeshBaseNum = MeshBaseIndex + NumMeshesPerThread;

			for (int32 Index = MeshBaseIndex; Index < MeshBaseNum; ++Index)
			{
				if (Index >= NumMeshes)
				{
					break;
				}

				//CriticalSection.Lock();
				FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[GlobalLibrary.UpdateGroups[
					UpdateContext.GroupIndex].RawIDs[Index]];

				FSkinnedMeshReference_Lf& Reference = GlobalLibrary.PerReferenceData[Runtime.DataAsset];
				//CriticalSection.Unlock();
				if (!IsValid(Reference.FirstValidMeshLevelOfDetail))
				{
					return;
				}

				if (!IsValid(Instance))
				{
					return;
				}

				// Need to get always updated
				FTurboSequence_Utility_Lf::SolveAnimations(Runtime,
				                                           GlobalLibrary,
				                                           GlobalLibrary_RenderThread,
				                                           Reference,
				                                           DeltaTime,
				                                           CurrentFrameCount,
				                                           ThreadContext->CriticalSection);

				// Update Visibility first because the LodElement.InstanceMap is maybe not correct if the LOD change
				// and for the initial check we only need the LOD index
				const bool bIsVisiblePrevious = Runtime.bIsVisible;
				FTurboSequence_Utility_Lf::IsMeshVisible(Runtime, Reference, GlobalLibrary.CameraViews);

				FTurboSequence_Utility_Lf::UpdateCullingAndLevelOfDetail(
					Runtime, Reference, GlobalLibrary.CameraViews, ThreadContext, bIsVisiblePrevious, GlobalLibrary);

				FTurboSequence_Utility_Lf::UpdateDistanceUpdating(Runtime, DeltaTime);

				FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[Runtime.LodIndex];
				if (LodElement.bIsRenderStateValid)
				{
					FTurboSequence_Utility_Lf::SetCustomDataForInstance(
						Reference, GlobalLibrary.MeshIDToGlobalIndex[Runtime.GetMeshID()], LodElement.SkinWeightOffset,
						Runtime, GlobalLibrary);
				}

				const bool& bIsInTSMeshDrawRange = !IsValid(Runtime.FootprintAsset) ||
					(IsValid(Runtime.FootprintAsset) && Runtime.ClosestCameraDistance >= Runtime.FootprintAsset->HybridModeMeshDrawRangeUEInstance);

				if (!LodElement.bIsFrustumCullingEnabled && bIsInTSMeshDrawRange)
				{
					Runtime.bIsVisible = true;

					FTurboSequence_Utility_Lf::UpdateRenderInstanceLod_Concurrent(
						Reference, Runtime, LodElement, Runtime.bIsVisible && LodElement.bIsRenderStateValid);
				}
				else if (!bIsInTSMeshDrawRange && Runtime.bSpawnedHybridActor)
				{
					Runtime.bIsVisible = false;

					FTurboSequence_Utility_Lf::UpdateRenderInstanceLod_Concurrent(
						Reference, Runtime, LodElement, false);
				}

				if (Runtime.bIsVisible || Runtime.bForceVisibilityUpdatingThisFrame)
				{
					FTurboSequence_Utility_Lf::UpdateRendererBounds(ThreadContext->CriticalSection, Reference, Runtime);

					if ((LodElement.bIsAnimated && Runtime.bIsDistanceUpdatingThisFrame && LodElement.
						bIsRenderStateValid) || (Runtime.bForceVisibilityUpdatingThisFrame && LodElement.
						bIsRenderStateValid))
					{
						Runtime.bForceVisibilityUpdatingThisFrame = false;

						ThreadContext->CriticalSection.Lock();
						bMeshIsVisible = true;
						ThreadContext->CriticalSection.Unlock();

						const uint8 MeshIdx = LodElement.GPUMeshIndex;
						const uint32 MeshID = Runtime.GetMeshID();
						const TArray<FAnimationMetaData_RenderThread_Lf> Animations_RenderThread = Runtime.
							AnimationMetaData_RenderThread;
						TMap<uint16, FIKBoneData_Lf> IK_RenderThread;
						if (Runtime.bIKDataInUse)
						{
							IK_RenderThread = Runtime.IKData;
						}
						const bool bUseIK = Runtime.bIKDataInUse;
						ENQUEUE_RENDER_COMMAND(TurboSequence_UpdateRenderInstance_Lf)(
							[&Library_RenderThread = GlobalLibrary_RenderThread, MeshID, MeshIdx, bUseIK,
								Animations_RenderThread,
								IK_RenderThread](FRHICommandListImmediate& RHICmdList)
							{
								FSkinnedMeshRuntime_RenderThread_Lf& Runtime_RenderThread = Library_RenderThread.
									RuntimeSkinnedMeshes[MeshID];

								Runtime_RenderThread.bIsVisible = true;
								Runtime_RenderThread.CurrentGPUMeshIndex = MeshIdx;
								Runtime_RenderThread.AnimationMetaData_RenderThread = Animations_RenderThread;
								Runtime_RenderThread.bIKDataInUse = bUseIK;
								Runtime_RenderThread.IKData = IK_RenderThread;
							});
					}
				}

				// Better to manage it ourselves for easier use....
				FTurboSequence_Utility_Lf::ClearIKState(Runtime, ThreadContext->CriticalSection);
			}
		}, EParallelForFlags::BackgroundPriority);


		FTurboSequence_Utility_Lf::UpdateAnimationLayerMasks(ThreadContext->CriticalSection, GlobalLibrary, Library_RenderThread);

		for (TTuple<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FSkinnedMeshReference_Lf>& ReferenceData : GlobalLibrary.
		     PerReferenceData)
		{
			FSkinnedMeshReference_Lf& Reference = ReferenceData.Value;

			for (const TTuple<uint32, bool>& ManagementData : Reference.HybridMeshManagementData)
			{
				if (GlobalLibrary.RuntimeSkinnedMeshes.Contains(ManagementData.Key))
				{
					FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[ManagementData.Key];
					const FTurboSequence_MinimalMeshData_Lf& MeshData = GlobalLibrary.MeshIDToMinimalData[Runtime.
						GetMeshID()];
					if (IsValid(Runtime.FootprintAsset))
					{
						if (!Runtime.bSpawnedHybridActor)
						{
							Runtime.bSpawnedHybridActor = true;

							GlobalLibrary.HybridMeshes.FindOrAdd(MeshData, false);

							Runtime.FootprintAsset->OnHybridModeUEInstanceSpawn_GameThread_Override(
								MeshData, InWorld);
						}
						else
						{
							Runtime.bSpawnedHybridActor = false;

							Runtime.FootprintAsset->OnHybridModeUEInstanceDestroy_GameThread_Override(MeshData, InWorld);

							GlobalLibrary.HybridMeshes.Remove(MeshData);
						}
					}

					Reference.RenderData[Runtime.MaterialsHash].bCollectionDirty = true;
				}
			}
			Reference.HybridMeshManagementData.Empty();
		}

		for (TTuple<FTurboSequence_MinimalMeshData_Lf, bool>& HybridMesh : GlobalLibrary.HybridMeshes)
		{
			const FSkinnedMeshRuntime_Lf& Runtime = GlobalLibrary.RuntimeSkinnedMeshes[HybridMesh.Key.RootMotionMeshID];

			if (IsValid(Runtime.FootprintAsset))
			{
				Runtime.FootprintAsset->OnHybridModeUEInstanceTick_GameThread_Override(
					HybridMesh.Key, InWorld, DeltaTime);
			}
		}

		if (bMeshIsVisible && GlobalLibrary.PerReferenceData.Num() && IsValid(Instance->GlobalData) && IsValid(
			Instance->GlobalData->TransformTexture))
		{
			GlobalLibrary.NumGroupsUpdatedThisFrame++;

			GlobalLibrary_RenderThread.BoneTransformParams.NumDebugData = GET10_NUMBER;

			GlobalLibrary_RenderThread.BoneTransformParams.bUse32BitTransformTexture = Instance->GlobalData->bUseHighPrecisionAnimationMode;

			const uint32 AnimationMaxNum = GlobalLibrary.AnimationLibraryMaxNum;

			if (GlobalLibrary.AnimationLibraryDataAllocatedThisFrame.Num())
			{
				TArray<FVector4f> AnimationData = GlobalLibrary.AnimationLibraryDataAllocatedThisFrame;
				GlobalLibrary.AnimationLibraryDataAllocatedThisFrame.Empty();

				ENQUEUE_RENDER_COMMAND(TurboSequence_AddLibraryAnimationChunked_Lf)(
					[&Library_RenderThread, AnimationData, AnimationMaxNum](FRHICommandListImmediate& RHICmdList)
					{
						Library_RenderThread.AnimationLibraryParams.SettingsInput.Append(AnimationData);

						Library_RenderThread.AnimationLibraryMaxNum = AnimationMaxNum;
					});
			}

			FTurboSequence_Utility_Lf::RemoveAnimationFromLibraryChunked(
				GlobalLibrary, Library_RenderThread, ThreadContext->CriticalSection);
		}
		
		FTurboSequence_Utility_Lf::UpdateCameras_2(LastFrameCameraTransforms, GlobalLibrary.CameraViews);

		GlobalLibrary.bMaxNumCPUBonesChanged = false;

		bMeshesSolvingAtTheMoment = false;
	}
}

void ATurboSequence_Manager_Lf::SolveMeshes_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	if (!IsValid(Instance))
	{
		return;
	}

	if (!GlobalLibrary_RenderThread.RuntimeSkinnedMeshes.Num())
	{
		return;
	}


	FCriticalSection CriticalSection;
	const int32 NumMeshes = GlobalLibrary_RenderThread.RuntimeSkinnedMeshesHashMap.Num();
	const int16& NumThreads = FTurboSequence_Helper_Lf::NumCPUThreads() - GET1_NUMBER;
	const int32& NumMeshesPerThread = FMath::CeilToInt(static_cast<float>(NumMeshes) / static_cast<float>(NumThreads));

	FTurboSequence_Utility_Lf::ResizeBuffers(GlobalLibrary_RenderThread, NumMeshes);


	ParallelFor(NumThreads, [&](const int32& ThreadsIndex)
	{
		const int32 MeshBaseIndex = ThreadsIndex * NumMeshesPerThread;
		const int32 MeshBaseNum = MeshBaseIndex + NumMeshesPerThread;

		for (int32 Index = MeshBaseIndex; Index < MeshBaseNum; ++Index)
		{
			if (Index >= NumMeshes)
			{
				break;
			}

			FSkinnedMeshRuntime_RenderThread_Lf& Runtime = GlobalLibrary_RenderThread.RuntimeSkinnedMeshes[
				GlobalLibrary_RenderThread.RuntimeSkinnedMeshesHashMap[Index]];

			const FSkinnedMeshReference_RenderThread_Lf& Reference = GlobalLibrary_RenderThread.PerReferenceData[Runtime
				.
				DataAsset];

			GlobalLibrary_RenderThread.BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread[Index] = INDEX_NONE;

			if (Runtime.bIsVisible)
			{
				CriticalSection.Lock();
				const int32 MeshIndex = GlobalLibrary_RenderThread.NumMeshesVisibleCurrentFrame;
				if (!FTurboSequence_Utility_Lf::IsValidBufferIndex(GlobalLibrary_RenderThread, MeshIndex))
				{
					CriticalSection.Unlock();

					UE_LOG(LogTemp, Warning, TEXT("Not Valid Buffer Index... -> Mesh Index %d | Data Asset -> %s"),
					       MeshIndex,
					       *Runtime.DataAsset.GetName())
					Runtime.bIsVisible = false;
					Runtime.bIKDataInUse = false;
					return;
				}
				GlobalLibrary_RenderThread.NumMeshesVisibleCurrentFrame++;
				CriticalSection.Unlock();

				GlobalLibrary_RenderThread.BoneTransformParams.PerMeshCustomDataIndex_RenderThread[MeshIndex] =
					GlobalLibrary_RenderThread.MeshIDToGlobalIndex[Runtime.GetMeshID()];

				CriticalSection.Lock();
				GlobalLibrary_RenderThread.NumAnimationsCurrentFrame += Runtime.AnimationMetaData_RenderThread.Num();

				
				CriticalSection.Unlock();


				if (Runtime.bIKDataInUse)
				{
					uint32 IKDataIndex = GET0_NUMBER;
					uint16 IKBoneNum = GET0_NUMBER;
					for (const TTuple<uint16, uint16>& BoneIdx : Reference.FirstLodGPUBonesCollection)
					{
						if (Runtime.IKData.Contains(BoneIdx.Key) && Runtime.IKData[BoneIdx.Key].
							bIsInUsingWriteDataThisFrame && BoneIdx.Value != INDEX_NONE)
						{
							IKBoneNum++;
						}

						IKDataIndex += GET3_NUMBER;
					}

					if (IKBoneNum)
					{
						GlobalLibrary_RenderThread.BoneTransformParams.BoneSpaceAnimationIKIndex_RenderThread[Index] =
							IKBoneNum;
					}

					CriticalSection.Lock();
					GlobalLibrary_RenderThread.NumIKPixelCurrentFrame += IKBoneNum;
					CriticalSection.Unlock();
				}
			}

			Runtime.bIsVisible = false;
			Runtime.bIKDataInUse = false;
			// Runtime.IKData.Empty(); // Is now inside the minimal loop down below
		}
	}, EParallelForFlags::BackgroundPriority);

	if (!GlobalLibrary_RenderThread.NumMeshesVisibleCurrentFrame)
	{
		return;
	}

	FMeshUnitComputeShader_Params_Lf& MeshParams = GlobalLibrary_RenderThread.BoneTransformParams;

	MeshParams.NumMeshes = GlobalLibrary_RenderThread.NumMeshesVisibleCurrentFrame;
	GlobalLibrary_RenderThread.NumMeshesVisibleCurrentFrame = GET0_NUMBER;

	MeshParams.NumAnimations = GlobalLibrary_RenderThread.NumAnimationsCurrentFrame;
	GlobalLibrary_RenderThread.NumAnimationsCurrentFrame = GET0_NUMBER;

	MeshParams.NumIKData = GlobalLibrary_RenderThread.NumIKPixelCurrentFrame;
	GlobalLibrary_RenderThread.NumIKPixelCurrentFrame = GET0_NUMBER;


	const int32& NumMeshes_RenderThread = MeshParams.NumMeshes;
	const int32& NumAnimations_RenderThread = FMath::Max(MeshParams.NumAnimations, 1);
	const int32& NumIKData_RenderThread = FMath::Max(MeshParams.NumIKData, 1);
	const int32& NumReferences_RenderThread = MeshParams.ReferenceNumCPUBones.Num();

	MeshParams.ReferenceNumCPUBones_RenderThread.SetNumUninitialized(NumReferences_RenderThread);
	MeshParams.PerMeshCustomDataIndex_Global_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	MeshParams.PerMeshCustomDataLod_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	MeshParams.PerMeshCustomDataCollectionIndex_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	MeshParams.AnimationStartIndex_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	MeshParams.AnimationEndIndex_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	
	MeshParams.AnimationFramePose0_RenderThread.SetNumUninitialized(NumAnimations_RenderThread);
	MeshParams.AnimationWeights_RenderThread.SetNumUninitialized(NumAnimations_RenderThread);
	MeshParams.AnimationLayerIndex_RenderThread.SetNumUninitialized(NumAnimations_RenderThread);

	MeshParams.BoneSpaceAnimationIKData_RenderThread.SetNumUninitialized(NumIKData_RenderThread);
	MeshParams.BoneSpaceAnimationIKInput_RenderThread.SetNumUninitialized(
		NumIKData_RenderThread * FTurboSequence_Helper_Lf::NumGPUBoneIKBuffer);
	MeshParams.BoneSpaceAnimationIKStartIndex_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);
	MeshParams.BoneSpaceAnimationIKEndIndex_RenderThread.SetNumUninitialized(NumMeshes_RenderThread);


	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.ReferenceNumCPUBones_RenderThread, 0);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.PerMeshCustomDataIndex_Global_RenderThread, 1);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.PerMeshCustomDataLod_RenderThread, 2);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.PerMeshCustomDataCollectionIndex_RenderThread, 3);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.SkinWeightOffsetLod_RenderThread, 4);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationStartIndex_RenderThread, 5);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationEndIndex_RenderThread, 6);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationFrameAlpha_RenderThread, 7);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationFramePose0_RenderThread, 8);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationFramePose1_RenderThread, 9);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationWeights_RenderThread, 10);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationLayerIndex_RenderThread, 11);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.BoneSpaceAnimationIKData_RenderThread, 12);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.BoneSpaceAnimationIKInput_RenderThread, 13);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.BoneSpaceAnimationIKStartIndex_RenderThread, 14);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.BoneSpaceAnimationIKEndIndex_RenderThread, 15);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.BoneSpaceAnimationIKIndex_RenderThread, 16);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.PerMeshCustomDataIndex_RenderThread, 17);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.ReferenceNumCPUBones, 18);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationRawData_RenderThread, 19);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationLayers_RenderThread, 20);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.CPUInverseReferencePose, 21);
	FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.Indices, 22);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationLocationWeights_RenderThread, 23);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationRotationWeights_RenderThread, 24);
	//FTurboSequence_Helper_Lf::CheckArrayHasSize(MeshParams.AnimationScaleWeights_RenderThread, 25);

	constexpr uint8 NumOperations = GET4_NUMBER;
	ParallelFor(NumOperations, [&](const int32& ThreadsIndex)
	{
		if (ThreadsIndex == GET0_NUMBER)
		{
			for (int32 i = GET0_NUMBER; i < NumMeshes_RenderThread; ++i)
			{
				const int32& CPUIndex = MeshParams.PerMeshCustomDataIndex_RenderThread[i];
				MeshParams.PerMeshCustomDataIndex_Global_RenderThread[i] = CPUIndex;

				const FSkinnedMeshRuntime_RenderThread_Lf& Runtime = GlobalLibrary_RenderThread.RuntimeSkinnedMeshes[
					GlobalLibrary_RenderThread.RuntimeSkinnedMeshesHashMap[CPUIndex]];
				const FSkinnedMeshReference_RenderThread_Lf& Reference = GlobalLibrary_RenderThread.PerReferenceData[
					Runtime.DataAsset];

				MeshParams.PerMeshCustomDataLod_RenderThread[i] = Runtime.CurrentGPUMeshIndex;
				MeshParams.PerMeshCustomDataCollectionIndex_RenderThread[i] = Reference.ReferenceCollectionIndex;
			}
		}
		else if (ThreadsIndex == GET1_NUMBER)
		{
			if (MeshParams.NumAnimations)
			{
				int32 CurrentAnimationIndex = GET0_NUMBER;
				//int32 CurrentPerBoneWeightIndex = GET0_NUMBER;
				for (int32 i = GET0_NUMBER; i < NumMeshes_RenderThread; ++i)
				{
					const int32& CPUIndex = MeshParams.PerMeshCustomDataIndex_RenderThread[i];
					const int32 LastAnimationIndex = CurrentAnimationIndex;

					MeshParams.AnimationStartIndex_RenderThread[i] = LastAnimationIndex;

					const FSkinnedMeshRuntime_RenderThread_Lf& Runtime = GlobalLibrary_RenderThread.RuntimeSkinnedMeshes
						[GlobalLibrary_RenderThread.RuntimeSkinnedMeshesHashMap[CPUIndex]];
					//const FSkinnedMeshReference_RenderThread_Lf& Reference = GlobalLibrary_RenderThread.PerReferenceData[Runtime.DataAsset];

					const int32& NumAnimations = Runtime.AnimationMetaData_RenderThread.Num();
					//UE_LOG(LogTemp, Warning, TEXT("%d | %d"), Runtime.GetMeshID(), NumAnimations);
					for (int32 AnimIdx = GET0_NUMBER; AnimIdx < NumAnimations; ++AnimIdx)
					{
						const FAnimationMetaData_RenderThread_Lf& Animation = Runtime.AnimationMetaData_RenderThread[
							AnimIdx];

						const int32& BufferIndex = CurrentAnimationIndex;
						
						MeshParams.AnimationFramePose0_RenderThread[BufferIndex] = Animation.GPUAnimationIndex_0;
						MeshParams.AnimationWeights_RenderThread[BufferIndex] = Animation.FinalAnimationWeight;
						MeshParams.AnimationLayerIndex_RenderThread[BufferIndex] = Animation.LayerMaskIndex;

						CurrentAnimationIndex++;
					}

					MeshParams.AnimationEndIndex_RenderThread[i] = NumAnimations;
				}
			}
			else
			{
				for (int32 i = GET0_NUMBER; i < NumMeshes_RenderThread; ++i)
				{
					MeshParams.AnimationStartIndex_RenderThread[i] = GET0_NUMBER;
					MeshParams.AnimationEndIndex_RenderThread[i] = GET0_NUMBER;
				}
			}
		}
		else if (ThreadsIndex == GET2_NUMBER)
		{
			if (MeshParams.NumIKData)
			{
				int32 CurrentIKIndex = GET0_NUMBER;
				int32 CurrentIKDataIndex = GET0_NUMBER;
				for (int32 i = GET0_NUMBER; i < NumMeshes_RenderThread; ++i)
				{
					const int32& CPUIndex = MeshParams.PerMeshCustomDataIndex_RenderThread[i];
					const int32 LastIKDataIndex = CurrentIKDataIndex;

					int16 NumIKBones = GET0_NUMBER;

					// Really important to keep it INDEX_NONE otherwise it goes out of bounds when index
					// smoothing is activated
					if (const int32& IKIndex = MeshParams.BoneSpaceAnimationIKIndex_RenderThread[CPUIndex]; IKIndex >
						INDEX_NONE)
					{
						FSkinnedMeshRuntime_RenderThread_Lf& Runtime = GlobalLibrary_RenderThread.RuntimeSkinnedMeshes[
							GlobalLibrary_RenderThread.RuntimeSkinnedMeshesHashMap[CPUIndex]];
						const FSkinnedMeshReference_RenderThread_Lf& Reference = GlobalLibrary_RenderThread.
							PerReferenceData[Runtime.DataAsset];

						for (const TTuple<uint16, uint16>& BoneIdx : Reference.FirstLodGPUBonesCollection)
						{
							if (Runtime.IKData.Contains(BoneIdx.Key) && Runtime.IKData[BoneIdx.Key].
								bIsInUsingWriteDataThisFrame && BoneIdx.Value != INDEX_NONE)
							{
								MeshParams.BoneSpaceAnimationIKData_RenderThread[CurrentIKDataIndex] = BoneIdx.Value;

								const FMatrix& BoneMatrix = Runtime.IKData[BoneIdx.Key].IKWriteTransform.
								                                                        ToMatrixWithScale();

								for (int32 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
								{
									FVector4f BoneData;
									BoneData.X = BoneMatrix.M[GET0_NUMBER][M];
									BoneData.Y = BoneMatrix.M[GET1_NUMBER][M];
									BoneData.Z = BoneMatrix.M[GET2_NUMBER][M];
									BoneData.W = BoneMatrix.M[GET3_NUMBER][M];

									MeshParams.BoneSpaceAnimationIKInput_RenderThread[CurrentIKIndex] = BoneData;

									CurrentIKIndex++;
								}

								CurrentIKDataIndex++;
								NumIKBones++;

								// Runtime.IKData.Empty(); Replaced now without disposing the memory
								// Cause we will write the new data anyway every IK frame
								Runtime.IKData[BoneIdx.Key].bIsInUsingWriteDataThisFrame = false;
							}
						}
					}

					// If valid data or not, the loop in the compute shader will sort it out
					// it's a ranged loop which mean if the LastIKDataIndex == CurrentIKDataIndex
					// it will never enter, because i < Count
					// otherwise if the data is writable the indices may always change
					MeshParams.BoneSpaceAnimationIKStartIndex_RenderThread[i] = LastIKDataIndex;
					MeshParams.BoneSpaceAnimationIKEndIndex_RenderThread[i] = NumIKBones;
				}
			}
			else
			{
				for (int32 i = GET0_NUMBER; i < NumMeshes_RenderThread; ++i)
				{
					MeshParams.BoneSpaceAnimationIKStartIndex_RenderThread[i] = GET0_NUMBER;
					MeshParams.BoneSpaceAnimationIKEndIndex_RenderThread[i] = GET0_NUMBER;
				}
			}
		}
		else if (ThreadsIndex == GET3_NUMBER)
		{
			for (uint16 i = GET0_NUMBER; i < NumReferences_RenderThread; ++i)
			{
				MeshParams.ReferenceNumCPUBones_RenderThread[i] = MeshParams.ReferenceNumCPUBones[i];
			}
		}
	}, EParallelForFlags::BackgroundPriority);
}

void ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_RawID_Concurrent(const int32 GroupIndex, const int64& MeshID)
{
	{
		FScopeLock Lock(&Instance->GetThreadContext()->CriticalSection);

		if (!GlobalLibrary.UpdateGroups.IsValidIndex(GroupIndex))
		{
			GlobalLibrary.UpdateGroups.SetNum(GroupIndex + GET1_NUMBER);
		}

		FTurboSequence_UpdateGroup_Lf& Group = GlobalLibrary.UpdateGroups[GroupIndex];

		if (!Group.RawIDData.Contains(MeshID))
		{
			Group.RawIDData.Add(MeshID, Group.RawIDData.Num());
			Group.RawIDs.Add(MeshID);
			if (!Group.MeshIDToMinimal.Contains(GlobalLibrary.MeshIDToMinimalData[MeshID]))
			{
				Group.MeshIDToMinimal.Add(GlobalLibrary.MeshIDToMinimalData[MeshID], FIntVector2(GET1_NUMBER, Group.MeshIDToMinimal.Num()));
				Group.RawMinimalData.Add(GlobalLibrary.MeshIDToMinimalData[MeshID]);
			}
			else
			{
				Group.MeshIDToMinimal[GlobalLibrary.MeshIDToMinimalData[MeshID]].X++;
			}
		}
	}
}

void ATurboSequence_Manager_Lf::AddInstanceToUpdateGroup_Concurrent(const int32 GroupIndex,
                                                                    const FTurboSequence_MinimalMeshData_Lf& MeshData)
{
	{
		FScopeLock Lock(&Instance->GetThreadContext()->CriticalSection);

		if (!GlobalLibrary.UpdateGroups.IsValidIndex(GroupIndex))
		{
			GlobalLibrary.UpdateGroups.SetNum(GroupIndex + GET1_NUMBER);
		}

		FTurboSequence_UpdateGroup_Lf& Group = GlobalLibrary.UpdateGroups[GroupIndex];

		if (!Group.RawIDData.Contains(MeshData.RootMotionMeshID))
		{
			Group.RawIDData.Add(MeshData.RootMotionMeshID, Group.RawIDData.Num());
			Group.RawIDs.Add(MeshData.RootMotionMeshID);
			Group.MeshIDToMinimal.Add(MeshData, FIntVector2(GET1_NUMBER, Group.MeshIDToMinimal.Num()));
			Group.RawMinimalData.Add(MeshData);
			for (const int64& MeshID : MeshData.CustomizableMeshIDs)
			{
				if (!Group.RawIDData.Contains(MeshID))
				{
					Group.RawIDData.Add(MeshID, Group.RawIDData.Num());
					Group.RawIDs.Add(MeshID);
					Group.MeshIDToMinimal[MeshData].X++;
				}
			}
		}
	}
}

void ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_RawID_Concurrent(const int32 GroupIndex, const int64& MeshID)
{
	//int32 RawIndexToRemove = INDEX_NONE;
	{
		FScopeLock Lock(&Instance->GetThreadContext()->CriticalSection);

		if (GlobalLibrary.UpdateGroups.IsValidIndex(GroupIndex))
		{
			FTurboSequence_UpdateGroup_Lf& Group = GlobalLibrary.UpdateGroups[GroupIndex];

			if (Group.RawIDData.Contains(MeshID))
			{
				const int32 RawIndexToRemove = Group.RawIDData[MeshID];

				Group.RawIDData.Remove(MeshID);
				Group.RawIDs.RemoveAt(RawIndexToRemove);

				for (TTuple<uint32, int32>& Data : Group.RawIDData)
				{
					if (Data.Value > RawIndexToRemove)
					{
						Data.Value--;
					}
				}

				if (Group.MeshIDToMinimal.Contains(GlobalLibrary.MeshIDToMinimalData[MeshID]))
				{
					Group.MeshIDToMinimal[GlobalLibrary.MeshIDToMinimalData[MeshID]].X--;
					if (Group.MeshIDToMinimal[GlobalLibrary.MeshIDToMinimalData[MeshID]].X <= GET0_NUMBER)
					{
						const int32 MinimalIndexToRemove = Group.MeshIDToMinimal[GlobalLibrary.MeshIDToMinimalData[MeshID]].Y;

						Group.MeshIDToMinimal.Remove(GlobalLibrary.MeshIDToMinimalData[MeshID]);
						Group.RawMinimalData.RemoveAt(MinimalIndexToRemove);

						for (TTuple<FTurboSequence_MinimalMeshData_Lf, FIntVector2>& Data : Group.MeshIDToMinimal)
						{
							if (Data.Value.Y > MinimalIndexToRemove)
							{
								Data.Value.Y--;
							}
						}
					}
				}
			}
		}
	}
}

void ATurboSequence_Manager_Lf::RemoveInstanceFromUpdateGroup_Concurrent(const int32 GroupIndex,
                                                                         const FTurboSequence_MinimalMeshData_Lf& MeshData)
{
	RemoveInstanceFromUpdateGroup_RawID_Concurrent(GroupIndex, MeshData.RootMotionMeshID);
	for (const int64& MeshID : MeshData.CustomizableMeshIDs)
	{
		RemoveInstanceFromUpdateGroup_RawID_Concurrent(GroupIndex, MeshID);
	}
}

int32 ATurboSequence_Manager_Lf::GetNumMeshCollectionsInUpdateGroup_Concurrent(const int32 GroupIndex)
{
	{
		FScopeLock Lock(&Instance->GetThreadContext()->CriticalSection);

		if (!GlobalLibrary.UpdateGroups.IsValidIndex(GroupIndex))
		{
			return INDEX_NONE;
		}

		return GlobalLibrary.UpdateGroups[GroupIndex].MeshIDToMinimal.Num();
	}
}

int32 ATurboSequence_Manager_Lf::GetNumMeshIDsInUpdateGroup_RawID_Concurrent(const int32 GroupIndex)
{
	{
		FScopeLock Lock(&Instance->GetThreadContext()->CriticalSection);

		if (!GlobalLibrary.UpdateGroups.IsValidIndex(GroupIndex))
		{
			return INDEX_NONE;
		}

		return GlobalLibrary.UpdateGroups[GroupIndex].RawIDData.Num();
	}
}

void ATurboSequence_Manager_Lf::CleanManager_GameThread(const bool& bIsEndPlay)
{
	if (bIsEndPlay && IsValid(Instance))
	{
		ENQUEUE_RENDER_COMMAND(TurboSequence_EndPlayBufferClear_Lf)(
			[&](FRHICommandListImmediate& RHICmdList)
			{
				ClearBuffers();
			});

		Instance = nullptr;
	}
	else
	{
		ClearBuffers();
	}
}
