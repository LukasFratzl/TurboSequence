// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphEvent.h"
#include "ShaderParameterStruct.h"
#include "TurboSequence_Helper_Lf.h"
#include "Engine/TextureRenderTarget2DArray.h"


struct TURBOSEQUENCE_SHADER_LF_API FMeshUnitComputeShader_Params_Lf
{
	// ID for memory management
	uint32 ShaderID;

	double GetNumMB()
	{
		int64 Bytes = 0;
		int32 Num;

		Num = BoneSpaceAnimationIKIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = PerMeshCustomDataIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = ReferenceNumCPUBones.Num();
		Bytes += Num * 2;

		//Num = AnimationRawData_RenderThread.Num();
		//Bytes += Num * 2 * 4;

		Bytes += 4;
		Bytes += 4;
		Bytes += 4;

		Num = PerMeshCustomDataIndex_Global_RenderThread.Num();
		Bytes += Num * 4;

		Num = PerMeshCustomDataLod_RenderThread.Num();
		Bytes += Num * 2;

		Num = PerMeshCustomDataCollectionIndex_RenderThread.Num();
		Bytes += Num * 2;

		Num = ReferenceNumCPUBones_RenderThread.Num();
		Bytes += Num * 2;

		Num = AnimationStartIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = AnimationEndIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = AnimationFramePose0_RenderThread.Num();
		Bytes += Num * 4;

		Num = AnimationWeights_RenderThread.Num();
		Bytes += Num * 2;

		Num = AnimationLayerIndex_RenderThread.Num();
		Bytes += Num * 2;

		Num = AnimationLayers_RenderThread.Num();
		Bytes += Num * 2;

		Num = BoneSpaceAnimationIKStartIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = BoneSpaceAnimationIKEndIndex_RenderThread.Num();
		Bytes += Num * 4;

		Num = BoneSpaceAnimationIKInput_RenderThread.Num();
		Bytes += Num * 2 * 4;

		Num = BoneSpaceAnimationIKData_RenderThread.Num();
		Bytes += Num * 4;

		Num = CPUInverseReferencePose.Num();
		Bytes += Num * 2 * 4;

		Num = Indices.Num();
		Bytes += Num * 2 * 4;

		Bytes += 4;
		Bytes += 4;
		Bytes += 4;


		double BytesDouble = (double)Bytes;

		return BytesDouble / 1000.0 / 1000.0;
	}

	// This is for the CPU Memory
	//TArray<FVector4f> BoneSpaceAnimationIKInput;
	// TArray<int32> BoneSpaceAnimationIKData;
	TArray<int32> BoneSpaceAnimationIKIndex_RenderThread;
	// //TArray<int32> BoneSpaceAnimationIKGlobalIndex_RenderThread;
	TArray<int32> PerMeshCustomDataIndex_RenderThread;
	// TArray<int32> PerMeshCustomDataLod;
	// TArray<int32> PerMeshCustomDataCollectionIndex;
	TArray<int32> ReferenceNumCPUBones;
	// TArray<int32> AnimationFrameNormalizedTime;
	// TArray<int32> AnimationWeights;
	// TArray<int32> AnimationID;

	//TArray<int32> AnimationFrameMaxFrames;
	//TArray<int32> AnimationOffset;

	//TArray<FVector4f> AnimationRawData_RenderThread;

	// Minimals getting uploaded to the GPU
	int32 NumMeshes;
	int32 NumAnimations;
	//int32 MaxNumMeshesRenderThread;
	//float MaxNumMeshesRefreshTimer;
	//float DeltaTime;
	int32 NumIKData;
	//int32 MaxNumIKDataRenderThread;
	float MaxNumIKDataRefreshTimer;
	TArray<int32> PerMeshCustomDataIndex_Global_RenderThread;
	//TArray<int32> PerMeshCustomDataIndex_Minimal_RenderThread;
	TArray<int32> PerMeshCustomDataLod_RenderThread;
	TArray<int32> PerMeshCustomDataCollectionIndex_RenderThread;
	TArray<int32> ReferenceNumCPUBones_RenderThread;
	//TArray<FVector4f> PerMeshCustomData_RenderThread;
	//TArray<int32> SkinWeightVertexLod_RenderThread;
	//TArray<int32> SkinWeightOffsetLod_RenderThread;

	TArray<int32> AnimationStartIndex_RenderThread;
	TArray<int32> AnimationEndIndex_RenderThread;
	//TArray<int32> AnimationFrameAlpha_RenderThread;
	TArray<int32> AnimationFramePose0_RenderThread;
	//TArray<int32> AnimationFramePose1_RenderThread;

	TArray<int32> AnimationWeights_RenderThread;
	//TArray<int32> PerBoneAnimationWeights_RenderThread;
	//TArray<int32> PerBoneAnimationWeightIndices_RenderThread;

	//TArray<int32> AnimationLocationWeights_RenderThread;
	//TArray<int32> AnimationRotationWeights_RenderThread;
	//TArray<int32> AnimationScaleWeights_RenderThread;

	TArray<int32> AnimationLayerIndex_RenderThread;

	// < GPU Animation Layer Index | Skeleton Layer <- Fixed Size of MaxNumCPUBones >
	// Value -> Bool bIsValidAnimationBone
	TArray<int32> AnimationLayers_RenderThread;

	TArray<int32> BoneSpaceAnimationIKStartIndex_RenderThread;
	TArray<int32> BoneSpaceAnimationIKEndIndex_RenderThread;
	TArray<FVector4f> BoneSpaceAnimationIKInput_RenderThread;
	TArray<int32> BoneSpaceAnimationIKData_RenderThread;

	// Const Getting uploaded as well
	TArray<FVector4f> CPUInverseReferencePose;
	//TArray<float> CPUParentIndices;
	//TArray<float> CPUIndices;
	//TArray<float> GPUIndices;
	TArray<FVector4f> Indices;
	//int32 UVCoordIndex;
	//FIntVector2 SkinWeightTextureDimensions;
	//FIntVector2 AnimationTextureDimensions;
	//int32 NumGPUBonesPixelOffset;
	int32 NumMaxCPUBones;
	int32 NumMaxGPUBones;
	int32 NumMaxLevelOfDetails;

	//int32 AnimationLibraryMaxNum_Previous;
	//int32 AnimationLibraryMaxNum_Current;

	bool bUse32BitTransformTexture;

	// Debug
	int32 NumDebugData;

	TObjectPtr<UTextureRenderTarget2DArray> AnimationLibraryTexture;

	//bool bNeedSolveThisFrame;

	//uint32 TestIteration;
	//uint32 MaxFrames = GET32_NUMBER;
};

struct TURBOSEQUENCE_SHADER_LF_API FSettingsComputeShader_Params_Lf
{
	uint32 ShaderID;

	bool bUse32BitTexture;

	TArray<FVector4f> SettingsInput;
	//TArray<FVector4f> SettingsInput_Copy;

	bool bIsAdditiveWrite;

	uint32 AdditiveWriteBaseIndex = GET0_NUMBER;
	//uint32 NumAdditiveWrite_Previous = GET0_NUMBER;

	//FIntVector3 TargetDimensions;
};

// struct TURBOSEQUENCE_SHADER_LF_API FRenderDataComputeShader_Params_Lf
// {
// 	int32 TextureStride;
// 	TMap<uint8, TArray<FVector4f>> InputData;
// 	TArray<FIntVector2> CleanUpData;
// 	TArray<FVector4f> InputData_RenderThread;
// };

class TURBOSEQUENCE_SHADER_LF_API FTurboSequence_BoneTransform_CS_Lf : public FGlobalShader
{
public:
	inline static FIntVector3 NumThreads = FIntVector3(GET32_NUMBER, GET32_NUMBER, GET1_NUMBER);

	inline static const FString GraphName = TEXT("TurboSequence_MeshUnit_ComputeShaderExecute_{0}");
	inline static const FString DebugName = TEXT("TurboSequence_MeshUnit_Debug_{0}");
	inline static const FString BoneTransformsTextureDebugName = TEXT("TurboSequence_Write_Texture_BoneTransform_Library_{0}");
	inline static const FString AnimationLibraryTextureDebugName = TEXT("TurboSequence_Write_Texture_Animation_Library_{0}");
	inline static const FString DataTextureDebugName = TEXT("TurboSequence_Write_Texture_CustomData_Library_{0}");
	inline static const FString AnimationLibraryBufferDebugName = TEXT("TurboSequence_AnimationLibrary_Buffer_{0}");
	inline static const FString RefPoseDebugName = TEXT("TurboSequence_ReferencePose_Input_{0}");
	inline static const FString RefPoseCPUIndicesDebugName = TEXT("TurboSequence_ReferencePoseCPUIndices_{0}");
	inline static const FString RefPoseGPUIndicesDebugName = TEXT("TurboSequence_ReferencePoseGPUIndices_{0}");
	inline static const FString RefPoseParentIndicesDebugName = TEXT("TurboSequence_ReferencePoseCPU_ParentIndices_{0}");
	inline static const FString CustomDataIndicesDebugName = TEXT("TurboSequence_PerMeshCustomDataIndices_{0}");
	//inline static const FString CustomDataDebugName = TEXT("TurboSequence_PerMeshCustomData_{0}");
	inline static const FString CustomDataLodDebugName = TEXT("TurboSequence_PerMeshCustomDataLod_{0}");
	inline static const FString SkinWeightOffsetLodDebugName = TEXT("TurboSequence_SkinWeightOffsetLod_{0}");
	//inline static const FString SkinWeightVertexLodDebugName = TEXT("TurboSequence_SkinWeightVertexLod_{0}");
	//inline static const FString AnimationFrameNormalizedTimeDebugName = TEXT("TurboSequence_AnimationNormalizedTime_{0}");
	inline static const FString AnimationWeightsDebugName = TEXT("TurboSequence_AnimationWeights_{0}");
	inline static const FString PerBoneAnimationWeightsDebugName = TEXT("TurboSequence_PerBoneAnimationWeights_{0}");
	inline static const FString PerBoneAnimationWeightIndicesDebugName = TEXT("TurboSequence_PerBoneAnimationWeightIndices_{0}");
	inline static const FString AnimationLocationWeightsDebugName = TEXT("TurboSequence_AnimationWeights_{0}");
	inline static const FString AnimationRotationWeightsDebugName = TEXT("TurboSequence_AnimationWeights_{0}");
	inline static const FString AnimationScaleWeightsDebugName = TEXT("TurboSequence_AnimationWeights_{0}");

	inline static const FString AnimationFrameAlphaDebugName = TEXT("TurboSequence_AnimationFrameAlpha_{0}");
	inline static const FString AnimationFramePose0DebugName = TEXT("TurboSequence_AnimationFramePose0_{0}");
	inline static const FString AnimationFramePose1DebugName = TEXT("TurboSequence_AnimationFramePose1_{0}");

	inline static const FString AnimationStartIndexDebugName = TEXT("TurboSequence_AnimationStartIndex_{0}");
	inline static const FString AnimationEndIndexDebugName = TEXT("TurboSequence_AnimationEndIndex_{0}");
	inline static const FString AnimationIDsDebugName = TEXT("TurboSequence_AnimationIDs_{0}");
	inline static const FString AnimationRootIndexDebugName = TEXT("TurboSequence_AnimationRootIndices_{0}");
	inline static const FString AnimationLayersDebugName = TEXT("TurboSequence_AnimationLayers_{0}");
	inline static const FString AnimationFrameMaxFramesDebugName = TEXT("TurboSequence_AnimationNextFrames_{0}");
	inline static const FString AnimationOffsetDebugName = TEXT("TurboSequence_AnimationOffset_{0}");
	inline static const FString BoneTransformsTextureCopyDebugName = TEXT("TurboSequence_Write_Texture_BoneTransform_Library_Copy_{0}");
	inline static const FString CustomDataTextureCopyDebugName = TEXT("TurboSequence_Write_Texture_CustomLibrary_Library_Copy_{0}");
	inline static const FString BoneSpaceAnimationIKInputDebugName = TEXT("TurboSequence_IK_Bones_Input_{0}");
	inline static const FString BoneSpaceAnimationIKDataInputDebugName = TEXT("TurboSequence_IK_Data_Bones_Input_{0}");
	inline static const FString BoneSpaceAnimationIKDataStartIndexInputDebugName = TEXT("TurboSequence_IK_Data_Bones_Input_StartIndex_{0}");
	inline static const FString BoneSpaceAnimationIKDataEndIndexInputDebugName = TEXT("TurboSequence_IK_Data_Bones_Input_EndIndex_{0}");

	DECLARE_GLOBAL_SHADER(FTurboSequence_BoneTransform_CS_Lf);
	SHADER_USE_PARAMETER_STRUCT(FTurboSequence_BoneTransform_CS_Lf, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER(int, NumMeshesPerThread)
		SHADER_PARAMETER(int, NumFirstGPUBones)

		SHADER_PARAMETER(int, NumCPUBones)
		SHADER_PARAMETER(int, NumMeshesPerFrame)

		//SHADER_PARAMETER(int, AnimationLibraryIndex)
		//SHADER_PARAMETER(int, NumAnimationLibraryWrite)

		//SHADER_PARAMETER(int, DataTextureSizeX)
		//SHADER_PARAMETER(int, DataTextureSizeY)
		SHADER_PARAMETER(int, AnimTextureSizeX)
		SHADER_PARAMETER(int, AnimTextureSizeY)

		SHADER_PARAMETER(int, OutputTextureSizeX)
		SHADER_PARAMETER(int, OutputTextureSizeY)

		SHADER_PARAMETER(int, NumDebugBuffer)
		SHADER_PARAMETER(int, NumLevelOfDetails)

		SHADER_PARAMETER(int, NumCustomStates)
		SHADER_PARAMETER(int, NumPixelBuffer)

		//SHADER_PARAMETER(int, NumCustomDataPixels)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, ReferencePose_StructuredBuffer)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, ReferencePoseParentIndices_StructuredBuffer)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, ReferencePoseCPUIndices_StructuredBuffer)
		// SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, ReferencePoseGPUIndices_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, ReferencePoseIndices_StructuredBuffer)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, BoneSpaceAnimationInput_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, BoneSpaceAnimationDataInput_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, BoneSpaceAnimationDataStartIndex_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, BoneSpaceAnimationDataEndIndex_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, PerMeshCustomDataIndices_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, PerMeshCustomDataLod_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, PerMeshCustomDataCollectionIndex_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, ReferenceNumCPUBones_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, SkinWeightVertexLod_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, SkinWeightOffsetLod_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, PerMeshCustomData_StructuredBuffer)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationStartIndex_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationEndIndex_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationFrameOffset_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationMaxFrames_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, AnimationFrameAlpha_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, AnimationWeight_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, PerBoneAnimationWeight_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, PerBoneAnimationWeightIndex_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, AnimationLocationWeight_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, AnimationRotationWeight_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16int>, AnimationScaleWeight_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationFramePose0_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<int>, AnimationFramePose1_StructuredBuffer)
		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, AnimationID_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, AnimationLayerIndex_StructuredBuffer)

		//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float4>, AnimationLibrary_StructuredBuffer)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<min16uint>, AnimationLayerLibrary_StructuredBuffer)

		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2DArray<float4>, R_AnimationLibrary_InputTexture)
		//
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2DArray<float4>, RW_AnimationInput_OutputTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2DArray<float4>, RW_BoneTransform_OutputTexture)
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2DArray<float4>, RW_CustomData_OutputTexture)

		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, DebugValue)


	END_SHADER_PARAMETER_STRUCT()


	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		OutEnvironment.SetDefine(TEXT("THREADS_X"), NumThreads.X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NumThreads.Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), NumThreads.Z);
	}
};

class TURBOSEQUENCE_SHADER_LF_API FTurboSequence_Settings_CS_Lf : public FGlobalShader
{
public:
	inline static FIntVector2 NumThreads = FIntVector2(GET32_NUMBER, GET32_NUMBER);

	inline static const FString GraphName = TEXT("TurboSequence_Settings_ComputeShaderExecute_{0}");
	inline static const FString DebugName = TEXT("TurboSequence_Settings_Debug_{0}");
	inline static const FString WriteTextureDebugName = TEXT("TurboSequence_Settings_WriteTexture_{0}");
	inline static const FString DataInputDebugName = TEXT("TurboSequence_Settings_DataInput_{0}");

	DECLARE_GLOBAL_SHADER(FTurboSequence_Settings_CS_Lf);
	SHADER_USE_PARAMETER_STRUCT(FTurboSequence_Settings_CS_Lf, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER(uint32, TextureDimensionX)
		SHADER_PARAMETER(uint32, TextureDimensionY)
		SHADER_PARAMETER(uint32, NumPixelPerThread)
		SHADER_PARAMETER(uint32, BaseIndex)

		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FVector4f>, BoneWeights_StructuredBuffer)

		//SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, DebugValue)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2DArray<FVector4f>, RW_Settings_OutputTexture)
	END_SHADER_PARAMETER_STRUCT()


	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		return true;
	}

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

		const FPermutationDomain PermutationVector(Parameters.PermutationId);

		OutEnvironment.SetDefine(TEXT("THREADS_X"), NumThreads.X);
		OutEnvironment.SetDefine(TEXT("THREADS_Y"), NumThreads.Y);
		OutEnvironment.SetDefine(TEXT("THREADS_Z"), GET1_NUMBER);
	}
};


class TURBOSEQUENCE_SHADER_LF_API FTurboSequence_ClearRenderTarget_CS_Lf
{
public:
	//SHADER_USE_PARAMETER_STRUCT(FTurboSequence_ClearRenderTarget_CS_Lf, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters,)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2DArray<float4>, RW_OutputTexture)
	END_SHADER_PARAMETER_STRUCT()


	inline static const FString GraphName = TEXT("TurboSequence_ClearRenderTarget");
	inline static const FString TexName = TEXT("TurboSequence_ClearRenderTarget_Texture");
	inline static const FString RenderTexName = TEXT("TurboSequence_ClearRenderTarget_RenderTexture");

	static void ClearRenderTarget_GameThread(const TObjectPtr<UTextureRenderTarget2DArray> Texture, const EPixelFormat& TextureFormat)
	{
		ENQUEUE_RENDER_COMMAND(TurboSequence_ClearRenderTarget_Lf)(
			[Texture, TextureFormat](FRHICommandListImmediate& RHICmdList)
			{
				ClearRenderTarget_RenderThread(RHICmdList, Texture, TextureFormat);
			});
	}

	static void ClearRenderTarget_RenderThread(FRHICommandListImmediate& RHICmdList, const TObjectPtr<UTextureRenderTarget2DArray> Texture, const EPixelFormat& TextureFormat)
	{
		FRDGBuilder GraphBuilder(RHICmdList, FRDGEventName(*GraphName), ERDGBuilderFlags::AllowParallelExecute);

		FParameters* Parameters = GraphBuilder.AllocParameters<FParameters>();

		FRDGTextureRef DataOutputTextureRef;
		Parameters->RW_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, DataOutputTextureRef, Texture->SizeX, Texture->SizeY, Texture->Slices, *TexName, TextureFormat);

		GraphBuilder.AddPass(
			RDG_EVENT_NAME("TurboSequence_Execute_Clearing_RenderTarget"),
			Parameters,
			ERDGPassFlags::Compute,
			[&Parameters](FRHIComputeCommandList& RHICommandList)
			{
				RHICommandList.ClearUAVFloat(Parameters->RW_OutputTexture->GetRHI(), FLinearColor::Transparent);
			});

		const FRDGTextureRef OutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(Texture->GetRenderTargetResource()->GetTexture2DArrayRHI(), *RenderTexName));

		FRHICopyTextureInfo CopyInfo = FRHICopyTextureInfo();
		CopyInfo.NumSlices = Texture->Slices;

		AddCopyTexturePass(GraphBuilder, DataOutputTextureRef, OutputTexture, CopyInfo);

		// Execute the graph
		GraphBuilder.Execute();
	}
};


class TURBOSEQUENCE_SHADER_LF_API FMeshUnit_Compute_Shader_Execute_Lf
{
	inline static TArray<float> DebugData;

public:
	/**
	 * @brief RUNS THE SHADER ON THE RENDER THREAD
	 * @param RHICmdList THE RENDER COMMAND LIST FROM ENQUEUE_RENDER_COMMAND
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
	 */
	static void DispatchRenderThread(
		const TFunction<void(FRHICommandListImmediate& RHICmdList)>& PreCall,
		FRHICommandListImmediate& RHICmdList,
		FMeshUnitComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* AnimationOutputTexture, /*UTextureRenderTarget2DArray* DataOutputTexture,*/ TFunction<void(TArray<float>& DebugValues)> AsyncCallback
	);


	/**
	 * @brief RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
	 */
	static void DispatchGameThread(
		const TFunction<void(FRHICommandListImmediate& RHICmdList)>& PreCall,
		FMeshUnitComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* AnimationOutputTexture, /*
		UTextureRenderTarget2DArray* DataOutputTexture,*/
		TFunction<void(TArray<float>& DebugValues)> AsyncCallback
	)
	{
		// CREATE A RENDER COMMAND WHICH DISPATCHES THE SHADER ON THE RENDER THREAD
		ENQUEUE_RENDER_COMMAND(TurboSequence_MeshUnit_ComputeShader_Lf)(
			[PreCall, &Params, AnimationOutputTexture, /*DataOutputTexture,*/ AsyncCallback](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(PreCall, RHICmdList, Params, AnimationOutputTexture, /*DataOutputTexture,*/ AsyncCallback);
			});
	}


	/**
	 * @brief RUNS THE BAKE SHADER COMMAND AND CAN CALLED ON EVERY THREAD
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
	 */
	static void Dispatch(
		const TFunction<void(FRHICommandListImmediate& RHICmdList)>& PreCall,
		FMeshUnitComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* AnimationOutputTexture, /*
		UTextureRenderTarget2DArray* DataOutputTexture,*/
		TFunction<void(TArray<float>& DebugValues)> AsyncCallback
	)
	{
		// WHEN WE ARE ON THE RENDER THREAD CALL DispatchRenderThread DIRECTLY
		if (IsInRenderingThread())
		{
			// WE CAN START WITH THE SHADER DISPATCH BY GETTING THE RENDER COMMAND IMMEDIATELY
			DispatchRenderThread(PreCall, GetImmediateCommandList_ForRenderCommand(), Params, AnimationOutputTexture, /*DataOutputTexture,*/ AsyncCallback);
		}
		// WHEN WE ARE ON THE GAME THREAD
		else
		{
			// RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
			DispatchGameThread(PreCall, Params, AnimationOutputTexture, /*DataOutputTexture,*/ AsyncCallback);
		}
	}
};

/**
 * @brief THIS EXECUTES THE BAKE SHADER ON THE GAME OR RENDER THREAD BY CALLING FAnimation_Baker_Compute_Shader_Execute_Lf::Dispatch()
 */
class TURBOSEQUENCE_SHADER_LF_API FSettingsCompute_Shader_Execute_Lf
{
public:
	/**
	 * @brief RUNS THE SHADER ON THE RENDER THREAD
	 * @param RHICmdList THE RENDER COMMAND LIST FROM ENQUEUE_RENDER_COMMAND
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 */
	static void DispatchRenderThread(
		FRHICommandListImmediate& RHICmdList,
		FSettingsComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* OutputTexture
	);


	/**
	 * @brief RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 */
	static void DispatchGameThread(
		FSettingsComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* OutputTexture
	)
	{
		// CREATE A RENDER COMMAND WHICH DISPATCHES THE SHADER ON THE RENDER THREAD
		ENQUEUE_RENDER_COMMAND(TurboSequence_Settings_ComputeShader_Lf)(
			[&Params, OutputTexture](FRHICommandListImmediate& RHICmdList)
			{
				DispatchRenderThread(RHICmdList, Params, OutputTexture);
			});
	}


	/**
	 * @brief RUNS THE BAKE SHADER COMMAND AND CAN CALLED ON EVERY THREAD
	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
	 */
	static void Dispatch(
		FSettingsComputeShader_Params_Lf& Params,
		UTextureRenderTarget2DArray* OutputTexture
	)
	{
		// WHEN WE ARE ON THE RENDER THREAD CALL DispatchRenderThread DIRECTLY
		if (IsInRenderingThread())
		{
			// WE CAN START WITH THE SHADER DISPATCH BY GETTING THE RENDER COMMAND IMMEDIATELY
			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, OutputTexture);
		}
		// WHEN WE ARE ON THE GAME THREAD
		else
		{
			// RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
			DispatchGameThread(Params, OutputTexture);
		}
	}
};


// /**
//  * @brief THIS EXECUTES THE BAKE SHADER ON THE GAME OR RENDER THREAD BY CALLING FAnimation_Baker_Compute_Shader_Execute_Lf::Dispatch()
//  */
// class TURBOSEQUENCE_SHADER_LF_API FRenderDataCompute_Shader_Execute_Lf
// {
// public:
// 	/**
// 	 * @brief RUNS THE SHADER ON THE RENDER THREAD
// 	 * @param RHICmdList THE RENDER COMMAND LIST FROM ENQUEUE_RENDER_COMMAND
// 	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
// 	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
// 	 */
// 	void DispatchRenderThread(
// 		FRHICommandListImmediate& RHICmdList,
// 		FRenderDataComputeShader_Params_Lf& Params,
// 		UTextureRenderTarget2DArray* OutputTexture, const TFunction<void(FVector4f DebugValues)>& AsyncCallback
// 	);
//
//
// 	/**
// 	 * @brief RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
// 	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
// 	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
// 	 */
// 	void DispatchGameThread(
// 		FRenderDataComputeShader_Params_Lf& Params,
// 		UTextureRenderTarget2DArray* OutputTexture,
// 		const TFunction<void(FVector4f DebugValues)>& AsyncCallback
// 	)
// 	{
// 		// CREATE A RENDER COMMAND WHICH DISPATCHES THE SHADER ON THE RENDER THREAD
// 		ENQUEUE_RENDER_COMMAND(TurboSequence_RenderData_ComputeShader_Lf)(
// 			[&Params, OutputTexture, AsyncCallback, this](FRHICommandListImmediate& RHICmdList)
// 			{
// 				if (!this)
// 				{
// 					return;
// 				}
// 				this->DispatchRenderThread(RHICmdList, Params, OutputTexture, AsyncCallback);
// 			});
// 	}
//
//
// 	/**
// 	 * @brief RUNS THE BAKE SHADER COMMAND AND CAN CALLED ON EVERY THREAD
// 	 * @param Params THE PARAMETER WHICH THE SHADER NEEDS
// 	 * @param AsyncCallback THE ASYNC CALLBACK, IN THIS CASE WE RETURN A DEBUG VALUE
// 	 */
// 	void Dispatch(
// 		FRenderDataComputeShader_Params_Lf& Params,
// 		UTextureRenderTarget2DArray* OutputTexture,
// 		const TFunction<void(FVector4f DebugValues)>& AsyncCallback
// 	)
// 	{
// 		// WHEN WE ARE ON THE RENDER THREAD CALL DispatchRenderThread DIRECTLY
// 		if (IsInRenderingThread())
// 		{
// 			// WE CAN START WITH THE SHADER DISPATCH BY GETTING THE RENDER COMMAND IMMEDIATELY
// 			DispatchRenderThread(GetImmediateCommandList_ForRenderCommand(), Params, OutputTexture, AsyncCallback);
// 		}
// 		// WHEN WE ARE ON THE GAME THREAD
// 		else
// 		{
// 			// RUNS THE SHADER ON THE GAME THREAD BY ENQUEUE A RENDER COMMAND
// 			DispatchGameThread(Params, OutputTexture, AsyncCallback);
// 		}
// 	}
// };
