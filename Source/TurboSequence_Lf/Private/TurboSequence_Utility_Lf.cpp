// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#include "TurboSequence_Utility_Lf.h"

#include "BoneWeights.h"
#include "MeshDescription.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "Rendering/SkeletalMeshModel.h"


void FTurboSequence_Utility_Lf::CreateAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset,
                                                           const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
                                                           FSkinnedMeshGlobalLibrary_Lf& Library,
                                                           FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                           Library_RenderThread, FCriticalSection& CriticalSection)
{
	CreateTurboSequenceReference(Library, Library_RenderThread, CriticalSection, FromAsset);
	if (Library.PerReferenceData.Contains(FromAsset))
	{
		RefreshAsyncChunkedMeshData(GlobalData, Library, Library_RenderThread, CriticalSection);
	}
}

void FTurboSequence_Utility_Lf::RefreshAsyncChunkedMeshData(const TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData,
                                                            FSkinnedMeshGlobalLibrary_Lf& Library,
                                                            FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                            Library_RenderThread, FCriticalSection& CriticalSection)
{
	CreateBoneMaps(Library, Library_RenderThread, CriticalSection);
	CreateInverseReferencePose(Library, Library_RenderThread, CriticalSection);
}

void FTurboSequence_Utility_Lf::CreateTurboSequenceReference(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                             FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                             Library_RenderThread, FCriticalSection& CriticalSection,
                                                             const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset)
{
	if (!IsValid(FromAsset))
	{
		return;
	}

	if (!IsValid(FromAsset->GetSkeleton()))
	{
		return;
	}

	if (!FromAsset->MeshData.Num())
	{
		return;
	}

	if (!FromAsset->InstancedMeshes.Num())
	{
		return;
	}

	if (FromAsset->bExcludeFromSystem)
	{
		return;
	}

	FSkinnedMeshReference_Lf Reference = FSkinnedMeshReference_Lf(FromAsset);
	FSkinnedMeshReference_RenderThread_Lf Reference_RenderThread = FSkinnedMeshReference_RenderThread_Lf();


	CreateLevelOfDetails(Reference, Reference_RenderThread, CriticalSection, FromAsset);

	const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(FromAsset);
	const TArray<FTransform>& ReferencePose = GetSkeletonRefPose(ReferenceSkeleton);
	Reference.FirstReferenceBone = ReferencePose[GET0_NUMBER];

	CriticalSection.Lock();
	Library.PerReferenceDataKeys.Add(FromAsset);
	Reference_RenderThread.DataAsset = FromAsset;
	Library.PerReferenceData.Add(FromAsset, Reference);
	CriticalSection.Unlock();

	ENQUEUE_RENDER_COMMAND(TurboSequence_AddRenderThreadReference_Lf)(
		[&Library_RenderThread, Reference_RenderThread](FRHICommandListImmediate& RHICmdList)
		{
			Library_RenderThread.PerReferenceDataKeys.Add(Reference_RenderThread.DataAsset);
			Library_RenderThread.PerReferenceData.Add(Reference_RenderThread.DataAsset, Reference_RenderThread);

			const int16 NumReferences = Library_RenderThread.PerReferenceData.Num();
			for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
			{
				FSkinnedMeshReference_RenderThread_Lf& ReferenceOther = Library_RenderThread.PerReferenceData[
					Library_RenderThread.PerReferenceDataKeys[i]];

				ReferenceOther.ReferenceCollectionIndex = i;
			}
		});

	const int16 NumReferences = Library.PerReferenceData.Num();
	for (int16 i = GET0_NUMBER; i < NumReferences; ++i)
	{
		FSkinnedMeshReference_Lf& ReferenceOther = Library.PerReferenceData[Library.PerReferenceDataKeys[i]];

		ReferenceOther.ReferenceCollectionIndex = i;
	}

	UE_LOG(LogTurboSequence_Lf, Display, TEXT("Adding Mesh Asset to Library at Path | -> %s"),
	       *FromAsset->GetPathName());
}

void FTurboSequence_Utility_Lf::CreateGPUBones(FSkinnedMeshReferenceLodElement_Lf& LodElement,
                                               FCriticalSection& CriticalSection,
                                               const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
                                               bool bIsMeshDataEvaluation)
{
	if (!LodElement.CPUBoneToGPUBoneIndicesMap.Num())
	{
		if (bIsMeshDataEvaluation)
		{
			const FSkeletalMeshLODRenderData& RenderData = Asset->ReferenceMeshEdited->GetResourceForRendering()->
			                                                      LODRenderData[LodElement.MeshIndex];
			const FSkinWeightVertexBuffer* SkinWeightBuffer = RenderData.GetSkinWeightVertexBuffer();
			uint32 NumVertices = RenderData.GetNumVertices();
			for (uint32 i = GET0_NUMBER; i < NumVertices; ++i)
			{
				int32 VertexIndexSection;
				int32 SectionIndex;
				RenderData.GetSectionFromVertexIndex(i, SectionIndex, VertexIndexSection);

				const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];
				uint32 VertIdx = Section.BaseVertexIndex + VertexIndexSection;

				for (uint16 InfluenceIdx = GET0_NUMBER; InfluenceIdx < GET12_NUMBER; ++InfluenceIdx)
				{
					if (int32 BoneIndex =
							GetBoneMapIndex_CPU(Section.BoneMap, SkinWeightBuffer->GetBoneIndex(VertIdx, InfluenceIdx));
						SkinWeightBuffer->GetBoneWeight(VertIdx, InfluenceIdx) && BoneIndex > INDEX_NONE && !LodElement.
						CPUBoneToGPUBoneIndicesMap.Contains(BoneIndex))
					{
						CriticalSection.Lock();
						LodElement.CPUBoneToGPUBoneIndicesMap.Add(BoneIndex, GET0_NUMBER);
						CriticalSection.Unlock();
					}
				}
			}

			CriticalSection.Lock();
			// This actually generates the bones in BoneTree formation, I hope it's robust enough ...
			FTurboSequence_Helper_Lf::SortKeyFromStartLowToEndHigh(LodElement.CPUBoneToGPUBoneIndicesMap);

			Asset->MeshData[LodElement.CollectionIndex].CPUBoneToGPUBoneIndicesMap.Empty();

			uint16 BoneIndex = GET0_NUMBER;
			for (TTuple<uint16, uint16>& BoneIndicesMap : LodElement.CPUBoneToGPUBoneIndicesMap)
			{
				BoneIndicesMap.Value = BoneIndex;
				BoneIndex++;

				Asset->MeshData[LodElement.CollectionIndex].CPUBoneToGPUBoneIndicesMap.Add(
					BoneIndicesMap.Key, BoneIndicesMap.Value);
			}
			CriticalSection.Unlock();
		}
		else
		{
			if (Asset->MeshData.Num())
			{
				CriticalSection.Lock();
				for (const TTuple<int32, int32>& BoneIndicesMap : Asset->MeshData[LodElement.CollectionIndex].
				     CPUBoneToGPUBoneIndicesMap)
				{
					LodElement.CPUBoneToGPUBoneIndicesMap.Add(BoneIndicesMap.Key, BoneIndicesMap.Value);
				}
				CriticalSection.Unlock();
			}
			else
			{
				UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Not Valid Mesh Data..."));
			}
		}
	}
}

void FTurboSequence_Utility_Lf::CreateBoneMaps(FSkinnedMeshGlobalLibrary_Lf& Library,
                                               FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                               FCriticalSection& CriticalSection)
{
	if (Library.PerReferenceData.Num())
	{
		const int16 PreviousNumCPUBones = Library.MaxNumCPUBones;
		CreateMaxLodAndCPUBones(Library);

		if (PreviousNumCPUBones != Library.MaxNumCPUBones)
		{
			Library.bMaxNumCPUBonesChanged = true;
		}

		int16 MaxNumCPUBones = Library.MaxNumCPUBones;
		int16 MaxNumMeshLevelOfDetails = Library.MaxNumLevelOfDetailsWithMesh;
		int32 MaxNumGPUBones = GET0_NUMBER;

		ENQUEUE_RENDER_COMMAND(TurboSequence_UpdateBoneCollection_Lf)(
			[&Library_RenderThread, MaxNumCPUBones, MaxNumMeshLevelOfDetails](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.BoneTransformParams.NumMaxCPUBones = MaxNumCPUBones;
				Library_RenderThread.BoneTransformParams.NumMaxLevelOfDetails = MaxNumMeshLevelOfDetails;

				int32 NumReferences = Library_RenderThread.PerReferenceData.Num();
				Library_RenderThread.BoneTransformParams.ReferenceNumCPUBones.SetNum(NumReferences);
				for (int32 i = GET0_NUMBER; i < NumReferences; ++i)
				{
					FSkinnedMeshReference_RenderThread_Lf& Reference = Library_RenderThread.PerReferenceData[
						Library_RenderThread.PerReferenceDataKeys[i]];

					int32 NumCpuBones = GetSkeletonNumBones(GetReferenceSkeleton(Reference.DataAsset));
					Library_RenderThread.BoneTransformParams.ReferenceNumCPUBones[i] = NumCpuBones;
				}
			});

		int16 NumReferences = Library.PerReferenceData.Num();

		TArray<FVector4f> CachedIndices;
		int32 NumBoneIndices = NumReferences * MaxNumCPUBones * MaxNumMeshLevelOfDetails;
		CachedIndices.AddUninitialized(NumBoneIndices);
		for (int32 RefIdx = GET0_NUMBER; RefIdx < NumReferences; ++RefIdx)
		{
			const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Library.PerReferenceDataKeys[RefIdx];
			FSkinnedMeshReference_Lf& Reference = Library.PerReferenceData[Asset];
			Reference.NumCPUBones = GetSkeletonNumBones(GetReferenceSkeleton(Reference.DataAsset));

			const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Asset);
			//FromAsset->ReferenceSkeleton->GetMeshBoneIndexFromSkeletonBoneIndex()
			uint8 NumLevelOfDetails = Reference.LevelOfDetails.Num();
			int16 FirstGPULodCollectionIndex = NumLevelOfDetails - GET1_NUMBER;
			bool bHasRenderState = false;
			ParallelFor(NumLevelOfDetails, [&](int32 LodIdx)
			{
				FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[LodIdx];
				if (!LodElement.bIsRenderStateValid)
				{
					return;
				}

				CriticalSection.Lock();
				if (LodIdx < FirstGPULodCollectionIndex)
				{
					FirstGPULodCollectionIndex = LodIdx;
				}
				bHasRenderState = true;
				CriticalSection.Unlock();

				CreateGPUBones(LodElement, CriticalSection, Asset, false);

				int32 BaseIndex = RefIdx * MaxNumMeshLevelOfDetails * MaxNumCPUBones + LodElement.GPUMeshIndex *
					MaxNumCPUBones;
				for (uint16 i = GET0_NUMBER; i < MaxNumCPUBones; ++i)
				{
					FVector4f Data;
					if (GetSkeletonIsValidIndex(ReferenceSkeleton, i))
					{
						Data.X = i;
						if (LodElement.CPUBoneToGPUBoneIndicesMap.Contains(i))
						{
							Data.Y = LodElement.CPUBoneToGPUBoneIndicesMap[i];
						}
						else
						{
							Data.Y = INDEX_NONE;
						}
						Data.Z = GetSkeletonParentIndex(ReferenceSkeleton, i);
						Data.W = GET0_NUMBER; // Reserved
					}
					else // It's not part of the rig anymore, so tell the Compute Shader to not compute it
					{
						Data.X = INDEX_NONE;
						Data.Y = INDEX_NONE;
						Data.Z = INDEX_NONE;
						Data.W = GET0_NUMBER;
					}
					CachedIndices[BaseIndex + i] = Data;
				}
			}, EParallelForFlags::BackgroundPriority);


			if (!Reference.FirstLodGPUBonesCollection.Num() && bHasRenderState)
			{
				const FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[
					FirstGPULodCollectionIndex];

				Reference.NumFirstLodGPUBones = LodElement.CPUBoneToGPUBoneIndicesMap.Num();
				Reference.FirstLodGPUBonesCollection = LodElement.CPUBoneToGPUBoneIndicesMap;

				TMap<uint16, uint16> GPUBoneCollection = LodElement.CPUBoneToGPUBoneIndicesMap;
				ENQUEUE_RENDER_COMMAND(TurboSequence_UpdateGPUBoneCollection_Lf)(
					[&Library_RenderThread, Asset, GPUBoneCollection](FRHICommandListImmediate& RHICmdList)
					{
						Library_RenderThread.PerReferenceData[Asset].FirstLodGPUBonesCollection = GPUBoneCollection;
					});
			}

			if (int32 NumBones = Reference.NumFirstLodGPUBones; MaxNumGPUBones <= NumBones)
			{
				MaxNumGPUBones = NumBones;
			}
		}

		Library.MaxNumGPUBones = MaxNumGPUBones;
		ENQUEUE_RENDER_COMMAND(TurboSequence_FillBoneCollection_Lf)(
			[&Library_RenderThread, CachedIndices, MaxNumGPUBones](FRHICommandListImmediate& RHICmdList)
			{
				Library_RenderThread.BoneTransformParams.NumMaxGPUBones = MaxNumGPUBones;
				Library_RenderThread.BoneTransformParams.Indices = CachedIndices;
			});
	}
}

void FTurboSequence_Utility_Lf::CreateRawSkinWeightTextureBuffer(
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> FromAsset, const TFunction<void(bool bSuccess)>& PostCall,
	const TObjectPtr<UWorld> World)
{
#if WITH_EDITOR
	if (!IsValid(FromAsset->GlobalData))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Global data is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	if (!IsValid(FromAsset->GlobalData->SkinWeightTexture))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Skin Weight Texture is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	if (!IsValid(FromAsset->ReferenceMeshEdited))
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("it Seems the Reference Mesh is Missing...."));
		FTimerDelegate Callback;
		Callback.BindLambda([PostCall]
		{
			PostCall(false);
		});
		World->GetTimerManager().SetTimerForNextTick(Callback);
		return;
	}

	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams = FSettingsComputeShader_Params_Lf();
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.bUse32BitTexture = false;
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.ShaderID = GetTypeHash(FromAsset);
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.Empty();

	FromAsset->SetReferenceSkeleton(GetReferenceSkeleton(FromAsset, true));

	FCriticalSection CriticalSection;

	FSkinnedMeshReference_Lf Reference = FSkinnedMeshReference_Lf(FromAsset);
	FSkinnedMeshReference_RenderThread_Lf Reference_RenderThread = FSkinnedMeshReference_RenderThread_Lf();

	CreateLevelOfDetails(Reference, Reference_RenderThread, CriticalSection, FromAsset, true);

	int32 NumPixels = GET0_NUMBER;
	for (const TTuple<uint8, FSkinnedMeshReferenceLodElement_Lf>& LevelOfDetail : Reference.LevelOfDetails)
	{
		if (!LevelOfDetail.Value.bIsRenderStateValid)
		{
			continue;
		}

		const FSkeletalMeshLODRenderData& RenderData = FromAsset->ReferenceMeshEdited->GetResourceForRendering()->
		                                                          LODRenderData[LevelOfDetail.Value.MeshIndex];
		uint32 NumVertices = RenderData.GetNumVertices();

		NumPixels += NumVertices * FTurboSequence_Helper_Lf::NumSkinWeightPixels;
	}
	FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.AddUninitialized(NumPixels);

	uint8 NumLevelOfDetails = Reference.LevelOfDetails.Num();
	ParallelFor(NumLevelOfDetails, [&](int32 LodIdx)
	{
		FSkinnedMeshReferenceLodElement_Lf& LodElement = Reference.LevelOfDetails[LodIdx];
		if (!LodElement.bIsRenderStateValid)
		{
			return;
		}

		if (!FromAsset->MeshDataOrderView.IsValidIndex(LodElement.MeshIndex))
		{
			return;
		}

		CreateGPUBones(LodElement, CriticalSection, FromAsset, true);

		const FSkeletalMeshLODRenderData& RenderData = FromAsset->ReferenceMeshEdited->GetResourceForRendering()->
		                                                          LODRenderData[LodElement.MeshIndex];
		const FSkinWeightVertexBuffer* SkinWeightBuffer = RenderData.GetSkinWeightVertexBuffer();


		const TArray<int32>& VertexIndices = FromAsset->MeshDataOrderView[LodElement.MeshIndex].StaticMeshIndices;

		int32 NumVertices = VertexIndices.Num();
		for (int32 VertIdx = GET0_NUMBER; VertIdx < NumVertices; ++VertIdx)
		{
			int32 RealVertexIndex = VertexIndices[VertIdx];

			int32 VertexIndexSection;
			int32 SectionIndex;
			RenderData.GetSectionFromVertexIndex(RealVertexIndex, SectionIndex, VertexIndexSection);


			const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];

			uint8 MaxSkinWeightVertex = GET0_NUMBER;
			for (uint16 InfluenceChunkIdx = GET0_NUMBER; InfluenceChunkIdx < GET3_NUMBER; ++InfluenceChunkIdx)
			{
				uint8 ChunkIndex = InfluenceChunkIdx * GET4_NUMBER; // 12 SkinData

				FVector4f Weights;
				FVector4f Indices;
				for (int32 WeightIdx = GET0_NUMBER; WeightIdx < GET4_NUMBER; ++WeightIdx)
				{
					uint16 RawWeight = SkinWeightBuffer->GetBoneWeight(RealVertexIndex, ChunkIndex + WeightIdx);

					float WeightFloat = static_cast<float>(RawWeight) / static_cast<float>(0xFFFF);

					uint8 Weight = WeightFloat * 0xFF;

					Weights[WeightIdx] = Weight;

					int32 RawBoneIndex = SkinWeightBuffer->GetBoneIndex(RealVertexIndex, ChunkIndex + WeightIdx);

					int32 IndexedBoneIndex = GetBoneMapIndex_GPU(LodElement.CPUBoneToGPUBoneIndicesMap,
					                                             Section.BoneMap, RawBoneIndex, Weight);


					Indices[WeightIdx] = IndexedBoneIndex;

					if (Weight)
					{
						MaxSkinWeightVertex = ChunkIndex + WeightIdx;
					}
				}


				FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
						RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels + InfluenceChunkIdx *
						GET2_NUMBER] =
					Indices;
				FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
					RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels + InfluenceChunkIdx * GET2_NUMBER +
					GET1_NUMBER] = Weights;
			}

			FVector4f PerVertexCustomData_0;
			PerVertexCustomData_0.X = GET12_NUMBER;
			PerVertexCustomData_0.Y = GET0_NUMBER;
			PerVertexCustomData_0.Z = GET0_NUMBER;
			PerVertexCustomData_0.W = GET0_NUMBER;

			FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput[LodElement.SkinWeightOffset +
				RealVertexIndex * FTurboSequence_Helper_Lf::NumSkinWeightPixels +
				FTurboSequence_Helper_Lf::NumSkinWeightPixels - GET1_NUMBER] = PerVertexCustomData_0;
		}
	}, EParallelForFlags::BackgroundPriority);

	const int32 Slice = FMath::Min(
		FMath::CeilToInt(
			static_cast<float>(FromAsset->GlobalData->CachedMeshDataCreationSettingsParams.SettingsInput.Num()) /
			static_cast<float>(GET128_NUMBER * GET128_NUMBER)), 1023);

	FromAsset->GlobalData->SkinWeightTexture->Init(GET128_NUMBER, GET128_NUMBER, Slice, PF_FloatRGBA);
	FromAsset->GlobalData->SkinWeightTexture->UpdateResourceImmediate(true);

	FromAsset->MeshDataCustomData = GET24_NUMBER;

	FTimerDelegate WaitTimerCallback;
	WaitTimerCallback.BindLambda([FromAsset, PostCall, World, WaitTimerCallback]
	{
		FromAsset->MeshDataCustomData--;
		if (FromAsset->MeshDataCustomData < GET0_NUMBER)
		{
			PostCall(false);
			return;
		}

		if (!FromAsset->GlobalData->SkinWeightTexture->HasPendingInitOrStreaming(true) && !FromAsset->GlobalData->
			SkinWeightTexture->HasPendingRenderResourceInitialization())
		{
			bool bValidPixels = true;
			if (bValidPixels)
			{
				ENQUEUE_RENDER_COMMAND(TurboSequence_FillRenderThreadSkinWeightData_Lf)(
					[FromAsset](FRHICommandListImmediate& RHICmdList)
					{
						FSettingsCompute_Shader_Execute_Lf::DispatchRenderThread(
							RHICmdList, FromAsset->GlobalData->CachedMeshDataCreationSettingsParams,
							FromAsset->GlobalData->SkinWeightTexture);
					});

				ENQUEUE_RENDER_COMMAND(TurboSequence_FillRenderThreadSkinWeightData_Iteration2_Lf)(
					[PostCall, World, FromAsset](FRHICommandListImmediate& RHICmdList)
					{
						AsyncTask(ENamedThreads::GameThread, [PostCall, World, FromAsset]
						{
							FTimerDelegate WaitTimer2Callback;
							WaitTimer2Callback.BindLambda([FromAsset, PostCall, World, WaitTimer2Callback]
							{
								FromAsset->MeshDataCustomData--;
								if (FromAsset->MeshDataCustomData < GET0_NUMBER)
								{
									FromAsset->GlobalData->CachedMeshDataCreationSettingsParams =
										FSettingsComputeShader_Params_Lf();
									PostCall(false);
									return;
								}

								if (!FromAsset->GlobalData->SkinWeightTexture->HasPendingInitOrStreaming(true) && !
									FromAsset->GlobalData->SkinWeightTexture->HasPendingRenderResourceInitialization())
								{
									FromAsset->GlobalData->CachedMeshDataCreationSettingsParams =
										FSettingsComputeShader_Params_Lf();
									PostCall(true);
								}
								else
								{
									World->GetTimerManager().SetTimerForNextTick(WaitTimer2Callback);
								}
							});
							World->GetTimerManager().SetTimerForNextTick(WaitTimer2Callback);
						});
					});
			}
			else
			{
				World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);
			}
		}
		else
		{
			World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);
		}
	});
	World->GetTimerManager().SetTimerForNextTick(WaitTimerCallback);

#endif
}

void FTurboSequence_Utility_Lf::RemoveAnimationFromLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                                  FSkinnedMeshGlobalLibrary_RenderThread_Lf&
                                                                  Library_RenderThread,
                                                                  FCriticalSection& CriticalSection)
{
}


int32 FTurboSequence_Utility_Lf::AddAnimationToLibraryChunked(FSkinnedMeshGlobalLibrary_Lf& Library,
                                                              FCriticalSection& CriticalSection,
                                                              int32& CPUIndices,
                                                              const FSkinnedMeshRuntime_Lf& Runtime,
                                                              const FAnimationMetaData_Lf& Animation)
{
	// Slow...
	FScopeLock Lock(&CriticalSection);

	int32 PoseIndices = GET0_NUMBER;
	CPUIndices = GET0_NUMBER;

	if (!Library.AnimationLibraryData.Contains(Animation.AnimationLibraryHash))
	{
		UE_LOG(LogTemp, Warning, TEXT("Don't Contains Animation Library Data"));
		return PoseIndices;
	}


	FAnimationLibraryData_Lf& LibraryAnimData = Library.AnimationLibraryData[Animation.AnimationLibraryHash];

	if (IsValid(Animation.Animation))
	{
		if (!LibraryAnimData.KeyframesFilled.Num())
		{
			LibraryAnimData.MaxFrames = Animation.Animation->GetPlayLength() / Runtime.DataAsset->
				TimeBetweenAnimationLibraryFrames - GET1_NUMBER;


			LibraryAnimData.KeyframesFilled.Init(INDEX_NONE, LibraryAnimData.MaxFrames);
			if (!LibraryAnimData.KeyframesFilled.Num())
			{
				return PoseIndices;
			}
		}

		int32 Pose0;
		AnimationCodecTimeToIndex(Animation.AnimationNormalizedTime, LibraryAnimData.MaxFrames,
		                          Animation.Animation->Interpolation, Pose0);

		if (!LibraryAnimData.KeyframesFilled.IsValidIndex(Pose0))
		{
			return PoseIndices;
		}

		PoseIndices = LibraryAnimData.KeyframesFilled[Pose0];
		CPUIndices = Pose0;

		if (LibraryAnimData.KeyframesFilled[Pose0] > INDEX_NONE)
		{
			return PoseIndices;
		}

		LibraryAnimData.KeyframesFilled[Pose0] = Library.AnimationLibraryMaxNum;

		PoseIndices = LibraryAnimData.KeyframesFilled[Pose0];

		const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);

		const FReferenceSkeleton& AnimationSkeleton = Animation.Animation->GetSkeleton()->GetReferenceSkeleton();

		if (!LibraryAnimData.bHasPoseData)
		{
			LibraryAnimData.bHasPoseData = true;

			LibraryAnimData.PoseOptions = FAnimPoseEvaluationOptions_Lf();
			LibraryAnimData.PoseOptions.bEvaluateCurves = true;
			LibraryAnimData.PoseOptions.bShouldRetarget = true;
			LibraryAnimData.PoseOptions.bExtractRootMotion = false;
			LibraryAnimData.PoseOptions.bRetrieveAdditiveAsFullPose = true;
			bool bNeedOptionalSkeletonMesh = true;
			const uint16 NumAnimationBones = AnimationSkeleton.GetNum();
			for (uint16 b = GET0_NUMBER; b < NumAnimationBones; ++b)
			{
				if (const FName& BoneName = GetSkeletonBoneName(AnimationSkeleton, b); GetSkeletonBoneIndex(
					ReferenceSkeleton, BoneName) < GET0_NUMBER)
				{
					bNeedOptionalSkeletonMesh = false;
					break;
				}
			}
			if (bNeedOptionalSkeletonMesh)
			{
				LibraryAnimData.PoseOptions.OptionalSkeletalMesh = Runtime.DataAsset->ReferenceMeshNative;
			}
			if (Animation.Animation->IsCompressedDataValid())
			{
				LibraryAnimData.PoseOptions.EvaluationType = EAnimDataEvalType_Lf::Compressed;
			}

			FAnimPose_Lf AlphaPose;
			FTurboSequence_Helper_Lf::GetPoseInfo(GET0_NUMBER, Animation.Animation, LibraryAnimData.PoseOptions,
			                                      AlphaPose, CriticalSection);

			for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
			{
				const FName& BoneName = GetSkeletonBoneName(ReferenceSkeleton, b);
				if (int32 BoneIndex = FTurboSequence_Helper_Lf::GetAnimationBonePoseIndex(AlphaPose, BoneName);
					BoneIndex > INDEX_NONE)
				{
					LibraryAnimData.BoneNameToAnimationBoneIndex.FindOrAdd(BoneName, BoneIndex);
				}
			}
		}


		if (!LibraryAnimData.AnimPoses.Contains(Pose0))
		{
			float FrameTime0 = static_cast<float>(Pose0) / static_cast<float>(LibraryAnimData.MaxFrames -
				GET1_NUMBER) * Animation.Animation->GetPlayLength();

			FAnimPose_Lf PoseData_0;
			FTurboSequence_Helper_Lf::GetPoseInfo(FrameTime0, Animation.Animation, LibraryAnimData.PoseOptions,
			                                      PoseData_0, CriticalSection);

			FCPUAnimationPose_Lf CPUPose_0;
			CPUPose_0.Pose = PoseData_0;
			CPUPose_0.BelongsToKeyframe = Pose0;
			LibraryAnimData.AnimPoses.Add(Pose0, CPUPose_0);
		}

		uint32 AnimationDataIndex = Library.AnimationLibraryDataAllocatedThisFrame.Num();

		int32 NumAllocations = LibraryAnimData.NumBones * GET3_NUMBER;
		Library.AnimationLibraryMaxNum += NumAllocations;

		Library.AnimationLibraryDataAllocatedThisFrame.AddUninitialized(NumAllocations);
		//LibraryAnimData.AnimPoses[Pose0].RawData.AddUninitialized(NumAllocations);
		for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
		{
			const FTurboSequence_TransposeMatrix_Lf& BoneSpaceTransform = GetBoneTransformFromLocalPoses(
				b, LibraryAnimData, ReferenceSkeleton, AnimationSkeleton, LibraryAnimData.AnimPoses[Pose0].Pose,
				Animation.Animation);

			// NOTE: Keep in mind this matrix is not in correct order after uploading it
			//		 for performance reason we using matrix calculations which match the oder
			//		 in the Vertex Shader
			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				Library.AnimationLibraryDataAllocatedThisFrame[AnimationDataIndex] = BoneSpaceTransform.Colum[M];
				//LibraryAnimData.AnimPoses[Pose0].RawData[b * GET3_NUMBER + M] = BoneData;

				AnimationDataIndex++;
			}
		}
	}
	else // Is Rest Pose
	{
		CPUIndices = GET0_NUMBER;

		if (LibraryAnimData.KeyframesFilled.Num() && LibraryAnimData.KeyframesFilled[GET0_NUMBER] > INDEX_NONE)
		{
			PoseIndices = LibraryAnimData.KeyframesFilled[GET0_NUMBER];
			return PoseIndices;
		}

		if (!LibraryAnimData.KeyframesFilled.Num())
		{
			LibraryAnimData.KeyframesFilled.Init(Library.AnimationLibraryMaxNum, GET1_NUMBER);
			LibraryAnimData.MaxFrames = GET1_NUMBER;

			//const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
			//LibraryAnimData.NumBones = GetSkeletonNumBones(ReferenceSkeleton);
		}
		else
		{
			LibraryAnimData.KeyframesFilled[GET0_NUMBER] = Library.AnimationLibraryMaxNum;
		}
		PoseIndices = LibraryAnimData.KeyframesFilled[GET0_NUMBER];

		if (!LibraryAnimData.AnimPoses.Contains(GET0_NUMBER))
		{
			FCPUAnimationPose_Lf CPUPose_0;
			CPUPose_0.BelongsToKeyframe = GET0_NUMBER;
			LibraryAnimData.AnimPoses.Add(GET0_NUMBER, CPUPose_0);
		}

		int32 NumAllocations = LibraryAnimData.NumBones * GET3_NUMBER;
		Library.AnimationLibraryMaxNum += NumAllocations;

		uint32 AnimationDataIndex = Library.AnimationLibraryDataAllocatedThisFrame.Num();

		const FReferenceSkeleton& ReferenceSkeleton = GetReferenceSkeleton(Runtime.DataAsset);
		Library.AnimationLibraryDataAllocatedThisFrame.AddUninitialized(NumAllocations);
		for (uint16 b = GET0_NUMBER; b < LibraryAnimData.NumBones; ++b)
		{
			const FMatrix& BoneMatrix = GetSkeletonRefPose(ReferenceSkeleton)[b].ToMatrixWithScale();
			// NOTE: Keep in mind this matrix is not in correct order after uploading it
			//		 for performance reason we using matrix calculations which match the oder
			//		 in the Vertex Shader
			for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
			{
				FVector4f BoneData;
				BoneData.X = BoneMatrix.M[GET0_NUMBER][M];
				BoneData.Y = BoneMatrix.M[GET1_NUMBER][M];
				BoneData.Z = BoneMatrix.M[GET2_NUMBER][M];
				BoneData.W = BoneMatrix.M[GET3_NUMBER][M];

				Library.AnimationLibraryDataAllocatedThisFrame[AnimationDataIndex] = BoneData;
				//LibraryAnimData.AnimPoses[GET0_NUMBER].RawData[b * GET3_NUMBER + M] = BoneData;

				AnimationDataIndex++;
			}
		}
	}

	return PoseIndices;
}

void FTurboSequence_Utility_Lf::CustomizeMesh(FSkinnedMeshRuntime_Lf& Runtime,
                                              const TObjectPtr<UTurboSequence_MeshAsset_Lf> TargetMesh,
                                              const TArray<TObjectPtr<UMaterialInterface>>& TargetMaterials,
                                              TMap<TObjectPtr<UTurboSequence_MeshAsset_Lf>, FRenderingMaterialMap_Lf>&
                                              NiagaraComponents, FSkinnedMeshGlobalLibrary_Lf& Library,
                                              FSkinnedMeshGlobalLibrary_RenderThread_Lf& Library_RenderThread,
                                              const TObjectPtr<USceneComponent> RootComponent,
                                              FCriticalSection& CriticalSection)
{
	FSkinnedMeshReference_Lf& PreReference = Library.PerReferenceData[Runtime.DataAsset];

	if (!Library.PerReferenceData.Contains(TargetMesh))
	{
		CreateAsyncChunkedMeshData(TargetMesh, TargetMesh->GlobalData, Library, Library_RenderThread, CriticalSection);
	}

	FSkinnedMeshReference_Lf& PostReference = Library.PerReferenceData[TargetMesh];

	RemoveRenderInstance(PreReference, Runtime, CriticalSection);
	if (const FRenderData_Lf& PreRenderData = PreReference.RenderData[Runtime.MaterialsHash]; !PreRenderData.InstanceMap
		.Num())
	{
		CleanNiagaraRenderer(NiagaraComponents, PreReference, Runtime);
	}


	TArray<TObjectPtr<UMaterialInterface>> Materials;
	Materials.SetNum(TargetMaterials.Num());
	uint8 NumOverrideMaterials = TargetMaterials.Num();
	for (uint8 MaterialIdx = GET0_NUMBER; MaterialIdx < NumOverrideMaterials; ++MaterialIdx)
	{
		Materials[MaterialIdx] = TargetMaterials[MaterialIdx];
	}
	if (!Materials.Num() && IsValid(PostReference.FirstValidMeshLevelOfDetail))
	{
		const TArray<FStaticMaterial>& MeshMaterials = PostReference.FirstValidMeshLevelOfDetail->GetStaticMaterials();
		uint8 NumMeshMaterialsMaterials = MeshMaterials.Num();
		Materials.SetNum(NumMeshMaterialsMaterials);
		for (uint8 MaterialIdx = GET0_NUMBER; MaterialIdx < NumMeshMaterialsMaterials; ++MaterialIdx)
		{
			Materials[MaterialIdx] = MeshMaterials[MaterialIdx].MaterialInterface;
		}
	}

	uint32 MaterialsHash = FTurboSequence_Helper_Lf::GetArrayHash(Materials);
	if (!NiagaraComponents.Contains(TargetMesh) || (NiagaraComponents.Contains(TargetMesh) && !
		NiagaraComponents[TargetMesh].NiagaraRenderer.Contains(MaterialsHash)))
	{
		CreateRenderer(PostReference, TargetMesh->GlobalData, TargetMesh->RendererSystem,
		               PostReference.LevelOfDetails, RootComponent,
		               NiagaraComponents, TargetMesh, Materials, MaterialsHash);
	}
	Runtime.MaterialsHash = MaterialsHash;

	Runtime.DataAsset = TargetMesh;

	const FTransform& InstanceTransform =
		GetWorldSpaceTransformIncludingOffsets(Runtime);

	AddRenderInstance(PostReference, Runtime, CriticalSection, InstanceTransform);

	Runtime.LodIndex = INDEX_NONE;
	TObjectPtr<UTurboSequence_ThreadContext_Lf> ThreadContext = NewObject<UTurboSequence_ThreadContext_Lf>();
	UpdateCullingAndLevelOfDetail(Runtime, PostReference, Library.CameraViews,
	                              ThreadContext, false, Library);

	const FSkinnedMeshReferenceLodElement_Lf& LodElement = PostReference.LevelOfDetails[Runtime.LodIndex];
	UpdateRenderInstanceLod_Concurrent(PostReference, Runtime, LodElement,
	                                   Runtime.bIsVisible && LodElement.bIsRenderStateValid);

	TArray<FAnimationMetaData_Lf> Animations = Runtime.AnimationMetaData;
	const float BaseLayerWeight = Animations[GET0_NUMBER].FinalAnimationWeight;
	const float BaseLayerStartTime = Animations[GET0_NUMBER].AnimationWeightStartTime;
	Animations.RemoveAt(GET0_NUMBER);
	FTurboSequence_AnimPlaySettings_Lf PlaySettings = FTurboSequence_AnimPlaySettings_Lf();
	PlaySettings.ForceMode = ETurboSequence_AnimationForceMode_Lf::AllLayers;
	PlaySettings.RootMotionMode = ETurboSequence_RootMotionMode_Lf::None;

	bool bLoop = true;
	if (IsValid(TargetMesh->OverrideDefaultAnimation)) // Cause of rest pose will pass here
	{
		bLoop = TargetMesh->OverrideDefaultAnimation->bLoop;
	}

	ClearAnimations(CriticalSection, Runtime, Library, Library_RenderThread,
	                ETurboSequence_AnimationForceMode_Lf::AllLayers, PlaySettings.BoneLayerMasks,
	                [](const FAnimationMetaData_Lf& AnimationMeta)
	                {
		                return true;
	                });
	// Clear Animations Keeps the Base Layer so we remove it as well, to add it later back in
	RemoveAnimation(Runtime, CriticalSection, Library, Library_RenderThread, GET0_NUMBER);
	PlayAnimation(PostReference, Library, Library_RenderThread, Runtime, CriticalSection,
	              TargetMesh->OverrideDefaultAnimation, PlaySettings, bLoop, BaseLayerWeight, BaseLayerStartTime);

	for (const FAnimationMetaData_Lf& Animation : Animations)
	{
		PlaySettings = Animation.Settings;
		PlaySettings.AnimationPlayTimeInSeconds = Animation.AnimationTime;
		PlaySettings.StartTransitionTimeInSeconds = Animation.AnimationWeightStartTime;
		PlaySettings.EndTransitionTimeInSeconds = Animation.AnimationRemoveStartTime;

		PlayAnimation(PostReference, Library, Library_RenderThread, Runtime, CriticalSection, Animation.Animation,
		              PlaySettings, Animation.bIsLoop, Animation.FinalAnimationWeight, Animation.AnimationWeightTime,
		              Animation.AnimationRemoveTime);
	}
	Runtime.bForceVisibilityUpdatingThisFrame = true;

	const uint32 MeshID = Runtime.GetMeshID();
	ENQUEUE_RENDER_COMMAND(TurboSequence_CustomizeRenderInstance_Lf)(
		[&Library_RenderThread, MeshID, TargetMesh](FRHICommandListImmediate& RHICmdList)
		{
			if (Library_RenderThread.RuntimeSkinnedMeshes.Contains(MeshID))
			{
				FSkinnedMeshRuntime_RenderThread_Lf& Runtime_RenderThread = Library_RenderThread.RuntimeSkinnedMeshes[
					MeshID];

				Runtime_RenderThread.DataAsset = TargetMesh;
			}
		});
}
