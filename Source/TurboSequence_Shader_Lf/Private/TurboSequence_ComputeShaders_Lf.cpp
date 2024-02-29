// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.


#include "TurboSequence_ComputeShaders_Lf.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

// IMPLEMENTS THE SHADER TO A VIRTUAL DIRECTLY
// NODE: THE SHADER USE MainBakeEntry AS COMPUTE ENTRY POINT

IMPLEMENT_GLOBAL_SHADER(FTurboSequence_BoneTransform_CS_Lf, "/TurboSequence_Shaders/MeshUnit_CS_Lf.usf", "Main", SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FTurboSequence_Settings_CS_Lf, "/TurboSequence_Shaders/BoneSettings_CS_Lf.usf", "Main", SF_Compute);
//IMPLEMENT_GLOBAL_SHADER(FTurboSequence_RenderData_CS_Lf, "/TurboSequence_Shaders/RenderData_CS_Lf.usf", "Main", SF_Compute);

DECLARE_GPU_STAT(TurboSequence_Animation_ComputeShader);

//IMPLEMENT_UNIFORM_BUFFER_STRUCT(FTurboSequence_MeshUnit_CS_Lf::FUniformParameters, "MeshUnitUniformBuffers");

// RUNS THE SHADER ON THE RENDER THREAD
void FMeshUnit_Compute_Shader_Execute_Lf::DispatchRenderThread(
	const TFunction<void(FRHICommandListImmediate& RHICmdList)>& PreCall,
	FRHICommandListImmediate& RHICmdList,
	FMeshUnitComputeShader_Params_Lf& Params,
	UTextureRenderTarget2DArray* AnimationOutputTexture, /*
	UTextureRenderTarget2DArray* DataOutputTexture,*/
	TFunction<void(TArray<float>& DebugValues)> AsyncCallback
)
{
	PreCall(RHICmdList);

	if (!Params.NumMeshes)
	{
		return;
	}

	if (!AnimationOutputTexture->SizeX || !AnimationOutputTexture->SizeY || !AnimationOutputTexture->Slices)
	{
		return;
	}

	// if (!DataOutputTexture->SizeX || !DataOutputTexture->SizeY || !DataOutputTexture->Slices)
	// {
	// 	return;
	// }

	// if (!Params.AnimationRawData_RenderThread.Data.Num())
	// {
	// 	return;
	// }

	SCOPED_DRAW_EVENT(RHICmdList, TurboSequence_Animation_ComputeShader);
	SCOPED_GPU_STAT(RHICmdList, TurboSequence_Animation_ComputeShader);
	{
		const int32& NumDebugData = FMath::Max(Params.NumDebugData, 1);
		DebugData.SetNum(NumDebugData);

		//const int32& NumAnimations_RenderThread = FMath::Max(Params.NumAnimations, 1);

		// CREATE THE RDG GRAPH
		FRDGBuilder GraphBuilder(RHICmdList, FRDGEventName(*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::GraphName, Params.ShaderID)), ERDGBuilderFlags::AllowParallelExecute);

		// WE DON'T DO ANY VISUAL APPEARANCE AND ONLY DO HEAVY WORK ON THE GPU WITH AsyncCompute SHADERS
		// WHICH IS SUPPORTED ON Vulkan AND DX11 AND SHOULD RUN ON ALL MODERN DEVICES
		// SO WE USE FPermutationDomain = FShaderPermutationNone;
		const FTurboSequence_BoneTransform_CS_Lf::FPermutationDomain PermutationVector;

		FIntVector GroupCount = /*FIntVector(GET1_NUMBER, GET1_NUMBER, GET1_NUMBER); //*/FComputeShaderUtils::GetGroupCount(FTurboSequence_BoneTransform_CS_Lf::NumThreads, FComputeShaderUtils::kGolden2DGroupSize);

		TShaderMapRef<FTurboSequence_BoneTransform_CS_Lf> MeshUnitComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		// HERE WE ALLOCATE THE PARAMETERS FOR THE SHADER SO WE CAN FILL IT LATER DOWN THE ROAD
		FTurboSequence_BoneTransform_CS_Lf::FParameters* MeshUnitPassParameters = GraphBuilder.AllocParameters<FTurboSequence_BoneTransform_CS_Lf::FParameters>();

		//FTurboSequence_MeshUnit_CS_Lf::FUniformParameters* UniformParameters = FTurboSequence_MeshUnit_CS_Lf::GetUniformParameters(GraphBuilder);


		FRDGTextureRef AnimationOutputTextureRef;
		if (Params.bUse32BitTransformTexture)
		{
			MeshUnitPassParameters->RW_BoneTransform_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, AnimationOutputTextureRef, AnimationOutputTexture->SizeX, AnimationOutputTexture->SizeY, AnimationOutputTexture->Slices, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID), PF_A32B32G32R32F);
		}
		else
		{
			MeshUnitPassParameters->RW_BoneTransform_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, AnimationOutputTextureRef, AnimationOutputTexture->SizeX, AnimationOutputTexture->SizeY, AnimationOutputTexture->Slices, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID), PF_FloatRGBA);
		}
		//FRDGTextureRef DataOutputTextureRef;
		//MeshUnitPassParameters->RW_CustomData_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, DataOutputTextureRef, DataOutputTexture->SizeX, DataOutputTexture->SizeY, DataOutputTexture->Slices,*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::DataTextureDebugName, Params.ShaderID), PF_FloatRGBA);

		MeshUnitPassParameters->AnimationLibrary_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.AnimationRawData_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationLibraryBufferDebugName, Params.ShaderID), true);

		// MeshUnitPassParameters->R_AnimationLibrary_InputTexture = FTurboSequence_Helper_Lf::CreateReadRenderTargetArrayTexture_Half4_Out(GraphBuilder, *AnimationLibraryTexture, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationLibraryTextureDebugName, Params.ShaderID));

		MeshUnitPassParameters->ReferencePose_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.CPUInverseReferencePose, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::RefPoseDebugName, Params.ShaderID), true);
		MeshUnitPassParameters->ReferenceNumCPUBones_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.ReferenceNumCPUBones_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R16_UINT, true);

		//MeshUnitPassParameters->PerMeshCustomData_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.PerMeshCustomData_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataDebugName, Params.ShaderID), true);

		// MeshUnitPassParameters->ReferencePoseCPUIndices_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.CPUIndices, FTurboSequence_BoneTransform_CS_Lf::RefPoseCPUIndicesDebugName, PF_R32_FLOAT, true);
		// MeshUnitPassParameters->ReferencePoseGPUIndices_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.GPUIndices, FTurboSequence_BoneTransform_CS_Lf::RefPoseGPUIndicesDebugName, PF_R32_FLOAT, true);
		// MeshUnitPassParameters->ReferencePoseParentIndices_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.CPUParentIndices, FTurboSequence_BoneTransform_CS_Lf::RefPoseParentIndicesDebugName, PF_R32_FLOAT, true);
		MeshUnitPassParameters->ReferencePoseIndices_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.Indices, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::RefPoseCPUIndicesDebugName, Params.ShaderID), true);

		MeshUnitPassParameters->OutputTextureSizeX = AnimationOutputTexture->SizeX;
		MeshUnitPassParameters->OutputTextureSizeY = AnimationOutputTexture->SizeY;
		//MeshUnitPassParameters->SkinWeightTextureSizeX = Params.SkinWeightTextureDimensions.X;
		//MeshUnitPassParameters->SkinWeightTextureSizeY = Params.SkinWeightTextureDimensions.Y;

		//MeshUnitPassParameters->DataTextureSizeX = DataOutputTexture->SizeX;
		//MeshUnitPassParameters->DataTextureSizeY = DataOutputTexture->SizeY;

		// MeshUnitPassParameters->AnimationTextureSizeX = Params.AnimationTextureDimensions.X;
		// MeshUnitPassParameters->AnimationTextureSizeY = Params.AnimationTextureDimensions.Y;
		//MeshUnitPassParameters->UVCoordIndex = Params.UVCoordIndex;
		MeshUnitPassParameters->NumLevelOfDetails = Params.NumMaxLevelOfDetails;
		MeshUnitPassParameters->NumCPUBones = Params.NumMaxCPUBones;
		MeshUnitPassParameters->NumFirstGPUBones = Params.NumMaxGPUBones;
		const int32& NumMeshesPerThread = FMath::CeilToInt32(static_cast<float>(Params.NumMeshes) / static_cast<float>(FTurboSequence_BoneTransform_CS_Lf::NumThreads.X * FTurboSequence_BoneTransform_CS_Lf::NumThreads.Y * FTurboSequence_BoneTransform_CS_Lf::NumThreads.Z));
		MeshUnitPassParameters->NumMeshesPerThread = NumMeshesPerThread;
		MeshUnitPassParameters->NumMeshesPerFrame = Params.NumMeshes;
		MeshUnitPassParameters->NumPixelBuffer = FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer;
		MeshUnitPassParameters->NumCustomStates = FTurboSequence_Helper_Lf::NumCustomStates;
		//MeshUnitPassParameters->NumCustomDataPixels = FTurboSequence_Helper_Lf::NumInstanceCustomDataPixel;
		//MeshUnitPassParameters->bIsSimpleAnimationMode = Params.bSimpleAnimationMode;
		// if (!Params.BoneSpaceAnimationIKInput.Num())
		// {
		// 	return;
		// }
		// MeshUnitPassParameters->BoneSpaceAnimationInput_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.BoneSpaceAnimationIKInput, TEXT("TurboSequence_BoneSpaceTransforms"), true);

		MeshUnitPassParameters->PerMeshCustomDataIndices_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.PerMeshCustomDataIndex_Global_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R32_SINT, true);
		MeshUnitPassParameters->PerMeshCustomDataLod_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.PerMeshCustomDataLod_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataLodDebugName, Params.ShaderID), PF_R16_SINT, true);
		MeshUnitPassParameters->PerMeshCustomDataCollectionIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.PerMeshCustomDataCollectionIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R16_UINT, true);

		//MeshUnitPassParameters->SkinWeightOffsetLod_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.SkinWeightOffsetLod_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::SkinWeightOffsetLodDebugName, Params.ShaderID), PF_R32_SINT, true);
		//MeshUnitPassParameters->SkinWeightVertexLod_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.SkinWeightVertexLod_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::SkinWeightVertexLodDebugName, Params.ShaderID), PF_R16_UINT, true);


		MeshUnitPassParameters->AnimationStartIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationStartIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationStartIndexDebugName, Params.ShaderID), PF_R32_SINT, true);
		MeshUnitPassParameters->AnimationEndIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationEndIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationEndIndexDebugName, Params.ShaderID), PF_R32_SINT, true);
		//MeshUnitPassParameters->AnimationFrameAlpha_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationFrameAlpha_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationFrameAlphaDebugName, Params.ShaderID), PF_R16_SINT, true);
		MeshUnitPassParameters->AnimationFramePose0_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationFramePose0_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationFramePose0DebugName, Params.ShaderID), PF_R32_SINT, true);
		//MeshUnitPassParameters->AnimationFramePose1_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationFramePose1_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationFramePose1DebugName, Params.ShaderID), PF_R32_SINT, true);
		MeshUnitPassParameters->AnimationWeight_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationWeights_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);
		//MeshUnitPassParameters->AnimationLocationWeight_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationLocationWeights_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationLocationWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);
		// MeshUnitPassParameters->PerBoneAnimationWeight_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.PerBoneAnimationWeights_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::PerBoneAnimationWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);
		// MeshUnitPassParameters->PerBoneAnimationWeightIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.PerBoneAnimationWeightIndices_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::PerBoneAnimationWeightIndicesDebugName, Params.ShaderID), PF_R32_SINT, true);
		//MeshUnitPassParameters->AnimationRotationWeight_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationRotationWeights_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationRotationWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);
		//MeshUnitPassParameters->AnimationScaleWeight_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationScaleWeights_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationScaleWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);
		//MeshUnitPassParameters->AnimationID_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationID_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationIDsDebugName, Params.ShaderID), PF_R16_UINT, true);
		MeshUnitPassParameters->AnimationLayerIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationLayerIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationRootIndexDebugName, Params.ShaderID), PF_R16_UINT, true);
		//MeshUnitPassParameters->AnimationMaxFrames_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationFrameMaxFrames, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationFrameMaxFramesDebugName, Params.ShaderID), PF_R32_SINT, true);
		//MeshUnitPassParameters->AnimationFrameOffset_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, TArray<int32>{0}, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationOffsetDebugName, Params.ShaderID), PF_R32_SINT, true);

		if (!Params.AnimationLayers_RenderThread.Num())
		{
			TArray<int32> Layers;
			Layers.Add(GET0_NUMBER);
			MeshUnitPassParameters->AnimationLayerLibrary_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Layers, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationLayersDebugName, Params.ShaderID), PF_R16_SINT, false);
		}
		else
		{
			MeshUnitPassParameters->AnimationLayerLibrary_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.AnimationLayers_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::AnimationLayersDebugName, Params.ShaderID), PF_R16_UINT, true);
		}

		MeshUnitPassParameters->BoneSpaceAnimationInput_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.BoneSpaceAnimationIKInput_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKInputDebugName, Params.ShaderID), true);
		MeshUnitPassParameters->BoneSpaceAnimationDataInput_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.BoneSpaceAnimationIKData_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataInputDebugName, Params.ShaderID), PF_R16_SINT, true);
		MeshUnitPassParameters->BoneSpaceAnimationDataStartIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.BoneSpaceAnimationIKStartIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataStartIndexInputDebugName, Params.ShaderID), PF_R32_SINT, true);
		MeshUnitPassParameters->BoneSpaceAnimationDataEndIndex_StructuredBuffer = FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Params.BoneSpaceAnimationIKEndIndex_RenderThread, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataEndIndexInputDebugName, Params.ShaderID), PF_R32_SINT, true);

		FRDGBufferRef OutputDebugBufferRef;
		MeshUnitPassParameters->DebugValue = FTurboSequence_Helper_Lf::TCreateWriteBuffer<float>(GraphBuilder, DebugData.Num(), *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::DebugName, Params.ShaderID), PF_R32_FLOAT, OutputDebugBufferRef);
		MeshUnitPassParameters->NumDebugBuffer = FMath::Max(Params.NumDebugData, 0);


		// GraphBuilder.AddPass(
		// 	[=](FRHICommandListImmediate& RHICmdList)
		// 	{
		// 		
		// 	});

		const FRDGTextureRef RenderTargetAnimationOutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(AnimationOutputTexture->GetRenderTargetResource()->GetTexture2DArrayRHI(), *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureCopyDebugName, Params.ShaderID)));

		FRHICopyTextureInfo AnimationRenderTargetCopyInfo = FRHICopyTextureInfo();
		AnimationRenderTargetCopyInfo.NumSlices = AnimationOutputTexture->Slices;

		AddCopyTexturePass(GraphBuilder, RenderTargetAnimationOutputTexture, AnimationOutputTextureRef, AnimationRenderTargetCopyInfo);

		// const FRDGTextureRef RenderTargetCustomDataOutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(DataOutputTexture->GetRenderTargetResource()->GetTexture2DArrayRHI(), *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataTextureCopyDebugName, Params.ShaderID)));
		//
		// FRHICopyTextureInfo CustomDataRenderTargetCopyInfo = FRHICopyTextureInfo();
		// CustomDataRenderTargetCopyInfo.NumSlices = DataOutputTexture->Slices;

		//AddCopyTexturePass(GraphBuilder, RenderTargetCustomDataOutputTexture, DataOutputTextureRef, CustomDataRenderTargetCopyInfo);

		// GraphBuilder.AddPass(
		// 	RDG_EVENT_NAME("TurboSequence_Execute_Clear_CustomData_Texture %d", Params.ShaderID),
		// 	MeshUnitPassParameters,
		// 	ERDGPassFlags::Compute,
		// 	[&MeshUnitPassParameters](FRHIComputeCommandList& RHICommandList)
		// 	{
		// 		RHICommandList.ClearUAVFloat(MeshUnitPassParameters->RW_CustomData_OutputTexture->GetRHI(), FLinearColor::Transparent);
		// 	});
		GraphBuilder.AddPass(
			RDG_EVENT_NAME("TurboSequence_Execute_Writing_Transform_Texture %d", Params.ShaderID),
			MeshUnitPassParameters,
			ERDGPassFlags::Compute,
			[&MeshUnitPassParameters, MeshUnitComputeShader, GroupCount](FRHIComputeCommandList& RHICommandList)
			{
				//RHICommandList.ClearUAVFloat(MeshUnitPassParameters->RW_CustomData_OutputTexture->GetRHI(), FLinearColor::Transparent);
				//RHICommandList.ClearUAVFloat(MeshUnitPassParameters->RW_BoneTransform_OutputTexture->GetRHI(), FLinearColor::Transparent);

				FComputeShaderUtils::Dispatch(RHICommandList, MeshUnitComputeShader, *MeshUnitPassParameters, GroupCount);
			});

		// const FRDGTextureRef RenderTargetAnimationOutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(AnimationOutputTexture->GetRenderTargetResource()->GetTexture2DArrayRHI(), *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureCopyDebugName, Params.ShaderID)));
		//
		// FRHICopyTextureInfo AnimationRenderTargetCopyInfo = FRHICopyTextureInfo();
		// AnimationRenderTargetCopyInfo.NumSlices = AnimationOutputTexture->Slices;

		AddCopyTexturePass(GraphBuilder, AnimationOutputTextureRef, RenderTargetAnimationOutputTexture, AnimationRenderTargetCopyInfo);

		//AddClearRenderTargetPass(GraphBuilder, RenderTargetCustomDataOutputTexture);

		//ClearRenderTarget(RHICmdList, AnimationOutputTexture->GetRenderTargetResource()->GetTexture2DArrayRHI());

		// const FRDGTextureRef RenderTargetCustomDataOutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(DataOutputTexture->GetRenderTargetResource()->GetTexture2DArrayRHI(), *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataTextureCopyDebugName, Params.ShaderID)));
		//
		// FRHICopyTextureInfo CustomDataRenderTargetCopyInfo = FRHICopyTextureInfo();
		// CustomDataRenderTargetCopyInfo.NumSlices = DataOutputTexture->Slices;

		//AddCopyTexturePass(GraphBuilder, DataOutputTextureRef, RenderTargetCustomDataOutputTexture, CustomDataRenderTargetCopyInfo);

		// CALLBACK THE DEBUG VALUES
		if (Params.NumDebugData > GET0_NUMBER)
		{
			FTurboSequence_Helper_Lf::CallbackDebugValues(GraphBuilder, OutputDebugBufferRef, DebugData, AsyncCallback);
		}

		// Execute the graph
		GraphBuilder.Execute();

		// if (Params.TestIteration >= Params.MaxFrames)
		// {
		// 	Params.TestIteration = GET0_NUMBER;
		// }
	}
}

// RUNS THE SHADER ON THE RENDER THREAD
void FSettingsCompute_Shader_Execute_Lf::DispatchRenderThread(
	FRHICommandListImmediate& RHICmdList,
	FSettingsComputeShader_Params_Lf Params,
	UTextureRenderTarget2DArray* OutputTexture
)
{
	if (!IsValid(OutputTexture))
	{
		return;
	}

	if (!OutputTexture->GetRenderTargetResource())
	{
		return;
	}
	if (!OutputTexture->GetRenderTargetResource()->GetTextureRHI())
	{
		return;
	}

	if (!Params.SettingsInput.Num())
	{
		return;
	}

	// CREATE THE RDG GRAPH
	FRDGBuilder GraphBuilder(RHICmdList, FRDGEventName(*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::GraphName, Params.ShaderID)), ERDGBuilderFlags::AllowParallelExecute);
	// WE DON'T DO ANY VISUAL APPEARANCE AND ONLY DO HEAVY WORK ON THE GPU WITH AsyncCompute SHADERS
	// WHICH IS SUPPORTED ON Vulkan AND DX11 AND SHOULD RUN ON ALL MODERN DEVICES
	// SO WE USE FPermutationDomain = FShaderPermutationNone;
	const FTurboSequence_Settings_CS_Lf::FPermutationDomain PermutationVector;
	// HERE WE CREATE THE ACTUAL SHADER, WE USE GMaxRHIFeatureLevel BECAUSE IT SUPPORTS ALL FEATURES, SIMPLY
	// BECAUSE IT'S A COMPUTE SHADER AND SHOULD RUN ON Vulkan AND DX11 ON ALL DEVICES
	TShaderMapRef<FTurboSequence_Settings_CS_Lf> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
	// HERE WE ALLOCATE THE PARAMETERS FOR THE SHADER SO WE CAN FILL IT LATER DOWN THE ROAD
	FTurboSequence_Settings_CS_Lf::FParameters* PassParameters = GraphBuilder.AllocParameters<FTurboSequence_Settings_CS_Lf::FParameters>();
	// 8 192 IS THE TEXTURE SIZE X, WE ITERATE 32 THREADS x 16 CHUNKS x 16 PIXEL ON THE X
	// 8 192 IS THE TEXTURE SIZE Y, WE ITERATE 32 THREADS x 16 CHUNKS x 16 PIXEL ON THE Y
	// SO WE NEED 32x32 THREADS AND LEAVING THE 3rd DIMENSION TO 1
	FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(FIntVector(FTurboSequence_Settings_CS_Lf::NumThreads.X, FTurboSequence_Settings_CS_Lf::NumThreads.Y, GET1_NUMBER), FComputeShaderUtils::kGolden2DGroupSize);

	// CREATE THE AnimationLibraryOutputTextureRef FOR LATER DOWN THE ROAD FOR QueueTextureExtraction
	// BECAUSE WE WANT COPY THE OUTPUT LATER INTO THE RENDER TARGET TEXTURE
	FRDGTextureRef SettingOutputTextureRef;

	//const int32& NumPixels = Params.SettingsInput.Num();
	//const int16& SizeY = FMath::CeilToInt(static_cast<float>(NumPixels) / static_cast<float>(OutputTexture->SizeY));

	// ALLOCATE THE PASS PARAMETER RW_AnimationLibrary_OutputTexture WITH CreateWriteTexture_Half4_Out
	if (Params.bUse32BitTexture)
	{
		PassParameters->RW_Settings_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, SettingOutputTextureRef, OutputTexture->SizeX, OutputTexture->SizeY, OutputTexture->Slices, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::WriteTextureDebugName, Params.ShaderID), PF_A32B32G32R32F);
	}
	else
	{
		PassParameters->RW_Settings_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(GraphBuilder, SettingOutputTextureRef, OutputTexture->SizeX, OutputTexture->SizeY, OutputTexture->Slices, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::WriteTextureDebugName, Params.ShaderID), PF_FloatRGBA);
	}

	// ALLOCATE THE PASS PARAMETER KeyFrame_Input_In_Pixel_StructuredBuffer THIS ARE THE KEYFRAMES WHICH
	// WRITES THE ANIMATIONS TO RW_AnimationLibrary_OutputTexture
	// if (!Params.SettingsInput.Num())
	// {
	// 	Params.SettingsInput.AddUninitialized(GET1_NUMBER);
	// }
	PassParameters->BoneWeights_StructuredBuffer = FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Params.SettingsInput, *FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::DataInputDebugName, Params.ShaderID), true);

	// COPY THE DebugOutputBufferRef FOR DEBUGGING THE SHADER
	//FRDGBufferRef DebugOutputBufferRef;
	// CREATE A DEBUG WRITE BUFFER AND ALLOCATE DebugValue
	//PassParameters->DebugValue = FTurboSequence_Helper_Lf::CreateDebugWriteBuffer(GraphBuilder, DebugOutputBufferRef);

	PassParameters->TextureDimensionX = OutputTexture->SizeX;
	PassParameters->TextureDimensionY = OutputTexture->SizeY;
	const int32& NumThreads = FTurboSequence_Settings_CS_Lf::NumThreads.X * FTurboSequence_Settings_CS_Lf::NumThreads.Y;
	const int32& NumPixelsPerThread = FMath::CeilToInt32(static_cast<float>(Params.SettingsInput.Num()) / static_cast<float>(NumThreads));
	PassParameters->NumPixelPerThread = NumPixelsPerThread;

	//UE_LOG(LogTemp, Warning, TEXT("SizeY -> %d | NumPixels -> %d | NumPixelsPerThread -> %d | TexParamY -> %d | TexParamZ -> %d"), SizeY, NumPixels, NumPixelsPerThread, Params.TargetDimensions.Y, Params.TargetDimensions.Z);

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("Execute_TurboSequence_Settings_Clear_SettingsTexture %d", Params.ShaderID),
		PassParameters,
		ERDGPassFlags::Compute,
		[&PassParameters](FRHIComputeCommandList& RHICommandList)
		{
			RHICommandList.ClearUAVFloat(PassParameters->RW_Settings_OutputTexture->GetRHI(), FLinearColor::Transparent);
		});
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("Execute_TurboSequence_Settings_Input %d", Params.ShaderID),
		PassParameters,
		ERDGPassFlags::AsyncCompute,
		[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICommandList)
		{
			FComputeShaderUtils::Dispatch(RHICommandList, ComputeShader, *PassParameters, GroupCount);
		});

	const FRDGTextureRef RenderTargetOutputTexture = GraphBuilder.RegisterExternalTexture(CreateRenderTarget(OutputTexture->GetRenderTargetResource()->GetTextureRHI(), *FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureCopyDebugName));
	//TRefCountPtr<IPooledRenderTarget> PooledComputeRenderTargetTexture;
	// FILL PooledComputeRenderTargetTexture WITH THE WANTED REQUIRED INFORMATION OF THE BUFFER
	//GraphBuilder.QueueTextureExtraction(SettingOutputTextureRef, &PooledComputeRenderTargetTexture, ERDGResourceExtractionFlags::AllowTransient);

	FRHICopyTextureInfo CopyInfo = FRHICopyTextureInfo();
	// CopyInfo.Size = RenderTargetOutputTexture->Desc.GetSize();
	CopyInfo.NumSlices = OutputTexture->Slices;
	// CopyInfo.NumSlices = GET1_NUMBER; // Need to be 1
	// CopyInfo.SourceSliceIndex = GET0_NUMBER; // Need to be 0
	// CopyInfo.DestSliceIndex = Params.TargetDimensions.Z; // Can be something between 0 ... OutputTexture.Slices - 1
	// CopyInfo.SourcePosition = FIntVector::ZeroValue; // We start at 0,0 on the source
	// CopyInfo.DestPosition = FIntVector(GET0_NUMBER, Params.TargetDimensions.Y, GET0_NUMBER);
	// CopyInfo.Size = FIntVector(OutputTexture->SizeX, SizeY, GET1_NUMBER);
	AddCopyTexturePass(GraphBuilder, SettingOutputTextureRef, RenderTargetOutputTexture, CopyInfo);

	// CALLBACK THE DEBUG VALUES
	//FTurboSequence_Helper_Lf::CallbackDebugValues(GraphBuilder, DebugOutputBufferRef, AsyncCallback);

	// Execute the graph
	GraphBuilder.Execute();

	//UE_LOG(LogTurboSequence_Lf, Display, TEXT("Written %d Pixels with %d Threads"), Params.SettingsInput.Num(), NumThreads);


	//FRHICopyTextureInfo CopyInfo = FRHICopyTextureInfo();
	//CopyInfo.Size = RenderTargetOutputTexture->Desc.GetSize();
	//CopyInfo.NumSlices = OutputTexture->Slices;
	//RHICmdList.CopyTexture(PooledComputeRenderTargetTexture.GetReference()->GetRHI(), OutputTexture->GetRenderTargetResource()->TextureRHI, CopyInfo);
}
