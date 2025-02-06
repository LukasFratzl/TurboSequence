// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.


#include "TurboSequence_ComputeShaders_Lf.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"


IMPLEMENT_GLOBAL_SHADER(FTurboSequence_BoneTransform_CS_Lf, "/TurboSequence_Shaders/MeshUnit_CS_Lf.usf", "Main",
                        SF_Compute);
IMPLEMENT_GLOBAL_SHADER(FTurboSequence_Settings_CS_Lf, "/TurboSequence_Shaders/BoneSettings_CS_Lf.usf", "Main",
                        SF_Compute);

DECLARE_GPU_STAT(TurboSequence_Animation_ComputeShader);


void FMeshUnit_Compute_Shader_Execute_Lf::DispatchRenderThread(
	const TFunction<void(FRHICommandListImmediate& RHICmdList)>& PreCall,
	FRHICommandListImmediate& RHICmdList,
	FMeshUnitComputeShader_Params_Lf& Params,
	UTextureRenderTarget2DArray* AnimationOutputTextureCurrent, /*
	UTextureRenderTarget2DArray* DataOutputTexture,*/
	TFunction<void(TArray<float>& DebugValues)> AsyncCallback
)
{
	SCOPED_DRAW_EVENT(RHICmdList, TurboSequence_Animation_ComputeShader);
	SCOPED_GPU_STAT(RHICmdList, TurboSequence_Animation_ComputeShader);
	{
		PreCall(RHICmdList);

		if (!Params.NumMeshes)
		{
			return;
		}

		if (!Params.CPUInverseReferencePose.Num())
		{
			return;
		}

		if (!AnimationOutputTextureCurrent->SizeX || !AnimationOutputTextureCurrent->SizeY || !
			AnimationOutputTextureCurrent
			->Slices)
		{
			return;
		}

		if (!IsValid(Params.AnimationOutputTexturePrevious))
		{
			return;
		}

		if (AnimationOutputTextureCurrent->SizeX != Params.AnimationOutputTexturePrevious->SizeX ||
			AnimationOutputTextureCurrent->SizeY != Params.AnimationOutputTexturePrevious->SizeY ||
			AnimationOutputTextureCurrent->Slices != Params.AnimationOutputTexturePrevious->Slices ||
			AnimationOutputTextureCurrent->GetFormat() != Params.AnimationOutputTexturePrevious->GetFormat())
		{
			UE_LOG(LogTurboSequence_Lf, Error,
			       TEXT(
				       "The Transform Texture Current and the Transform Texture Previous are not the same size or format ... Please use the Control Panel Texture Generator in the Tweaks section to sync it, CurrentTexture -> %d, %d, %d, %hhd | PreviousTexture -> %d, %d, %d, %hhd"
			       ), AnimationOutputTextureCurrent->SizeX, AnimationOutputTextureCurrent->SizeY,
			       AnimationOutputTextureCurrent->Slices, AnimationOutputTextureCurrent->GetFormat(),
			       Params.AnimationOutputTexturePrevious->SizeX, Params.AnimationOutputTexturePrevious->SizeY,
			       Params.AnimationOutputTexturePrevious->Slices, Params.AnimationOutputTexturePrevious->GetFormat());
			return;
		}

		if (!IsValid(Params.AnimationLibraryTexture))
		{
			return;
		}
		int32 NumDebugData = FMath::Max(Params.NumDebugData, 1);
		DebugData.SetNum(NumDebugData);


		FRDGBuilder GraphBuilder(
			RHICmdList,
			FRDGEventName(
				*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::GraphName,
				                                           Params.ShaderID)), ERDGBuilderFlags::Parallel);


		const FTurboSequence_BoneTransform_CS_Lf::FPermutationDomain PermutationVector;

		int32 NumMeshesPerThread = GET1_NUMBER;

		int32 GroupCountX = FMath::CeilToInt32(
					FMath::CeilToInt32(static_cast<float>(Params.NumMeshes / NumMeshesPerThread)) / static_cast<float>(FTurboSequence_BoneTransform_CS_Lf::NumThreads.X *
						FTurboSequence_BoneTransform_CS_Lf::NumThreads.Y * FTurboSequence_BoneTransform_CS_Lf::NumThreads.Z));

		FIntVector GroupCount = FComputeShaderUtils::GetGroupCountWrapped(GroupCountX);

		TShaderMapRef<FTurboSequence_BoneTransform_CS_Lf> MeshUnitComputeShader(
			GetGlobalShaderMap(GMaxRHIFeatureLevel), PermutationVector);
		FTurboSequence_BoneTransform_CS_Lf::FParameters* MeshUnitPassParameters = GraphBuilder.AllocParameters<
			FTurboSequence_BoneTransform_CS_Lf::FParameters>();

		FRDGTextureRef AnimationOutputTextureRef;
		if (Params.bUse32BitTransformTexture)
		{
			MeshUnitPassParameters->RW_BoneTransform_OutputTexture =
				FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
					GraphBuilder, AnimationOutputTextureRef, AnimationOutputTextureCurrent->SizeX,
					AnimationOutputTextureCurrent->SizeY, AnimationOutputTextureCurrent->Slices,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID),
					PF_A32B32G32R32F);

			TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
			FRDGTextureRef OutputTextureRef;
			MeshUnitPassParameters->R_BoneTransform_OutputTexture =
				FTurboSequence_Helper_Lf::CreateReadRenderTargetArrayTexture_Custom_Out(
					GraphBuilder, PooledRenderTarget, OutputTextureRef, *AnimationOutputTextureCurrent,
					TEXT("TS_AnimationOutputTextureRead"), PF_A32B32G32R32F);
		}
		else
		{
			MeshUnitPassParameters->RW_BoneTransform_OutputTexture =
				FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
					GraphBuilder, AnimationOutputTextureRef, AnimationOutputTextureCurrent->SizeX,
					AnimationOutputTextureCurrent->SizeY, AnimationOutputTextureCurrent->Slices,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID),
					PF_FloatRGBA);

			MeshUnitPassParameters->R_BoneTransform_OutputTexture =
				FTurboSequence_Helper_Lf::CreateReadRenderTargetArrayTexture_Half4_Out(
					GraphBuilder, *AnimationOutputTextureCurrent, TEXT("TS_AnimationOutputTextureRead"));
		}

		FRDGTextureRef AnimationOutputTexturePeviousRef;
		if (Params.bUse32BitTransformTexture)
		{
			MeshUnitPassParameters->RW_BoneTransformPrevious_OutputTexture =
				FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
					GraphBuilder, AnimationOutputTexturePeviousRef, AnimationOutputTextureCurrent->SizeX,
					AnimationOutputTextureCurrent->SizeY, AnimationOutputTextureCurrent->Slices,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID + 1),
					PF_A32B32G32R32F);
		}
		else
		{
			MeshUnitPassParameters->RW_BoneTransformPrevious_OutputTexture =
				FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
					GraphBuilder, AnimationOutputTexturePeviousRef, AnimationOutputTextureCurrent->SizeX,
					AnimationOutputTextureCurrent->SizeY, AnimationOutputTextureCurrent->Slices,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureDebugName, Params.ShaderID + 1),
					PF_FloatRGBA);
		}

		MeshUnitPassParameters->R_AnimationLibrary_InputTexture =
			FTurboSequence_Helper_Lf::CreateReadRenderTargetArrayTexture_Half4_Out(
				GraphBuilder, *Params.AnimationLibraryTexture, TEXT("TS_AnimationLibrary"));

		MeshUnitPassParameters->ReferencePose_StructuredBuffer =
			FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(
				GraphBuilder, Params.CPUInverseReferencePose,
				*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::RefPoseDebugName,
				                                           Params.ShaderID), true);
		MeshUnitPassParameters->ReferenceNumCPUBones_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.ReferenceNumCPUBones_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R16_UINT,
				true);

		MeshUnitPassParameters->ReferencePoseIndices_StructuredBuffer =
			FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(
				GraphBuilder, Params.Indices,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::RefPoseCPUIndicesDebugName, Params.ShaderID), true);

		MeshUnitPassParameters->OutputTextureSizeX = AnimationOutputTextureCurrent->SizeX;
		MeshUnitPassParameters->OutputTextureSizeY = AnimationOutputTextureCurrent->SizeY;

		MeshUnitPassParameters->AnimTextureSizeX = Params.AnimationLibraryTexture->SizeX;
		MeshUnitPassParameters->AnimTextureSizeY = Params.AnimationLibraryTexture->SizeY;

		MeshUnitPassParameters->NumLevelOfDetails = Params.NumMaxLevelOfDetails;
		MeshUnitPassParameters->NumCPUBones = Params.NumMaxCPUBones;
		MeshUnitPassParameters->NumFirstGPUBones = Params.NumMaxGPUBones;
		// int32 NumMeshesPerThread = FMath::CeilToInt32(
		// 	static_cast<float>(Params.NumMeshes) / static_cast<float>(FTurboSequence_BoneTransform_CS_Lf::NumThreads.X *
		// 		FTurboSequence_BoneTransform_CS_Lf::NumThreads.Y * FTurboSequence_BoneTransform_CS_Lf::NumThreads.Z));
		MeshUnitPassParameters->NumMeshesPerThread = NumMeshesPerThread;
		MeshUnitPassParameters->NumMeshesPerFrame = Params.NumMeshes;
		MeshUnitPassParameters->NumPixelBuffer = FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer;
		MeshUnitPassParameters->NumCustomStates = FTurboSequence_Helper_Lf::NumCustomStates;

		MeshUnitPassParameters->PerMeshCustomDataIndices_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.PerMeshCustomDataIndex_Global_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R32_SINT,
				true);
		MeshUnitPassParameters->PerMeshCustomDataLod_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.PerMeshCustomDataLod_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::CustomDataLodDebugName,
				                                           Params.ShaderID), PF_R16_SINT, true);
		MeshUnitPassParameters->PerMeshCustomDataCollectionIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.PerMeshCustomDataCollectionIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::CustomDataIndicesDebugName, Params.ShaderID), PF_R16_UINT,
				true);


		MeshUnitPassParameters->AnimationStartIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.AnimationStartIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::AnimationStartIndexDebugName, Params.ShaderID), PF_R32_SINT,
				true);
		MeshUnitPassParameters->AnimationEndIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.AnimationEndIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::AnimationEndIndexDebugName, Params.ShaderID), PF_R16_SINT,
				true);

		MeshUnitPassParameters->AnimationFramePose0_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.AnimationFramePose0_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::AnimationFramePose0DebugName, Params.ShaderID), PF_R32_SINT,
				true);

		MeshUnitPassParameters->AnimationWeight_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.AnimationWeights_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::AnimationWeightsDebugName, Params.ShaderID), PF_R16_SINT, true);

		MeshUnitPassParameters->AnimationLayerIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.AnimationLayerIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::AnimationRootIndexDebugName, Params.ShaderID), PF_R16_UINT,
				true);

		if (!Params.AnimationLayers_RenderThread.Num())
		{
			TArray<int32> Layers;
			Layers.Add(GET0_NUMBER);
			MeshUnitPassParameters->AnimationLayerLibrary_StructuredBuffer =
				FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
					GraphBuilder, Layers,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::AnimationLayersDebugName, Params.ShaderID), PF_R16_SINT,
					false);
		}
		else
		{
			MeshUnitPassParameters->AnimationLayerLibrary_StructuredBuffer =
				FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
					GraphBuilder, Params.AnimationLayers_RenderThread,
					*FTurboSequence_Helper_Lf::FormatDebugName(
						FTurboSequence_BoneTransform_CS_Lf::AnimationLayersDebugName, Params.ShaderID), PF_R16_UINT,
					true);
		}

		MeshUnitPassParameters->BoneSpaceAnimationInput_StructuredBuffer =
			FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(
				GraphBuilder, Params.BoneSpaceAnimationIKInput_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKInputDebugName, Params.ShaderID), true);
		MeshUnitPassParameters->BoneSpaceAnimationDataInput_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.BoneSpaceAnimationIKData_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataInputDebugName, Params.ShaderID),
				PF_R16_SINT, true);
		MeshUnitPassParameters->BoneSpaceAnimationDataStartIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.BoneSpaceAnimationIKStartIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataStartIndexInputDebugName,
					Params.ShaderID), PF_R32_SINT, true);
		MeshUnitPassParameters->BoneSpaceAnimationDataEndIndex_StructuredBuffer =
			FTurboSequence_Helper_Lf::TCreateStructuredReadBufferFromTArray_Custom_Out(
				GraphBuilder, Params.BoneSpaceAnimationIKEndIndex_RenderThread,
				*FTurboSequence_Helper_Lf::FormatDebugName(
					FTurboSequence_BoneTransform_CS_Lf::BoneSpaceAnimationIKDataEndIndexInputDebugName,
					Params.ShaderID), PF_R16_SINT, true);

		FRDGBufferRef OutputDebugBufferRef;
		MeshUnitPassParameters->DebugValue = FTurboSequence_Helper_Lf::TCreateWriteBuffer<float>(
			GraphBuilder, DebugData.Num(),
			*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_BoneTransform_CS_Lf::DebugName, Params.ShaderID),
			PF_R32_FLOAT, OutputDebugBufferRef);
		MeshUnitPassParameters->NumDebugBuffer = FMath::Max(Params.NumDebugData, 0);

		const FRDGTextureRef RenderTargetAnimationOutputTexture = GraphBuilder.RegisterExternalTexture(
			CreateRenderTarget(AnimationOutputTextureCurrent->GetRenderTargetResource()->GetTexture2DArrayRHI(),
			                   *FTurboSequence_Helper_Lf::FormatDebugName(
				                   FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureCopyDebugName,
				                   Params.ShaderID)));

		const FRDGTextureRef RenderTargetAnimationOutputPreviousFrameTexture = GraphBuilder.RegisterExternalTexture(
			CreateRenderTarget(Params.AnimationOutputTexturePrevious->GetRenderTargetResource()->GetTextureRHI(),
			                   TEXT("TurboSequence_AnimationOutputRenderTargetTexturePreviousFrame")));

		FRHICopyTextureInfo AnimationRenderTargetCopyInfo = FRHICopyTextureInfo();
		AnimationRenderTargetCopyInfo.NumSlices = AnimationOutputTextureCurrent->Slices;

		AddCopyTexturePass(GraphBuilder, RenderTargetAnimationOutputTexture, AnimationOutputTextureRef,
		                   AnimationRenderTargetCopyInfo);

		AddCopyTexturePass(GraphBuilder, RenderTargetAnimationOutputPreviousFrameTexture,
		                   AnimationOutputTexturePeviousRef, AnimationRenderTargetCopyInfo);


		GraphBuilder.AddPass(
			RDG_EVENT_NAME("TurboSequence_Execute_Writing_Transform_Texture %d", Params.ShaderID),
			MeshUnitPassParameters,
			ERDGPassFlags::Compute,
			[&MeshUnitPassParameters, MeshUnitComputeShader, GroupCount](FRHIComputeCommandList& RHICommandList)
			{
				FComputeShaderUtils::Dispatch(RHICommandList, MeshUnitComputeShader, *MeshUnitPassParameters,
				                              GroupCount);
			});

		AddCopyTexturePass(GraphBuilder, AnimationOutputTextureRef, RenderTargetAnimationOutputTexture,
		                   AnimationRenderTargetCopyInfo);

		AddCopyTexturePass(GraphBuilder, AnimationOutputTexturePeviousRef,
		                   RenderTargetAnimationOutputPreviousFrameTexture, AnimationRenderTargetCopyInfo);

#if TURBO_SEQUENCE_DEBUG_GPU_READBACK
		if (Params.NumDebugData > GET0_NUMBER)
		{
			FTurboSequence_Helper_Lf::CallbackDebugValues(GraphBuilder, OutputDebugBufferRef, DebugData, AsyncCallback);
		}
#endif

		// Execute the graph
		GraphBuilder.Execute();
	}
}

// RUNS THE SHADER ON THE RENDER THREAD
void FSettingsCompute_Shader_Execute_Lf::DispatchRenderThread(
	FRHICommandListImmediate& RHICmdList,
	FSettingsComputeShader_Params_Lf& Params,
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

	FRDGBuilder GraphBuilder(
		RHICmdList,
		FRDGEventName(
			*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::GraphName, Params.ShaderID)),
		ERDGBuilderFlags::Parallel);

	const FTurboSequence_Settings_CS_Lf::FPermutationDomain PermutationVector;

	TShaderMapRef<FTurboSequence_Settings_CS_Lf> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel),
	                                                           PermutationVector);

	FTurboSequence_Settings_CS_Lf::FParameters* PassParameters = GraphBuilder.AllocParameters<
		FTurboSequence_Settings_CS_Lf::FParameters>();

	FIntVector GroupCount = FIntVector(GET1_NUMBER, GET1_NUMBER, GET1_NUMBER);

	FRDGTextureRef SettingOutputTextureRef;
	if (Params.bUse32BitTexture)
	{
		PassParameters->RW_Settings_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
			GraphBuilder, SettingOutputTextureRef, OutputTexture->SizeX, OutputTexture->SizeY, OutputTexture->Slices,
			*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::WriteTextureDebugName,
			                                           Params.ShaderID), PF_A32B32G32R32F);
	}
	else
	{
		PassParameters->RW_Settings_OutputTexture = FTurboSequence_Helper_Lf::CreateWriteTextureArray_Custom_Out(
			GraphBuilder, SettingOutputTextureRef, OutputTexture->SizeX, OutputTexture->SizeY, OutputTexture->Slices,
			*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::WriteTextureDebugName,
			                                           Params.ShaderID), PF_FloatRGBA);
	}

	PassParameters->BoneWeights_StructuredBuffer =
		FTurboSequence_Helper_Lf::CreateStructuredReadBufferFromTArray_Half4_Out(
			GraphBuilder, Params.SettingsInput,
			*FTurboSequence_Helper_Lf::FormatDebugName(FTurboSequence_Settings_CS_Lf::DataInputDebugName,
			                                           Params.ShaderID), true);

	PassParameters->TextureDimensionX = OutputTexture->SizeX;
	PassParameters->TextureDimensionY = OutputTexture->SizeY;
	int32 NumThreads = FTurboSequence_Settings_CS_Lf::NumThreads.X * FTurboSequence_Settings_CS_Lf::NumThreads.Y;
	int32 NumPixelsPerThread = FMath::CeilToInt32(
		static_cast<float>(Params.SettingsInput.Num()) / static_cast<float>(NumThreads));
	PassParameters->NumPixelPerThread = NumPixelsPerThread;
	if (Params.bIsAdditiveWrite)
	{
		PassParameters->BaseIndex = Params.AdditiveWriteBaseIndex;

		uint16 NumSlicesWritten = FMath::Min(
			FMath::CeilToInt(
				static_cast<float>((Params.AdditiveWriteBaseIndex + Params.SettingsInput.Num()) / (OutputTexture->SizeX
					* OutputTexture->SizeY))) + GET1_NUMBER, 1024);

		if (NumSlicesWritten >= OutputTexture->Slices)
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "You run out of Animation Pixel Space, which mean the Data Texture is on it's limits, you can resize the Data Texture when you navigate to ProjectDirectory/Plugins/TurboSequence/Content/Resources/T_TurboSequence_AnimationLibraryTexture_Lf, Make sure to Increase Slices or Upper the X Y ...."
			       ));
		}
	}
	else
	{
		PassParameters->BaseIndex = GET0_NUMBER;
	}

	const FRDGTextureRef RenderTargetOutputTexture = GraphBuilder.RegisterExternalTexture(
		CreateRenderTarget(OutputTexture->GetRenderTargetResource()->GetTextureRHI(),
		                   *FTurboSequence_BoneTransform_CS_Lf::BoneTransformsTextureCopyDebugName));

	FRHICopyTextureInfo CopyInfo = FRHICopyTextureInfo();
	CopyInfo.NumSlices = OutputTexture->Slices;


	if (Params.bIsAdditiveWrite)
	{
		AddCopyTexturePass(GraphBuilder, RenderTargetOutputTexture, SettingOutputTextureRef, CopyInfo);
	}
	else
	{
		GraphBuilder.AddPass(
			RDG_EVENT_NAME("Execute_TurboSequence_Settings_Clear_SettingsTexture %d", Params.ShaderID),
			PassParameters,
			ERDGPassFlags::Compute,
			[&PassParameters](FRHIComputeCommandList& RHICommandList)
			{
				RHICommandList.ClearUAVFloat(PassParameters->RW_Settings_OutputTexture->GetRHI(),
				                             FLinearColor::Transparent);
			});
	}
	GraphBuilder.AddPass(
		RDG_EVENT_NAME("Execute_TurboSequence_Settings_Input %d", Params.ShaderID),
		PassParameters,
		ERDGPassFlags::AsyncCompute,
		[&PassParameters, ComputeShader, GroupCount](FRHIComputeCommandList& RHICommandList)
		{
			FComputeShaderUtils::Dispatch(RHICommandList, ComputeShader, *PassParameters, GroupCount);
		});

	AddCopyTexturePass(GraphBuilder, SettingOutputTextureRef, RenderTargetOutputTexture, CopyInfo);

	// Execute the graph
	GraphBuilder.Execute();
}
