// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_Test.h"

#include "HierarchicalStaticMeshSceneProxy.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "TurboSequence_Manager_Lf.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/InstancedStaticMesh.h"


// Sets default values
ATurboSequence_Test::ATurboSequence_Test()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Renderer = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>("Renderer");
}

// Called when the game starts or when spawned
void ATurboSequence_Test::BeginPlay()
{
	Super::BeginPlay();


	// const FSkeletalMeshLODRenderData& RenderData = Mesh->GetResourceForRendering()->LODRenderData[0];
	//
	// const int32 MaxNumVertices = RenderData.GetNumVertices();
	// constexpr int32 SkinStride = 6; // (12 Skin Indices + 12 Skin Weights ) / 4 = 6
	//
	// const FReferenceSkeleton& ReferenceSkeleton = Mesh->GetSkeleton()->GetReferenceSkeleton();
	//
	// //const int32 MaxNumBones = ReferenceSkeleton.GetNum();
	// constexpr int32 NumBoneStrideElements = 1;
	// constexpr int32 BoneStride = 3 * NumBoneStrideElements; // TRS //16; // Matrix4x4
	//
	// //const int32 NumBoneData = MaxNumBones * BoneStride;
	// const int32 NumSkinData = MaxNumVertices * SkinStride;
	// //constexpr int32 NumCustomData = 10;
	// //const int32 NumSettingsData = NumCustomData + MaxNumVertices;
	//
	// //Bones.AddZeroed(MaxNumBones * NumBoneStrideElements);
	// BoneWeights.AddZeroed(NumSkinData);
	// //SettingsData.AddZeroed(NumSettingsData);
	// //SettingsData[0] = FVector4f(NumCustomData, 0, 0, 0);
	//
	// Renderer->SetStaticMesh(RendererMesh);
	//
	// for (int32 i = 0; i < RendererMesh->GetNumSections(0); ++i)
	// {
	// 	Renderer->SetMaterial(i, Material);
	// }
	//
	// int32 AmountCharactersSqrt = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(AmountOfCharactersToSpawn)));
	// if (AmountOfCharactersToSpawn == 1)
	// {
	// 	AmountCharactersSqrt = 1;
	// }
	// CurrentCharacterCount_Internal = AmountCharactersSqrt * AmountCharactersSqrt;
	// const float Offset = AmountCharactersSqrt * DistanceBetweenCharacters * 0.5f;
	// LastSpawnLocation_Internal = FVector(Offset, Offset, 0);
	//
	// if (CurrentCharacterCount_Internal)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("Spawning -> %d Characters"), CurrentCharacterCount_Internal);
	// 	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurboSequence_Test::SpawnCharactersDelayed);
	//
	// 	GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, FString::Printf(TEXT("Spawning -> %d Characters"), CurrentCharacterCount_Internal));	
	// }
	//
	// TArray<uint32> Indices;
	// RenderData.MultiSizeIndexContainer.GetIndexBuffer(Indices);
	// const int32 NumIndices = Indices.Num();
	// const FSkinWeightVertexBuffer* SkinWeightBuffer = RenderData.GetSkinWeightVertexBuffer();
	// const int32 NumVertices = RenderData.GetNumVertices();
	// for (int32 i = GET0_NUMBER; i < NumVertices; ++i)
	// {
	// 	const int32 VertIdx = i;
	//
	// 	int32 VertexIndexSection;
	// 	int32 SectionIndex;
	// 	RenderData.GetSectionFromVertexIndex(VertIdx, SectionIndex, VertexIndexSection);
	//
	// 	const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];
	//
	//
	// 	for (uint16 InfluenceIdx = GET0_NUMBER; InfluenceIdx < MAX_TOTAL_INFLUENCES; ++InfluenceIdx)
	// 	{
	// 		const int32 BoneIndex = Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, InfluenceIdx)];
	// 		const uint8 BoneWeight = SkinWeightBuffer->GetBoneWeight(VertIdx, InfluenceIdx);
	//
	// 		if (BoneWeight)
	// 		{
	// 			if (!Bones.Contains(BoneIndex))
	// 			{
	// 				FBone Bone = FBone();
	// 				Bones.Add(BoneIndex, Bone);
	// 			}
	// 		}
	// 		
	// 	}
	// }

	// 	for (int32 i = GET0_NUMBER; i < NumVertices; ++i)
	// {
	// 	const int32 VertIdx = i;
	//
	// 	int32 VertexIndexSection;
	// 	int32 SectionIndex;
	// 	RenderData.GetSectionFromVertexIndex(VertIdx, SectionIndex, VertexIndexSection);
	//
	// 	const FSkelMeshRenderSection& Section = RenderData.RenderSections[SectionIndex];
	//
	// 	
	//
	// 	FVector4f Data1;
	// 	Data1.X = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 0)]);
	// 	Data1.Y = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 1)]);
	// 	Data1.Z = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 2)]);
	// 	Data1.W = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 3)]);
	// 	BoneWeights[VertIdx * SkinStride + 0] = Data1;
	// 	
	// 	FVector4f Data2;
	// 	Data2.X = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 4)]);
	// 	Data2.Y = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 5)]);
	// 	Data2.Z = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 6)]);
	// 	Data2.W = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 7)]);
	// 	BoneWeights[VertIdx * SkinStride + 1] = Data2;
	// 	
	// 	FVector4f Data3;
	// 	Data3.X = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 7)]);
	// 	Data3.Y = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 8)]);
	// 	Data3.Z = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 10)]);
	// 	Data3.W = BoneIndexToWeightIndex(Bones, Section.BoneMap[SkinWeightBuffer->GetBoneIndex(VertIdx, 11)]);
	// 	BoneWeights[VertIdx * SkinStride + 2] = Data3;
	// 	
	// 	
	// 	FVector4f Data4;
	// 	Data4.X = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 0), 0, 0xFF);
	// 	Data4.Y = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 1), 0, 0xFF);
	// 	Data4.Z = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 2), 0, 0xFF);
	// 	Data4.W = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 3), 0, 0xFF);
	// 	BoneWeights[VertIdx * SkinStride + 3] = Data4;
	// 	
	// 	FVector4f Data5;
	// 	Data5.X = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 4), 0, 0xFF);
	// 	Data5.Y = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 5), 0, 0xFF);
	// 	Data5.Z = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 6), 0, 0xFF);
	// 	Data5.W = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 7), 0, 0xFF);
	// 	BoneWeights[VertIdx * SkinStride + 4] = Data5;
	// 	
	// 	FVector4f Data6;
	// 	Data6.X = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 8), 0, 0xFF);
	// 	Data6.Y = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 9), 0, 0xFF);
	// 	Data6.Z = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 10), 0, 0xFF);
	// 	Data6.W = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(SkinWeightBuffer->GetBoneWeight(VertIdx, 11), 0, 0xFF);
	// 	BoneWeights[VertIdx * SkinStride + 5] = Data6;
	// }
	//const TArray<FTransform>& RefPose = Mesh->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose();

	// for (const FSkelMeshRenderSection& Section : RenderData.RenderSections)
	// {
	// 	const int32 VertexBase = Section.BaseVertexIndex;
	// 	const int32 MaxNumVerticesPerSection = VertexBase + Section.NumVertices;
	// 	//const auto& BoneMap = Section.BoneMap;
	//
	// 	for (int32 VertIdx = VertexBase; VertIdx < MaxNumVerticesPerSection; ++VertIdx)
	// 	{
	// 		FVector4f Data;
	// 		
	// 		const int32 BoneIndex1 = SkinWeightBuffer->GetBoneIndex(VertIdx, 0);
	// 		const uint8 BoneWeight1 = SkinWeightBuffer->GetBoneWeight(VertIdx, 0);
	//
	// 		Data.X = Section.BoneMap[BoneIndex1];
	// 		Data.Y = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(BoneWeight1, 0, 0xFF);
	//
	// 		const int32 BoneIndex2 = SkinWeightBuffer->GetBoneIndex(VertIdx, 1);
	// 		const uint8 BoneWeight2 = SkinWeightBuffer->GetBoneWeight(VertIdx, 1);
	//
	// 		Data.Z = Section.BoneMap[BoneIndex2];
	// 		Data.W = FTurboSequence_Helper_Lf::GetPercentageBetweenMinMax(BoneWeight2, 0, 0xFF);
	//
	// 		BoneWeights[VertIdx] = Data;
	// 	}
	// }

	// if (Asset->SkinWeightTexture)
	// {
	// 	//TArray<FVector4f> DataArray;
	// 	FSettingsComputeShader_Params_Lf Params;
	// 	Params.InputData = BoneWeights;
	// 	Params.AnimationOutputTexture = Asset->SkinWeightTexture;
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("%s"), *DataArray[0].ToString());
	//
	// 	FSettingsCompute_Shader_Execute_Lf::Dispatch(Params, [&](FVector4f DebugValue)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugValue.ToString());
	//
	//
	// 		// for (int i = 0; i < 4; ++i)
	// 		// {
	// 		// 	UE_LOG(LogTemp, Warning, TEXT("Bake Shader -> Debug Output index -> %d, %d"), i, DebugValue[i]);	
	// 		// }
	//
	// 		//BoneWeights.Empty();
	// 	});
	// }


	// int32 VertexPositionOffsetIndex = SettingsData[0].X;
	// for (int32 i = 0; i < MaxNumVertices; ++i)
	// {
	// 	int32 RuntimeIndex = i;
	// 	FTransform RuntimeTransform = FTransform::Identity;
	// 	while (RuntimeIndex != INDEX_NONE)
	// 	{
	// 		RuntimeTransform *= RefPose[RuntimeIndex];
	//
	// 		RuntimeIndex = ReferenceSkeleton.GetParentIndex(RuntimeIndex);
	// 	}
	// 	FTransform BodyTransform;
	// 	Renderer->GetInstanceTransform(0, BodyTransform);
	// 	BodyTransform.SetLocation(FVector::ZeroVector);
	// 	RuntimeTransform *= BodyTransform;
	//
	// 	const FVector Location = RuntimeTransform.GetLocation();
	// 	const FQuat Rotation = RuntimeTransform.GetRotation();
	// 	const FVector Scale = RuntimeTransform.GetScale3D();
	//
	// 	FVector4f Data1;
	//
	// 	Data1.X = Location.X;
	// 	Data1.Y = Location.Y;
	// 	Data1.Z = Location.Z;
	// 	Data1.W = 0;
	//
	//
	// 	SettingsData[VertexPositionOffsetIndex + 0] = Data1;
	//
	// 	FVector4f Data2;
	//
	// 	Data2.X = Rotation.X;
	// 	Data2.Y = Rotation.Y;
	// 	Data2.Z = Rotation.Z;
	// 	Data2.W = Rotation.W;
	//
	//
	// 	SettingsData[VertexPositionOffsetIndex + 1] = Data2;
	//
	// 	FVector4f Data3;
	//
	// 	Data3.X = Scale.X;
	// 	Data3.Y = Scale.Y;
	// 	Data3.Z = Scale.Z;
	// 	Data3.W = 0;
	//
	//
	// 	SettingsData[VertexPositionOffsetIndex + 2] = Data3;
	//
	// 	VertexPositionOffsetIndex += 3;
	// }
	//
	// if (Asset->AnimationFramesTexture)
	// {
	// 	//TArray<FVector4f> DataArray;
	// 	FSettingsComputeShader_Params_Lf Params;
	// 	Params.InputData = SettingsData;
	// 	Params.AnimationOutputTexture = Asset->AnimationFramesTexture;
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("%s"), *DataArray[0].ToString());
	//
	// 	FSettingsCompute_Shader_Execute_Lf::Dispatch(Params, [](FVector4f DebugValue)
	// 	{
	// 		//UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugValue.ToString());
	//
	//
	// 		// for (int i = 0; i < 4; ++i)
	// 		// {
	// 		// 	UE_LOG(LogTemp, Warning, TEXT("Bake Shader -> Debug Output index -> %d, %d"), i, DebugValue[i]);	
	// 		// }
	// 	});	
	// }
}

// Called every frame
void ATurboSequence_Test::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//AnimateTestMesh(Poser, Animation, AnimationTime, DeltaTime);

	// if (!BonesMatrixArray.Num() && ATurboSequence_Manager_Lf::SkinnedMeshReferences.Contains(Asset))
	// {
	// 	BonesMatrixArray.AddZeroed(ATurboSequence_Manager_Lf::SkinnedMeshReferences[Asset].CPUBoneToGPUBoneIndicesMap.Num() * 3 + 2); // 2 for the settings
	//
	// 	for (const TTuple<uint16, uint16>& IndicesMap : ATurboSequence_Manager_Lf::SkinnedMeshReferences[Asset].CPUBoneToGPUBoneIndicesMap)
	// 	{
	// 		Bones.Add(IndicesMap.Key, FBoneData123());
	// 	}
	//
	// 	RestPoseBonesMatrixArray.AddZeroed(ATurboSequence_Manager_Lf::SkinnedMeshReferences[Asset].CPUBoneToGPUBoneIndicesMap.Num() * 3 + 2); // 2 for the settings
	// }

	//Renderer->UpdateInstanceTransform(0, Asset->TestTransform);

	// AnimationTime += DeltaTime;
	// AnimationTime = FMath::Fmod(AnimationTime, Animation->GetPlayLength());
	//
	// const FReferenceSkeleton& ReferenceSkeleton = Mesh->GetSkeleton()->GetReferenceSkeleton();
	// const TArray<FTransform>& RefPose = Mesh->GetSkeleton()->GetReferenceSkeleton().GetRefBonePose();
	//
	// //const int32 MaxNumBones = ReferenceSkeleton.GetNum();
	//
	// if (Bones.Num())
	// {
	// 	for (TTuple<uint16, FBoneData123>& Bone : Bones)
	// 	{
	// 		int32 RuntimeIndex = Bone.Key;
	// 		FTransform RuntimeTransform = FTransform::Identity;
	// 		FTransform RefPoseTransform = FTransform::Identity;
	// 		while (RuntimeIndex != INDEX_NONE)
	// 		{
	// 			FTransform BoneTransform;
	// 			Animation->GetBoneTransform(BoneTransform, RuntimeIndex, AnimationTime, true);
	// 			if (RuntimeIndex)
	// 			{
	// 				RuntimeTransform *= BoneTransform;
	// 			}
	// 			else
	// 			{
	// 				RuntimeTransform *= RefPose[RuntimeIndex];
	// 			}
	// 			RefPoseTransform *= RefPose[RuntimeIndex];
	//
	// 			//RuntimeTransform *= BoneTransform * RefPose[RuntimeIndex].Inverse();
	//
	// 			RuntimeIndex = ReferenceSkeleton.GetParentIndex(RuntimeIndex);
	// 		}
	//
	// 		//Animation->GetBoneTransform(RuntimeTransform1, i, AnimationTime, false);
	// 		//FTransform InverseTransposeTransform = RefPoseTransform.Inverse() * RuntimeTransform;
	//
	// 		// if (i == 2)
	// 		// {
	// 		// 	FVector Scale = RuntimeTransform.GetScale3D();
	// 		// 	Scale = FVector::OneVector * AnimationTime;
	// 		// 	RuntimeTransform.SetScale3D(Scale);
	// 		// }
	// 		// FTransform InstanceTransform;
	// 		// Renderer->GetInstanceTransform(0, InstanceTransform, true);
	// 		// FTransform RootTransform = InstanceTransform;
	// 		// RootTransform.SetLocation(FVector::ZeroVector);
	// 		// RuntimeTransform *= RootTransform;
	//
	// 		//RuntimeTransform *= FTransform::Identity;
	//
	// 		FBoneData123 Data;
	// 		Data.AnimatedBoneMatrix = RuntimeTransform;
	// 		Data.InverseRestPoseBoneMatrix = RefPoseTransform.Inverse();
	//
	// 		Bone.Value = Data;//InverseTransposeTransform;
	// 	}
	//
	// 	//const FSkeletalMeshLODRenderData& RenderData = Mesh->GetResourceForRendering()->LODRenderData[0];
	//
	// 	//const int32 MaxNumVertices = RenderData.GetNumVertices();
	// 	//constexpr int32 SkinStride = 3;
	// 	if (RendererMesh)
	// 	{
	// 		const int32 UVCoord = RendererMesh->GetNumTexCoords(GET0_NUMBER) - GET1_NUMBER;
	// 		BonesMatrixArray[GET0_NUMBER] = FVector4f(UVCoord, GET0_NUMBER,GET0_NUMBER,GET0_NUMBER);
	// 		BonesMatrixArray[GET1_NUMBER].X = Asset->SkinWeightTexture->SizeX;
	// 		BonesMatrixArray[GET1_NUMBER].Y = Asset->SkinWeightTexture->SizeY;
	// 		BonesMatrixArray[GET1_NUMBER].Z = Asset->TransformTexture_CurrentFrame->SizeX;
	// 		BonesMatrixArray[GET1_NUMBER].W = Asset->TransformTexture_CurrentFrame->SizeY;
	// 	}
	//
	// 	int32 CustomDataIndex = 2;
	// 	//const int32 NumBones = Bones.Num();
	// 	for (const TTuple<uint16, FBoneData123>& Bone : Bones)
	// 	{
	// 		FMatrix Matrix = Bone.Value.AnimatedBoneMatrix.ToMatrixWithScale();
	// 		FMatrix InvMatrix = Bone.Value.InverseRestPoseBoneMatrix.ToMatrixWithScale();
	// 		for (int32 M = 0; M < 3; ++M)
	// 		{
	// 			FVector4f Data;
	// 			Data.X = Matrix.M[0][M];
	// 			Data.Y = Matrix.M[1][M];
	// 			Data.Z = Matrix.M[2][M];
	// 			Data.W = Matrix.M[3][M];
	// 			BonesMatrixArray[CustomDataIndex + M] = Data;
	//
	// 			FVector4f Data1;
	// 			Data1.X = InvMatrix.M[0][M];
	// 			Data1.Y = InvMatrix.M[1][M];
	// 			Data1.Z = InvMatrix.M[2][M];
	// 			Data1.W = InvMatrix.M[3][M];
	// 			RestPoseBonesMatrixArray[CustomDataIndex + M] = Data1;
	// 		}
	//
	// 		CustomDataIndex += 3;
	// 	}
	// }

	// if (Asset->SkinWeightTexture && BoneWeights.Num())
	// {
	// 	//TArray<FVector4f> DataArray;
	// 	FSettingsComputeShader_Params_Lf Params;
	// 	Params.InputData = BoneWeights;
	// 	Params.AnimationOutputTexture = Asset->SkinWeightTexture;
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("%s"), *DataArray[0].ToString());
	//
	// 	FSettingsCompute_Shader_Execute_Lf::Dispatch(Params, [&](FVector4f DebugValue)
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugValue.ToString());
	//
	//
	// 		// for (int i = 0; i < 4; ++i)
	// 		// {
	// 		// 	UE_LOG(LogTemp, Warning, TEXT("Bake Shader -> Debug Output index -> %d, %d"), i, DebugValue[i]);	
	// 		// }
	//
	// 		BoneWeights.Empty();
	// 	});
	// }

	// if (Asset->TransformTexture_CurrentFrame && BonesMatrixArray.Num())
	// {
	// 	FMeshUnitComputeShader_Params_Lf Params;
	// 	Params.BoneMatricesInput = BonesMatrixArray;
	// 	Params.AnimationOutputTexture = Asset->TransformTexture_CurrentFrame;
	// 	Params.NumShaders = 150;
	// 	Params.NumMeshesPerThread = 2;
	// 	Params.NumMeshes = 1;
	// 	Params.CPUInverseReferencePose = RestPoseBonesMatrixArray;
	// 	Params.NumGPUBonesPixelOffset = BonesMatrixArray.Num();
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("%d"), BonesMatrixArray.Num());
	//
	// 	//UE_LOG(LogTemp, Warning, TEXT("%s"), *DataArray[0].ToString());
	//
	// 	FMeshUnit_Compute_Shader_Execute_Lf::Dispatch(Params, [](FVector4f DebugValue)
	// 	{
	// 		//UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugValue.ToString());
	// 	
	// 	
	// 		// for (int i = 0; i < 4; ++i)
	// 		// {
	// 		// 	UE_LOG(LogTemp, Warning, TEXT("Bake Shader -> Debug Output index -> %d, %d"), i, DebugValue[i]);	
	// 		// }
	// 	});
	//}

	//Renderer->SetForcedLodModel(2);

	// const TArray<FClusterNode>& ClusterTree = *Renderer->ClusterTreePtr;
	// for (const FClusterNode& ChildNode : ClusterTree)
	// {
	// 	for (int32 i = ChildNode.FirstInstance; i <= ChildNode.LastInstance; ++i)
	// 	{
	// 		int32 SortedIdx = Renderer->SortedInstances[i];
	//
	// 		FMatrix WorldToLocal = Renderer->SceneProxy->GetLocalToWorld().Inverse();
	// 		FVector ViewOriginInLocalZero = WorldToLocal.TransformPosition(View->GetTemporalLODOrigin(0, bMultipleSections));
	// 		FVector ViewOriginInLocalOne  = WorldToLocal.TransformPosition(View->GetTemporalLODOrigin(1, bMultipleSections));
	// 	}	
	// }

	// auto SceneProxy = Cast<FHierarchicalStaticMeshSceneProxy>(Renderer->SceneProxy);
	//
	// for (const FInstancedStaticMeshInstanceData& Data : Renderer->perins)
	// {
	// 	Data.
	// }

	//Renderer->MarkRenderStateDirty();
	//Renderer->SetCustomData(0, Renderer->PerInstanceSMCustomData, true);
}

// void ATurboSequence_Test::SpawnCharactersDelayed()
// {
// 	if (CurrentCharacterCount_Internal > 0)
// 	{
// 		LastSpawnLocation_Internal += FVector(-DistanceBetweenCharacters, 0, 0);
//
// 		int32 AmountCharactersSqrt = FMath::RoundToInt(FMath::Sqrt(static_cast<float>(AmountOfCharactersToSpawn)));
// 		if (AmountOfCharactersToSpawn == 1)
// 		{
// 			AmountCharactersSqrt = 1;
// 		}
//
// 		for (int32 Y = 0; Y < AmountCharactersSqrt; ++Y)
// 		{
// 			FVector Location = FVector(LastSpawnLocation_Internal.X, Y * -DistanceBetweenCharacters + LastSpawnLocation_Internal.Y, 0);
// 			CurrentCharacterCount_Internal--;
//
// 			FTransform Transform = FTransform(Location);
//
// 			Renderer->AddInstance(Transform, true);
// 		}
//
// 		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurboSequence_Test::SpawnCharactersDelayed);
// 	}
// }
