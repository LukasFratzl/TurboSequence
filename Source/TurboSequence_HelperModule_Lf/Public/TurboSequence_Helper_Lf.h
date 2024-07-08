// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include <thread>

#include "CoreMinimal.h"
#include "AnimationRuntime.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "RHIDefinitions.h"
#include "RHIGPUReadback.h"
#include "TextureRenderTarget2DArrayResource.h"
#include "Animation/AnimationPoseData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Async/Async.h"
#include "Engine/Texture2DArray.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Float16.h"
#include "Misc/HashBuilder.h"
#include "UObject/SavePackage.h"

#include "TurboSequence_Helper_Lf.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTurboSequence_Lf, Log, All);

inline DEFINE_LOG_CATEGORY(LogTurboSequence_Lf);


//         creates INFINITY AS 4 Byte value
#define GET_INFINITY ( TNumericLimits<float>::Max() )

//         creates 0 AS 1 Byte value
#define GET0_NUMBER ( static_cast<uint8>(0) )
//         creates 1 AS 1 Byte value -> It'S a power of 2 value
#define GET1_NUMBER ( static_cast<uint8>(1) )
//         creates 2 AS 1 Byte value -> It'S a power of 2 value
#define GET2_NUMBER ( static_cast<uint8>(2) )
//         creates 3 AS 1 Byte value
#define GET3_NUMBER ( static_cast<uint8>(3) )
//         creates 4 AS 1 Byte value -> It'S a power of 2 value
#define GET4_NUMBER ( static_cast<uint8>(4) )
//         creates 5 AS 1 Byte value
#define GET5_NUMBER ( static_cast<uint8>(5) )
//         creates 6 AS 1 Byte value
#define GET6_NUMBER ( static_cast<uint8>(6) )
//         creates 7 AS 1 Byte value
#define GET7_NUMBER ( static_cast<uint8>(7) )
//         creates 8 AS 1 Byte value -> It'S a power of 2 value
#define GET8_NUMBER ( static_cast<uint8>(8) )
//         creates 9 AS 1 Byte value
#define GET9_NUMBER ( static_cast<uint8>(9) )
//         creates 10 AS 1 Byte value
#define GET10_NUMBER ( static_cast<uint8>(10) )
//         creates 11 AS 1 Byte value
#define GET11_NUMBER ( static_cast<uint8>(11) )
//         creates 12 AS 1 Byte value
#define GET12_NUMBER ( static_cast<uint8>(12) )
//         creates 13 AS 1 Byte value
#define GET13_NUMBER ( static_cast<uint8>(13) )
//         creates 16 AS 1 Byte value -> It'S a power of 2 value
#define GET16_NUMBER ( static_cast<uint8>(16) )
//         creates 17 AS 1 Byte value
#define GET17_NUMBER ( static_cast<uint8>(17) )
//         creates 18 AS 1 Byte value
#define GET18_NUMBER ( static_cast<uint8>(18) )
//         creates 19 AS 1 Byte value
#define GET19_NUMBER ( static_cast<uint8>(19) )
//         creates 24 AS 1 Byte value
#define GET24_NUMBER ( static_cast<uint8>(24) )
//         creates 32 AS 1 Byte value -> It'S a power of 2 value
#define GET32_NUMBER ( static_cast<uint8>(32) )
//         creates 60 AS 1 Byte value
#define GET60_NUMBER ( static_cast<uint8>(60) )
//         creates 100 AS 1 Byte value
#define GET100_NUMBER ( static_cast<uint8>(100) )
//         creates 128 AS 1 Byte value -> It'S a power of 2 value
#define GET128_NUMBER ( static_cast<uint8>(128) )

//         creates 512 AS 2 Byte value -> It'S a power of 2 value
#define GET512_NUMBER ( static_cast<uint16>(512) )
//         creates 1000 AS 2 Byte value
#define GET1000_NUMBER ( static_cast<uint16>(1000) )
//         creates 1024 AS 2 Byte value -> It'S a power of 2 value
#define GET1024_NUMBER ( static_cast<uint16>(1024) )
//         creates 2048 AS 2 Byte value -> It'S a power of 2 value
#define GET2048_NUMBER ( static_cast<uint16>(2048) )
//         creates 4096 AS 2 Byte value -> It'S a power of 2 value
#define GET4096_NUMBER ( static_cast<uint16>(4096) )
//         creates 5500 AS 2 Byte value
#define GET5500_NUMBER ( static_cast<uint16>(5500) )
//         creates 6000 AS 2 Byte value
#define GET6000_NUMBER ( static_cast<uint16>(6000) )
//         creates 8192 AS 2 Byte value -> It'S a power of 2 value
#define GET8192_NUMBER ( static_cast<uint16>(8192) )


//         creates -1 AS 1 Byte value -> It'S a negative power of 2 value
#define GET1_NEGATIVE_NUMBER ( static_cast<int8>(-1) )
//         creates -20 AS 1 Byte value
#define GET20_NEGATIVE_NUMBER ( static_cast<int8>(-20) )
//         creates -45 AS 1 Byte value
#define GET45_NEGATIVE_NUMBER ( static_cast<int8>(-45) )
//         creates -100 AS 1 Byte value
#define GET100_NEGATIVE_NUMBER ( static_cast<int8>(-100) )
//         creates -135 AS 2 Byte value
#define GET135_NEGATIVE_NUMBER ( static_cast<int16>(-135) )


// -> Licence Start
// Copyright Epic Games, Inc. All Rights Reserved.
UENUM(BlueprintType)
enum class EAnimDataEvalType_Lf : uint8
{
	// Evaluates the original Animation Source data 
	Source,
	// Evaluates the original Animation Source data with additive animation layers
	Raw,
	// Evaluates the compressed Animation data - matching runtime (cooked)
	Compressed
};

USTRUCT(BlueprintType)
struct TURBOSEQUENCE_HELPERMODULE_LF_API FAnimPoseEvaluationOptions_Lf
{
	GENERATED_BODY()

	EAnimDataEvalType_Lf EvaluationType = EAnimDataEvalType_Lf::Raw;

	bool bShouldRetarget = true;

	bool bExtractRootMotion = false;

	UPROPERTY()
	TObjectPtr<USkeletalMesh> OptionalSkeletalMesh = nullptr;

	bool bRetrieveAdditiveAsFullPose = true;

	bool bEvaluateCurves = true;
};

USTRUCT()
struct TURBOSEQUENCE_HELPERMODULE_LF_API FTurboSequence_TransposeMatrix_Lf
{
	GENERATED_BODY()

	FTurboSequence_TransposeMatrix_Lf()
	{
	}

	~FTurboSequence_TransposeMatrix_Lf()
	{
	}

	TStaticArray<FVector4f, 3> Colum;
};


/** Script friendly representation of an evaluated animation bone pose */
USTRUCT(BlueprintType)
struct TURBOSEQUENCE_HELPERMODULE_LF_API FAnimPose_Lf
{
	GENERATED_BODY()

	/** Returns whether or not the pose data was correctly initialized and populated */
	//bool IsMeshDataValid() const;

	/** Initializes the various arrays, using and copying the provided bone container */
	void Init(const FBoneContainer& InBoneContainer)
	{
		Reset();

		const FReferenceSkeleton& RefSkeleton = InBoneContainer.GetSkeletonAsset()->GetReferenceSkeleton();

		for (const FBoneIndexType BoneIndex : InBoneContainer.GetBoneIndicesArray())
		{
			const FCompactPoseBoneIndex CompactIndex(BoneIndex);
			const FCompactPoseBoneIndex CompactParentIndex = InBoneContainer.GetParentBoneIndex(CompactIndex);

			const int32 SkeletonBoneIndex = InBoneContainer.GetSkeletonIndex(CompactIndex);
			if (SkeletonBoneIndex != INDEX_NONE)
			{
				//const int32 ParentBoneIndex = CompactParentIndex.GetInt() != INDEX_NONE
				//	                              ? InBoneContainer.GetSkeletonIndex(CompactParentIndex)
				//	                              : INDEX_NONE;

				BoneIndices.Add(SkeletonBoneIndex);
				//ParentBoneIndices.Add(ParentBoneIndex);

				BoneNames.Add(RefSkeleton.GetBoneName(SkeletonBoneIndex));

				//RefLocalSpacePoses.Add(InBoneContainer.GetRefPoseTransform(FCompactPoseBoneIndex(BoneIndex)));
			}
		}

		TArray<bool> Processed;
		Processed.SetNumZeroed(BoneNames.Num());
		//RefWorldSpacePoses.SetNum(BoneNames.Num());
		for (int32 EntryIndex = 0; EntryIndex < BoneNames.Num(); ++EntryIndex)
		{
			// const int32 ParentIndex = ParentBoneIndices[EntryIndex];
			// const int32 TransformIndex = BoneIndices.IndexOfByKey(ParentIndex);
			//
			// if (TransformIndex != INDEX_NONE)
			// {
			// 	ensure(Processed[TransformIndex]);
			// 	RefWorldSpacePoses[EntryIndex] = RefLocalSpacePoses[EntryIndex] * RefWorldSpacePoses[TransformIndex];
			// }
			// else
			// {
			// 	RefWorldSpacePoses[EntryIndex] = RefLocalSpacePoses[EntryIndex];
			// }

			Processed[EntryIndex] = true;
		}
	}

	// /** Populates an FCompactPose using the contained bone data */
	// void GetPose(FCompactPose& InOutCompactPose) const;
	// /** Generates the contained bone data using the provided Component and its AnimInstance */
	// void SetPose(USkeletalMeshComponent* Component);
	// /** Generates the contained bone data using the provided CompactPose */
	void SetPose(const FAnimationPoseData& PoseData)
	{
		const FCompactPose& CompactPose = PoseData.GetPose();
		if (IsInitialized())
		{
			const FBoneContainer& ContextBoneContainer = CompactPose.GetBoneContainer();

			LocalSpacePoses.SetNum(BoneIndices.Num());
			for (const FCompactPoseBoneIndex BoneIndex : CompactPose.ForEachBoneIndex())
			{
				const int32 SkeletonBoneIndex = ContextBoneContainer.GetSkeletonIndex(BoneIndex);

				FTransform PoseTransform = CompactPose[BoneIndex];
				PoseTransform.NormalizeRotation();

				const FMatrix& PoseMatrix = PoseTransform.ToMatrixWithScale();

				FTurboSequence_TransposeMatrix_Lf FinalPoseMatrix;
				for (uint8 M = GET0_NUMBER; M < GET3_NUMBER; ++M)
				{
					FinalPoseMatrix.Colum[M].X = PoseMatrix.M[GET0_NUMBER][M];
					FinalPoseMatrix.Colum[M].Y = PoseMatrix.M[GET1_NUMBER][M];
					FinalPoseMatrix.Colum[M].Z = PoseMatrix.M[GET2_NUMBER][M];
					FinalPoseMatrix.Colum[M].W = PoseMatrix.M[GET3_NUMBER][M];
				}

				LocalSpacePoses[BoneIndices.IndexOfByKey(SkeletonBoneIndex)] = FinalPoseMatrix;
			}

			//ensure(LocalSpacePoses.Num() == RefLocalSpacePoses.Num());
			//GenerateWorldSpaceTransforms();

			const FBlendedCurve& Curve = PoseData.GetCurve();
			Curve.ForEachElement(
				[&CurveNames = CurveNames, &CurveValues = CurveValues](const UE::Anim::FCurveElement& InElement)
				{
					CurveNames.Add(InElement.Name);
					CurveValues.Add(InElement.Value);
				});
		}
		else
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Anim Pose was not previously initialized"));
		}
	}

	// /** Copies the reference pose to animated pose data */
	// void SetToRefPose();
	//
	// /** (Re-)Generates the world space transforms using populated local space data */
	// void GenerateWorldSpaceTransforms();
	// /** Resets all contained data, rendering the instance invalid */
	void Reset()
	{
		BoneNames.Empty();
		BoneIndices.Empty();
		//ParentBoneIndices.Empty();
		LocalSpacePoses.Empty();
		//WorldSpacePoses.Empty();
		//RefLocalSpacePoses.Empty();
		//RefWorldSpacePoses.Empty();
	}

	/** Whether or not the contained data was initialized and can be used to store a pose */
	bool IsInitialized() const { return BoneNames.Num() != 0; }
	/** Whether or local space pose data has been populated */
	bool IsPopulated() const { return LocalSpacePoses.Num() != 0; }

	UPROPERTY()
	TArray<FName> BoneNames;

	UPROPERTY()
	TArray<int32> BoneIndices;

	//UPROPERTY()
	//TArray<int32> ParentBoneIndices;

	UPROPERTY()
	TArray<FTurboSequence_TransposeMatrix_Lf> LocalSpacePoses;

	//UPROPERTY()
	//TArray<FTransform> WorldSpacePoses;

	//UPROPERTY()
	//TArray<FTransform> RefLocalSpacePoses;

	//UPROPERTY()
	//TArray<FTransform> RefWorldSpacePoses;

	UPROPERTY()
	TArray<FName> CurveNames;

	UPROPERTY()
	TArray<float> CurveValues;
};

// -> Licence End

/**
 * 
 */
class TURBOSEQUENCE_HELPERMODULE_LF_API FTurboSequence_Helper_Lf
{
public:
	FTurboSequence_Helper_Lf() = delete;
	~FTurboSequence_Helper_Lf() = delete;

	static FORCEINLINE_DEBUGGABLE FName GetEngineVersionAsString()
	{
		return FName(ENGINE_VERSION_STRING);
	}

	static FORCEINLINE_DEBUGGABLE FString GetEditorPlatformAsString()
	{
		return GetTargetPlatformManagerRef().GetRunningTargetPlatform()->PlatformName();
	}

	static constexpr uint8 NumGPUTextureBoneBuffer = GET3_NUMBER;
	static constexpr uint8 NumGPUBoneIKBuffer = GET3_NUMBER;
	//inline static constexpr uint8 NumInstanceCustomDataPixel = GET2_NUMBER; // Four 16 bit floats per patch
	static constexpr uint8 NumInstanceCustomData = GET16_NUMBER; // Max Niagara
	static constexpr uint8 NumCustomStates = GET1_NUMBER;
	static constexpr uint8 NumSkinWeightPixels = GET7_NUMBER; // 6 For the weights, 1 for custom data

	inline static const FString PluginConfigName = FString("TurboSequence_Lf.ini");

	inline static const FString ReferenceTurboSequenceNiagaraSystem = FString(
		"/Script/Niagara.NiagaraSystem'/TurboSequence_Lf/Resources/FXS_TurboSequence_Mesh_Lf.FXS_TurboSequence_Mesh_Lf'");

	inline static const FString NameNiagaraEmitter = FString("FXE_TurboSequence_Mesh_Unit_Lf");
	// Example User.Mesh_0 -> For the first LOD, We have max 32 LODs per system
	inline static const FString NameNiagaraMeshObject = FString("User.Mesh_{0}");
	// Example User.Material_0 -> For the first Material, We have max 15 Materials per system
	inline static const FString NameNiagaraMaterialObject = FString("User.Material_{0}");
	//inline static const FName NameNiagaraParticleSpawnAmount = FName("User.Particle_Spawn_Amount");
	inline static const FString NameNiagaraParticleIDs = FString("User.Particle_ID_To_Index");
	inline static const FString NameNiagaraParticleLocations = FString("User.Particle_Position");
	inline static const FString NameNiagaraParticleRotations = FString("User.Particle_Rotation");
	inline static const FString NameNiagaraParticleScales = FString("User.Particle_Scale");
	// inline static const FName NameNiagaraParticleIndex = FName("User.Particle_Index");
	inline static const FString NameNiagaraParticleRemove = FString("User.Particle_Remove");
	inline static const FString NameNiagaraLevelOfDetailIndex = FString("User.LevelOfDetail_Index");
	inline static const FString NameNiagaraCustomData = FString("User.CustomData");


	inline static const FString ReferenceTurboSequenceTransformTextureCurrentFrame = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_CurrentFrame_Lf.T_TurboSequence_TransformTexture_CurrentFrame_Lf'");
	inline static const FString ReferenceTurboSequenceTransformTexturePreviousFrame = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_PreviousFrame_Lf.T_TurboSequence_TransformTexture_PreviousFrame_Lf'");
	inline static const FString ReferenceTurboSequenceSkinWeightTexture = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_SkinWeightTexture_Lf.T_TurboSequence_SkinWeightTexture_Lf'");
	inline static const FString ReferenceTurboSequenceDataTexture = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_DataTexture_Lf.T_TurboSequence_DataTexture_Lf'");

	static constexpr uint8 NotVisibleMeshIndex = GET13_NUMBER;

	inline static const FName NameMaterialParameterMeshDataTexture = FName("SkinWeight_Texture2DArray");
	inline static const FName NameMaterialParameterMeshDataTextureSizeX = FName("SkinWeight_TexX");
	inline static const FName NameMaterialParameterMeshDataTextureSizeY = FName("SkinWeight_TexY");


	// -> Licence Start
	// Copyright Epic Games, Inc. All Rights Reserved.
	static void GetAnimPoseAtTimeIntervals(const UAnimSequenceBase* AnimationSequenceBase,
	                                       const TArray<float>& TimeIntervals,
	                                       const FAnimPoseEvaluationOptions_Lf& EvaluationOptions,
	                                       TArray<FAnimPose_Lf>& InOutPoses)
	{
		if (AnimationSequenceBase && AnimationSequenceBase->GetSkeleton())
		{
			FMemMark Mark(FMemStack::Get());

			// asset to use for retarget proportions (can be either USkeletalMesh or USkeleton)
			UObject* AssetToUse;
			int32 NumRequiredBones;
			if (EvaluationOptions.OptionalSkeletalMesh)
			{
				AssetToUse = CastChecked<UObject>(EvaluationOptions.OptionalSkeletalMesh);
				NumRequiredBones = EvaluationOptions.OptionalSkeletalMesh->GetRefSkeleton().GetNum();
			}
			else
			{
				AssetToUse = CastChecked<UObject>(AnimationSequenceBase->GetSkeleton());
				NumRequiredBones = AnimationSequenceBase->GetSkeleton()->GetReferenceSkeleton().GetNum();
			}

			TArray<FBoneIndexType> RequiredBoneIndexArray;
			RequiredBoneIndexArray.AddUninitialized(NumRequiredBones);
			for (int32 BoneIndex = 0; BoneIndex < RequiredBoneIndexArray.Num(); ++BoneIndex)
			{
				RequiredBoneIndexArray[BoneIndex] = BoneIndex;
			}

			FBoneContainer RequiredBones;
			RequiredBones.InitializeTo(RequiredBoneIndexArray,
			                           UE::Anim::FCurveFilterSettings(
				                           EvaluationOptions.bEvaluateCurves
					                           ? UE::Anim::ECurveFilterMode::None
					                           : UE::Anim::ECurveFilterMode::DisallowAll), *AssetToUse);

			RequiredBones.SetUseRAWData(EvaluationOptions.EvaluationType == EAnimDataEvalType_Lf::Raw);
			RequiredBones.SetUseSourceData(EvaluationOptions.EvaluationType == EAnimDataEvalType_Lf::Source);

			RequiredBones.SetDisableRetargeting(!EvaluationOptions.bShouldRetarget);

			FCompactPose CompactPose;
			FBlendedCurve Curve;
			UE::Anim::FStackAttributeContainer Attributes;

			FAnimationPoseData PoseData(CompactPose, Curve, Attributes);
			FAnimExtractContext Context(0.0, EvaluationOptions.bExtractRootMotion);

			FCompactPose BasePose;
			BasePose.SetBoneContainer(&RequiredBones);

			CompactPose.SetBoneContainer(&RequiredBones);
			Curve.InitFrom(RequiredBones);

			FAnimPose_Lf Pose;
			Pose.Init(RequiredBones);

			for (int32 Index = 0; Index < TimeIntervals.Num(); ++Index)
			{
				const float EvalInterval = TimeIntervals[Index];

				Context.CurrentTime = EvalInterval;

				FAnimPose_Lf& FramePose = InOutPoses.AddDefaulted_GetRef();
				FramePose = Pose;

				Curve.InitFrom(RequiredBones);

				if (AnimationSequenceBase->IsValidAdditive())
				{
					CompactPose.ResetToAdditiveIdentity();
					AnimationSequenceBase->GetAnimationPose(PoseData, Context);

					if (EvaluationOptions.bRetrieveAdditiveAsFullPose)
					{
						const UAnimSequence* AnimSequence = Cast<const UAnimSequence>(AnimationSequenceBase);

						FBlendedCurve BaseCurve;
						BaseCurve.InitFrom(RequiredBones);
						UE::Anim::FStackAttributeContainer BaseAttributes;

						FAnimationPoseData BasePoseData(BasePose, BaseCurve, BaseAttributes);
						AnimSequence->GetAdditiveBasePose(BasePoseData, Context);

						FAnimationRuntime::AccumulateAdditivePose(BasePoseData, PoseData, 1.f,
						                                          AnimSequence->GetAdditiveAnimType());
						BasePose.NormalizeRotations();

						FramePose.SetPose(BasePoseData);
					}
					else
					{
						FramePose.SetPose(PoseData);
					}
				}
				else
				{
					CompactPose.ResetToRefPose();
					AnimationSequenceBase->GetAnimationPose(PoseData, Context);
					FramePose.SetPose(PoseData);
				}
			}
		}
		else
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Invalid Animation Sequence supplied for GetBonePosesForTime"));
		}
	}

	static FORCEINLINE int32 GetAnimationBonePoseIndex(const FAnimPose_Lf& Pose, const FName& BoneName)
	{
		return Pose.BoneNames.IndexOfByKey(BoneName);
	}

	static const FTurboSequence_TransposeMatrix_Lf& GetAnimationBonePose(
		const FAnimPose_Lf& Pose, const FName& BoneName)
	{
		if (const int32 BoneIndex = Pose.BoneNames.IndexOfByKey(BoneName); BoneIndex != INDEX_NONE)
		{
			return Pose.LocalSpacePoses[BoneIndex];
		}
		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("No bone with name %s was found"), *BoneName.ToString());

		return Pose.LocalSpacePoses[GET0_NUMBER];
	}

	static float GetAnimationCurveWeight(const FAnimPose_Lf& Pose, const FName& CurveName)
	{
		float CurveValue = 0.0f;
		if (int32 CurveIndex = Pose.CurveNames.IndexOfByKey(CurveName); CurveIndex != INDEX_NONE)
		{
			CurveValue = Pose.CurveValues[CurveIndex];
		}

		return CurveValue;
	}

	static FORCEINLINE_DEBUGGABLE void GetPoseInfo(float FrameTime,
	                                               const UAnimSequenceBase* AnimationSequenceBase,
	                                               const FAnimPoseEvaluationOptions_Lf& EvaluationOptions,
	                                               FAnimPose_Lf& OutPose, FCriticalSection& CriticalSection)
	{
		FMemMark Mark(FMemStack::Get());

		UObject* AssetToUse;
		int32 NumRequiredBones;
		if (EvaluationOptions.OptionalSkeletalMesh)
		{
			AssetToUse = CastChecked<UObject>(EvaluationOptions.OptionalSkeletalMesh);
			NumRequiredBones = EvaluationOptions.OptionalSkeletalMesh->GetRefSkeleton().GetNum();
		}
		else
		{
			AssetToUse = CastChecked<UObject>(AnimationSequenceBase->GetSkeleton());
			NumRequiredBones = AnimationSequenceBase->GetSkeleton()->GetReferenceSkeleton().GetNum();
		}

		//CriticalSection.Lock();
		TArray<FBoneIndexType> RequiredBoneIndexArray;
		RequiredBoneIndexArray.AddUninitialized(NumRequiredBones);
		for (int32 BoneIndex = 0; BoneIndex < RequiredBoneIndexArray.Num(); ++BoneIndex)
		{
			RequiredBoneIndexArray[BoneIndex] = BoneIndex;
		}

		FBoneContainer RequiredBones;
		RequiredBones.InitializeTo(RequiredBoneIndexArray,
		                           UE::Anim::FCurveFilterSettings(
			                           EvaluationOptions.bEvaluateCurves
				                           ? UE::Anim::ECurveFilterMode::None
				                           : UE::Anim::ECurveFilterMode::DisallowAll), *AssetToUse);

		RequiredBones.SetUseRAWData(EvaluationOptions.EvaluationType == EAnimDataEvalType_Lf::Raw);
		RequiredBones.SetUseSourceData(EvaluationOptions.EvaluationType == EAnimDataEvalType_Lf::Source);

		RequiredBones.SetDisableRetargeting(!EvaluationOptions.bShouldRetarget);

		FCompactPose CompactPose;
		FBlendedCurve Curve;
		UE::Anim::FStackAttributeContainer Attributes;

		FAnimationPoseData PoseData(CompactPose, Curve, Attributes);
		FAnimExtractContext Context(0.0, EvaluationOptions.bExtractRootMotion);

		FCompactPose BasePose;
		BasePose.SetBoneContainer(&RequiredBones);

		CompactPose.SetBoneContainer(&RequiredBones);
		Curve.InitFrom(RequiredBones);
		//CriticalSection.Unlock();

		Context.CurrentTime = FrameTime;

		OutPose.Init(RequiredBones);

		if (AnimationSequenceBase->IsValidAdditive())
		{
			CompactPose.ResetToAdditiveIdentity();
			AnimationSequenceBase->GetAnimationPose(PoseData, Context);

			if (EvaluationOptions.bRetrieveAdditiveAsFullPose)
			{
				const UAnimSequence* AnimSequence = Cast<const UAnimSequence>(AnimationSequenceBase);

				FBlendedCurve BaseCurve;
				BaseCurve.InitFrom(RequiredBones);
				UE::Anim::FStackAttributeContainer BaseAttributes;

				FAnimationPoseData BasePoseData(BasePose, BaseCurve, BaseAttributes);
				AnimSequence->GetAdditiveBasePose(BasePoseData, Context);

				FAnimationRuntime::AccumulateAdditivePose(BasePoseData, PoseData, 1.f,
				                                          AnimSequence->GetAdditiveAnimType());
				BasePose.NormalizeRotations();

				OutPose.SetPose(BasePoseData);
			}
			else
			{
				OutPose.SetPose(PoseData);
			}
		}
		else
		{
			CompactPose.ResetToRefPose();
			AnimationSequenceBase->GetAnimationPose(PoseData, Context);
			OutPose.SetPose(PoseData);
		}
	}

	// -> Licence End


	static FORCEINLINE_DEBUGGABLE int16 NumCPUThreads()
	{
		static int16 NumThreads = INDEX_NONE;
		if (NumThreads < GET1_NUMBER)
		{
			NumThreads = std::thread::hardware_concurrency();

			if (NumThreads < GET1_NUMBER)
			{
				NumThreads = GET4_NUMBER;
			}
		}

		return NumThreads;
	}

	static FORCEINLINE_DEBUGGABLE FVector4f ConvertQuaternionToVector4F(const FQuat& Quaternion)
	{
		return FVector4f(Quaternion.X, Quaternion.Y, Quaternion.Z, Quaternion.W);
	}

	static FORCEINLINE_DEBUGGABLE FVector4 ConvertVector4FToVector4(const FVector4f& Vector)
	{
		return FVector4(Vector.X, Vector.Y, Vector.Z, Vector.W);
	}

	static FORCEINLINE_DEBUGGABLE FVector3f ConvertVectorToVector3F(const FVector& Vector)
	{
		return FVector3f(Vector.X, Vector.Y, Vector.Z);
	}

	static FORCEINLINE_DEBUGGABLE FVector ConvertVector3FToVector(const FVector3f& Vector)
	{
		return FVector(Vector.X, Vector.Y, Vector.Z);
	}

	template <typename TValue>
	static FORCEINLINE_DEBUGGABLE void CheckArrayHasSize(const TArray<TValue>& Array, uint8 Id)
	{
		if (!Array.Num())
		{
			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Array with ID -> %d is Length 0"), Id);
		}
		// else if (Array.Num() > 1000 && Id != -1)
		// {
		// 	UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Array with ID -> %d is Length Greater than 22000 -> %d"), Id, Array.Num());
		// }
	}


	/**
	 * @brief Clamps a float between 0...1
	 * @param Input The input value which needs to get clamped between 0...1
	 * @return The clamped input vector in a range between 0...1
	 */
	static FORCEINLINE_DEBUGGABLE float Clamp01(float Input)
	{
		return FMath::Clamp(Input, GET0_NUMBER, GET1_NUMBER);
	}

	/**
	 * @brief Get percentage values between min and max in a 0 ... 1 range
	 * @param Current The current value
	 * @param Min The min value
	 * @param Max The max value which needs to be greater than the min value
	 * @return A percentage between 0 ... 1
	 */
	static constexpr FORCEINLINE_DEBUGGABLE float GetPercentageBetweenMinMax(float Current, float Min,
	                                                                         float Max)
	{
		if (Min == Max)
		{
			return GET1_NUMBER;
		}
		return (Current - Min) / (Max - Min);
	}


	/**
	 * @brief Create a simple subtraction between A and B for double and float FVectors
	 * @param A The a double FVector
	 * @param B The b float FVector
	 * @return The product of A minus B in float values
	 */
	static FORCEINLINE_DEBUGGABLE FVector3f SubtractVector(const FVector& A, const FVector& B)
	{
		// Return a simple subtraction
		return FVector3f(A.X - B.X, A.Y - B.Y, A.Z - B.Z);
	}

	/**
	 * @brief Checks if a point is inside a sphere based on the sphere location and radius
	 * @param SphereLocation The location of the sphere in world space
	 * @param SphereRadius The radius of the sphere
	 * @param Point The point which is the location of an FVector in world space
	 * @return The result as a bool which returns true when the point is inside the sphere otherwise false when the point is outside the sphere
	 */
	static FORCEINLINE_DEBUGGABLE bool IsPointInsideSphere(const FVector& SphereLocation, float SphereRadius,
	                                                       const FVector& Point)
	{
		return SubtractVector(Point, SphereLocation).Length() <= SphereRadius;
	}


	static FORCEINLINE_DEBUGGABLE void GetCameraFrustumPlanes_ObjectSpace(FPlane (&Out_ObjectSpace_Planes)[GET6_NUMBER],
	                                                                      float Fov, const FVector2f ViewportSize,
	                                                                      const TOptional<EAspectRatioAxisConstraint>
	                                                                      InAspectRatioAxisConstraint,
	                                                                      float NearClippingPlane,
	                                                                      float FarClippingPlane,
	                                                                      bool bOrthographicModeEnabled,
	                                                                      float OrthographicWidth)
	{
		// Object space is better so we can hardcode a lot more
		const FVector Direction = FVector::ForwardVector;
		const FVector LeftVector = FVector::RightVector;
		const FVector UpVector = FVector::UpVector;


		const float HozHalfAngleInRadians = FMath::DegreesToRadians(Fov * 0.5f);

		// Determine FOV multipliers to match render target's aspect ratio
		float HozLength;
		float VertLength;
		float AspectRatio;
		if (((ViewportSize.X > ViewportSize.Y) && (InAspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)) || (
			InAspectRatioAxisConstraint == AspectRatio_MaintainXFOV))
		{
			//if the viewport is wider than it is tall
			AspectRatio = ViewportSize.X / static_cast<float>(ViewportSize.Y);
		}
		else
		{
			//if the viewport is taller than it is wide
			AspectRatio = ViewportSize.Y / static_cast<float>(ViewportSize.X);
		}

		if (bOrthographicModeEnabled)
		{
			HozLength = OrthographicWidth * 0.5f;
			VertLength = HozLength / AspectRatio;
		}
		else
		{
			if (((ViewportSize.X > ViewportSize.Y) && (InAspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)) || (
				InAspectRatioAxisConstraint == AspectRatio_MaintainXFOV))
			{
				// Calculate lengths based on the chosen FOV
				HozLength = NearClippingPlane * FMath::Tan(HozHalfAngleInRadians);
				VertLength = HozLength / AspectRatio;
			}
			else
			{
				// Calculate lengths based on the chosen FOV
				VertLength = NearClippingPlane * FMath::Tan(HozHalfAngleInRadians);
				HozLength = VertLength / AspectRatio;
			}
		}

		// near plane verts
		const FVector NearRTopEdge = Direction * NearClippingPlane + UpVector * VertLength + LeftVector * HozLength;
		const FVector NearLTopEdge = Direction * NearClippingPlane + UpVector * VertLength - LeftVector * HozLength;
		const FVector NearLBottomEdge = Direction * NearClippingPlane - UpVector * VertLength - LeftVector * HozLength;
		const FVector NearRBottomEdge = Direction * NearClippingPlane - UpVector * VertLength + LeftVector * HozLength;

		if (!bOrthographicModeEnabled)
		{
			if (((ViewportSize.X > ViewportSize.Y) && (InAspectRatioAxisConstraint == AspectRatio_MajorAxisFOV)) || (
				InAspectRatioAxisConstraint == AspectRatio_MaintainXFOV))
			{
				// Calculate lengths based on the chosen FOV
				HozLength = FarClippingPlane * FMath::Tan(HozHalfAngleInRadians);
				VertLength = HozLength / AspectRatio;
			}
			else
			{
				// Calculate lengths based on the chosen FOV
				VertLength = FarClippingPlane * FMath::Tan(HozHalfAngleInRadians);
				HozLength = VertLength / AspectRatio;
			}
		}

		// far plane verts
		const FVector RTopEdge = Direction * FarClippingPlane + UpVector * VertLength + LeftVector * HozLength;
		const FVector LTopEdge = Direction * FarClippingPlane + UpVector * VertLength - LeftVector * HozLength;
		const FVector LBottomEdge = Direction * FarClippingPlane - UpVector * VertLength - LeftVector * HozLength;
		const FVector RBottomEdge = Direction * FarClippingPlane - UpVector * VertLength + LeftVector * HozLength;

		if (bOrthographicModeEnabled)
		{
			Out_ObjectSpace_Planes[GET0_NUMBER] = FPlane(NearRTopEdge, RTopEdge, LTopEdge); // Top plane
			Out_ObjectSpace_Planes[GET1_NUMBER] = FPlane(NearLBottomEdge, LBottomEdge, RBottomEdge); // Bottom plane
			Out_ObjectSpace_Planes[GET2_NUMBER] = FPlane(NearRBottomEdge, RBottomEdge, RTopEdge); // Right plane
			Out_ObjectSpace_Planes[GET3_NUMBER] = FPlane(NearLTopEdge, LTopEdge, LBottomEdge); // Left plane

			Out_ObjectSpace_Planes[GET4_NUMBER] = FPlane(Direction * NearClippingPlane, Direction); // Near plane
			Out_ObjectSpace_Planes[GET5_NUMBER] = FPlane(Direction * FarClippingPlane, -Direction); // Far plane
		}
		else
		{
			Out_ObjectSpace_Planes[GET0_NUMBER] = FPlane(FVector::ZeroVector, RTopEdge, LTopEdge); // Top plane
			Out_ObjectSpace_Planes[GET1_NUMBER] = FPlane(FVector::ZeroVector, LBottomEdge, RBottomEdge); // Bottom plane
			Out_ObjectSpace_Planes[GET2_NUMBER] = FPlane(FVector::ZeroVector, RBottomEdge, RTopEdge); // Right plane
			Out_ObjectSpace_Planes[GET3_NUMBER] = FPlane(FVector::ZeroVector, LTopEdge, LBottomEdge); // Left plane

			Out_ObjectSpace_Planes[GET4_NUMBER] = FPlane(Direction * NearClippingPlane, Direction); // Near plane
			Out_ObjectSpace_Planes[GET5_NUMBER] = FPlane(Direction * FarClippingPlane, -Direction); // Far plane	
		}
	}

	/**
	 * @brief Checks if a point intersects with the camera frustum
	 * @param Point The point we like to check
	 * @param ObjectSpace_PlanesInput The camera frustum planes in object space, you get it with GetCameraFrustumPlanes_ObjectSpace
	 * @param CameraTransform The camera transform
	 * @return true if the point intersects with the camera | false if the point is outside the camera view
	 */
	static FORCEINLINE_DEBUGGABLE bool Point_Intersects_With_Frustum(const FVector& Point,
	                                                                 const FPlane (&ObjectSpace_PlanesInput)[
		                                                                 GET6_NUMBER],
	                                                                 const FTransform& CameraTransform)
	{
		const FVector InversePoint = CameraTransform.InverseTransformPosition(Point);

		for (uint8 p = GET0_NUMBER; p < GET6_NUMBER; ++p)
		{
			if (ObjectSpace_PlanesInput[p].PlaneDot(InversePoint) < GET0_NUMBER)
			{
				return false;
			}
		}

		return true;
	}


	/**
	 * @brief Checks if a box intersects with the camera frustum
	 * @param Box The box shape we like to check
	 * @param ObjectSpace_PlanesInput The camera frustum planes in object space, you get it with GetCameraFrustumPlanes_ObjectSpace
	 * @param CameraTransform The camera transform
	 * @param IntersectionToleranceRadius The tolerance radius which says if ( IsBoxCenterInsideCameraRadius )
	 * @return true if the box intersects with the camera | false if the box is fully outside the camera view
	 */
	static FORCEINLINE_DEBUGGABLE bool Box_Intersects_With_Frustum(const FBox& Box,
	                                                               const FPlane (&ObjectSpace_PlanesInput)[GET6_NUMBER],
	                                                               const FTransform& CameraTransform,
	                                                               float IntersectionToleranceRadius =
		                                                               GET0_NUMBER)
	{
		const FVector& CameraLocation = CameraTransform.GetLocation();

		// When the Box Center and CameraLocation is IntersectionToleranceRadius we want count it too as
		// Intersection, because if the Fov is interpolated on the Camera Planes we get different fov
		// on near range and this is solving clipping issues in this case
		if (IsPointInsideSphere(Box.GetCenter(), IntersectionToleranceRadius, CameraLocation))
		{
			return true;
		}

		// When the Camera is inside the box we count it as Intersecting
		if (Box.IsInside(CameraLocation))
		{
			return true;
		}

		// Sometimes the Box is so bounds is that huge to not fit inside the camera frustum,
		// here we assume the bounds is at the same rotation than the camera which mean,
		// it would choose the shortest path to reach the camera which is what we want,
		// and it would work with large bounces too
		if (Point_Intersects_With_Frustum(Box.GetClosestPointTo(CameraLocation), ObjectSpace_PlanesInput,
		                                  CameraTransform))
		{
			return true;
		}

		// Get the verts of the box
		FVector Verts[GET8_NUMBER];
		Box.GetVertices(Verts);
		// Loop through every vert and check if the vert is intersecting with the camera
		for (uint8 Vert = GET0_NUMBER; Vert < GET8_NUMBER; ++Vert)
		{
			// When it's intersecting return true
			if (Point_Intersects_With_Frustum(Verts[Vert], ObjectSpace_PlanesInput, CameraTransform))
			{
				return true;
			}
		}

		// Return false if there is no intersection happening
		return false;
	}

	static FORCEINLINE_DEBUGGABLE void OrthoNormalizeMatrix(FMatrix& Matrix)
	{
		FVector XAxis = Matrix.GetScaledAxis(EAxis::X);
		FVector YAxis = Matrix.GetScaledAxis(EAxis::Y);
		//FVector ZAxis = Matrix.GetScaledAxis(EAxis::Z);

		// Ensure that the basis vectors are orthogonal
		XAxis.Normalize();
		FVector ZAxis = FVector::CrossProduct(XAxis, YAxis);
		ZAxis.Normalize();
		YAxis = FVector::CrossProduct(ZAxis, XAxis);
		YAxis.Normalize();

		// Update the input matrix with the ortho-normalized basis vectors
		Matrix.SetAxis(GET0_NUMBER, XAxis);
		Matrix.SetAxis(GET1_NUMBER, YAxis);
		Matrix.SetAxis(GET2_NUMBER, ZAxis);
	}

	static FORCEINLINE_DEBUGGABLE void NormalizeMatrix(FMatrix& Matrix)
	{
		Matrix.SetAxis(GET0_NUMBER, Matrix.GetScaledAxis(EAxis::X).GetSafeNormal());
		Matrix.SetAxis(GET1_NUMBER, Matrix.GetScaledAxis(EAxis::Y).GetSafeNormal());
		Matrix.SetAxis(GET2_NUMBER, Matrix.GetScaledAxis(EAxis::Z).GetSafeNormal());
	}

	static FORCEINLINE FQuat VectorAccumulateQuaternionShortestPath(const FQuat& A, const FQuat& B)
	{
		// Ensure both quaternions are normalized
		// A = normalize(A);
		// B = normalize(B);

		// Calculate the dot product
		float dotProduct = A | B;

		// Determine the sign of the dot product
		float sign = (dotProduct >= 0.0) ? 1.0 : -1.0;

		// Blend rotation
		return A + sign * B;
	}

	static FORCEINLINE FQuat Scale_Quaternion(const FQuat& Q1, const FQuat& Q2, float T)
	{
		const FQuat& QScaled = Q2 * T;

		return VectorAccumulateQuaternionShortestPath(Q1, QScaled);
	}


	template <typename TKey>
	static FORCEINLINE_DEBUGGABLE TObjectPtr<UObject> SetOrAdd(TMap<TKey, TObjectPtr<UObject>>& Map,
	                                                           const TObjectPtr<UObject>& Item, const TKey& Key)
	{
		TObjectPtr<UObject>& Result = Map.FindOrAdd(Key, Item);
		Result = Item;
		return Item;
	}

	template <typename TKey, typename TValue, typename Function>
	static FORCEINLINE_DEBUGGABLE TTuple<TKey, TValue> FindByPredicate(const TMap<TKey, TValue>& Map,
	                                                                   const Function& Predicate)
	{
		for (const TTuple<TKey, TValue> Item : Map)
		{
			if (::Invoke(Predicate, Item))
			{
				return Item;
			}
		}

		return TTuple<TKey, TValue>(TKey(), TValue());
	}

	static FORCEINLINE_DEBUGGABLE uint32 EncodeUInt32ToColor(const FColor& Color)
	{
		// return (float)(((int)(Color.x) << 24) | ((int)(Color.y) << 16) | ((int)(Color.z) <<8) | (int)Color.w);
		return (Color.R << GET24_NUMBER) | (Color.G << GET16_NUMBER) | (Color.B << GET8_NUMBER) | Color.A;
	}

	static FORCEINLINE_DEBUGGABLE FColor DecodeUInt32ToColor(uint32 Number)
	{
		FColor Color;

		Color.R = (Number >> GET24_NUMBER) & 0xFF;
		Color.G = (Number >> GET16_NUMBER) & 0xFF;
		Color.B = (Number >> GET8_NUMBER) & 0xFF;
		Color.A = Number & 0xFF;

		return Color;
	}

	static FORCEINLINE_DEBUGGABLE uint16 EncodeUInt16ToUInt8Vector2(const FIntVector2& Int8Vector)
	{
		return (Int8Vector.Y << GET8_NUMBER) | Int8Vector.X;
	}

	static FORCEINLINE_DEBUGGABLE FIntVector2 DecodeUInt16ToUInt8Vector2(uint16 Number)
	{
		FIntVector2 Vector;

		Vector.Y = (Number >> GET8_NUMBER) & 0xFF;
		Vector.X = Number & 0xFF;

		return Vector;
	}

	static FORCEINLINE_DEBUGGABLE FIntVector2 DecodeUInt32ToUInt16(int32 PackedValue)
	{
		// X, Y
		return FIntVector2(PackedValue % 0xFFFF,
		                   PackedValue / 0xFFFF);
	}

	static FORCEINLINE_DEBUGGABLE uint32 EncodeUInt16ToUInt32(uint16 XValue, uint16 YValue)
	{
		return YValue * 0xFFFF + XValue;
	}

	/**
 * @brief Sort a TMap with an predicate
 * @tparam Key The key type of the TMap
 * @tparam Pair The value type of the TMap
 * @tparam Function The predicate function to sort the TMap
 * @param InMap The TMap you like to sort
 * @param Predicate The sort function -> [](const int32 A, const int32 B) { return A < B; }
 */
	template <class Key, class Pair, typename Function>
	static FORCEINLINE_DEBUGGABLE void SortTMapByPredicate(TMap<Key, Pair>& InMap, const Function& Predicate)
	{
		// Create an array with the same type as TPair<Key, Value>
		TArray<TPair<Key, Pair>> Pairs;
		// Allocate the temp array
		Pairs.SetNum(InMap.Num());
		// Add an index for the loop
		int32 Index = 0;
		// Fill the temp array
		for (const TPair<Key, Pair>& PairValue : InMap)
		{
			// Assign the values
			Pairs[Index] = PairValue;
			// Increment the index
			Index++;
		}

		// Sort the array
		Algo::Sort(Pairs, Predicate);

		// Clear the current map and rebuild it
		InMap.Empty();
		// Add the sorted array to the TMap
		for (const TPair<Key, Pair>& PairValue : Pairs)
		{
			InMap.Add(PairValue.Key, PairValue.Value);
		}
	}

	/**
	 * @brief Checks if the TMap contains an item by predicate | note: use TMap.Contains() for key searching
	 * @tparam Key The key type of the TMap
	 * @tparam Pair The value type of the TMap
	 * @tparam Function The predicate function for finding the containing item
	 * @param InMap The TMap you like to look up
	 * @param Predicate The function to invoke the check -> []( const int32 item ) { return item == 0; }
	 * @return returns true if the item was found, false if the item does is not containing in the TMap
	 */
	template <class Key, class Pair, typename Function>
	static FORCEINLINE_DEBUGGABLE bool ContainsTMapByPredicateInPairs(TMap<Key, Pair>& InMap, const Function& Predicate)
	{
		// Loop through all items in the map
		for (const TTuple<Key, Pair>& Element : InMap)
		{
			// If this item match with the predicate return true
			if (::Invoke(Predicate, Element.Value))
			{
				return true;
			}
		}
		// Otherwise if this map does not contain the item return false
		return false;
	}


	template <class Key1, class Key2>
	static FORCEINLINE_DEBUGGABLE uint32 GetDoubleMapKey(const TObjectPtr<Key1>& Asset1, const TObjectPtr<Key2>& Asset2)
	{
		uint32 Hash = GET0_NUMBER;
		Hash = HashCombineFast(Hash, GetTypeHash(Asset1));
		Hash = HashCombineFast(Hash, GetTypeHash(Asset2));
		return Hash;
	}

	template <class Key1, class Key2, class Key3>
	static FORCEINLINE_DEBUGGABLE uint32 GetTripleMapKey(const TObjectPtr<Key1>& Asset1, const TObjectPtr<Key2>& Asset2,
	                                                     const TObjectPtr<Key3>& Asset3)
	{
		uint32 Hash = GET0_NUMBER;
		Hash = HashCombineFast(Hash, GetTypeHash(Asset1));
		Hash = HashCombineFast(Hash, GetTypeHash(Asset2));
		Hash = HashCombineFast(Hash, GetTypeHash(Asset3));
		return Hash;
	}

	template <class T>
	static FORCEINLINE_DEBUGGABLE uint32 GetArrayHash(const TArray<TObjectPtr<T>>& Assets)
	{
		uint32 Hash = GET0_NUMBER;
		for (const TObjectPtr<T>& Element : Assets)
		{
			Hash = HashCombineFast(Hash, GetTypeHash(Element));
		}
		return Hash;
	}

	template <class T>
	static FORCEINLINE_DEBUGGABLE uint32 HashDataType(const T& DataValue)
	{
		FHashBuilder HashBuilder;
		HashBuilder << DataValue;
		return HashBuilder.GetHash(); // Get the resulting hash value
	}

	template <class T>
	static FORCEINLINE_DEBUGGABLE void HashDataTypeToHash(uint32& Hash, const T& DataValue)
	{
		Hash = HashCombineFast(Hash, HashDataType(DataValue));
	}

	template <class Key, class Value>
	static FORCEINLINE_DEBUGGABLE uint32 GetDataMapHash(TMap<Key, Value>& Map)
	{
		uint32 Hash = GET0_NUMBER;
		for (const TTuple<Key, Value>& Element : Map)
		{
			HashDataTypeToHash(Hash, Element.Key);
			HashDataTypeToHash(Hash, Element.Value);
		}

		return Hash;
	}


	/**
 * @brief Creates a buffer with the wanted size and format
 * @tparam T The type of the upload buffer, as example: for a float4 in the shader you need a FVector4f
 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
 * @param Size The length of the buffer array
 * @param BufferName The name of the buffer for debugging
 * @param Format The PixelFormat which is PF_R32_FLOAT for a float array and PF_FloatRGBA for a float4 array in the shader, use this format you need
 * @param OutputBuffer The input needs a empty buffer reference and outputs a valid buffer reference, usually really important for more operations on this data stream
 * @return returns the final pass parameter to use it for your shader allocation as unordered access view
 */
	template <class T>
	static FORCEINLINE_DEBUGGABLE FRDGBufferUAVRef TCreateWriteBuffer(FRDGBuilder& GraphBuilder, int32 Size,
	                                                                  const TCHAR* BufferName,
	                                                                  const EPixelFormat& Format,
	                                                                  FRDGBufferRef& OutputBuffer)
	{
		// Create the buffer and a description with the wanted type
		OutputBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateBufferDesc(sizeof(T), Size), BufferName);

		// Create the unordered access view and return it
		return GraphBuilder.CreateUAV(FRDGBufferUAVDesc(OutputBuffer, Format),
		                              ERDGUnorderedAccessViewFlags::SkipBarrier);
	}

	/**
	 * @brief Creates a buffer with the wanted size and format
	 * @tparam T The type of the upload buffer, as example: for a float4 in the shader you need a FVector4f
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param Size The length of the buffer array
	 * @param BufferName The name of the buffer for debugging
	 * @param Format The PixelFormat which is PF_R32_FLOAT for a float array and PF_FloatRGBA for a float4 array in the shader, use this format you need
	 * @return returns the final pass parameter to use it for your shader allocation as unordered access view
	 */
	template <class T>
	static FORCEINLINE_DEBUGGABLE FRDGBufferUAVRef TCreateWriteBuffer(FRDGBuilder& GraphBuilder, int32 Size,
	                                                                  const TCHAR* BufferName,
	                                                                  const EPixelFormat& Format)
	{
		FRDGBufferRef OutputBuffer;
		return TCreateWriteBuffer<T>(GraphBuilder, Size, BufferName, Format, OutputBuffer);
	}


	/**
	 * @brief Creates a texture which the shader can write into it in a PF_FloatRGBA format which mean the data is represented as float4 inside the shader
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param OutputTextureRef The input needs a empty buffer reference and outputs a valid buffer reference, usually useful for making more operations with the data 
	 * @param SizeX The size width of the texture in pixels
	 * @param SizeY The size height of the texture in pixels
	 * @param TextureName The name of the texture for debugging
	 * @return returns the final pass parameter to use it for your shader allocation as unordered access view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGTextureUAVRef CreateWriteTexture_Half4_Out(FRDGBuilder& GraphBuilder,
		FRDGTextureRef& OutputTextureRef,
		int32 SizeX, int32 SizeY,
		const TCHAR* TextureName, const FStaticShaderPlatform& ShaderPlatform)
	{
		return CreateWriteTexture_Custom_Out(GraphBuilder, OutputTextureRef, SizeX, SizeY, TextureName, PF_FloatRGBA,
		                                     ShaderPlatform);
	}

	static FORCEINLINE_DEBUGGABLE FRDGTextureUAVRef CreateWriteTexture_Custom_Out(FRDGBuilder& GraphBuilder,
		FRDGTextureRef& OutputTextureRef,
		int32 SizeX, int32 SizeY,
		const TCHAR* TextureName,
		const EPixelFormat& Format, const FStaticShaderPlatform& ShaderPlatform)
	{
		ETextureCreateFlags TexCreateFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV;


		// Create the texture description
		// here we straight define PF_FloatRGBA to have a float4 value in the shader
		const FRDGTextureDesc OutTextureDescription = FRDGTextureDesc::Create2D(
			FIntPoint(SizeX, SizeY),
			Format,
			FClearValueBinding::Transparent,
			TexCreateFlags,
			GET1_NUMBER,
			GET1_NUMBER);
		// Allocate the texture
		OutputTextureRef = GraphBuilder.CreateTexture(OutTextureDescription, TextureName);
		// Create the unordered access view and return it
		return GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTextureRef));
	}

	/**
	 * @brief Creates a texture which the shader can write into it in a PF_FloatRGBA format which mean the data is represented as float4 inside the shader
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param SizeX The size width of the texture in pixels
	 * @param SizeY The size height of the texture in pixels
	 * @param TextureName The name of the texture for debugging
	 * @return returns the final pass parameter to use it for your shader allocation as unordered access view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGTextureUAVRef CreateWriteTexture_Half4_Out(
		FRDGBuilder& GraphBuilder, int32 SizeX,
		int32 SizeY, const TCHAR* TextureName, const FStaticShaderPlatform& ShaderPlatform)
	{
		FRDGTextureRef OutputTextureRef;
		return CreateWriteTexture_Half4_Out(GraphBuilder, OutputTextureRef, SizeX, SizeY, TextureName, ShaderPlatform);
	}


	static FORCEINLINE_DEBUGGABLE FRDGTextureUAVRef CreateWriteTextureArray_Custom_Out(
		FRDGBuilder& GraphBuilder, FRDGTextureRef& OutputTextureRef, int32 SizeX, int32 SizeY,
		int32 Slice, const TCHAR* TextureName, const EPixelFormat& Format)
	{
		ETextureCreateFlags TexCreateFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV;

		// if (!IsVulkanPlatform(ShaderPlatform))
		// {
		// 	TexCreateFlags |= TexCreate_ReduceMemoryWithTilingMode;
		// }


		const FRDGTextureDesc OutTextureDescription = FRDGTextureDesc::Create2DArray(
			FIntPoint(SizeX, SizeY),
			Format,
			FClearValueBinding::Transparent,
			TexCreateFlags,
			Slice, GET1_NUMBER, GET1_NUMBER);

		OutputTextureRef = GraphBuilder.CreateTexture(OutTextureDescription, TextureName);

		return GraphBuilder.CreateUAV(FRDGTextureUAVDesc(OutputTextureRef));
	}

	// static FORCEINLINE_DEBUGGABLE auto CreateWriteTextureArray_Half4_Out(FRDGBuilder& GraphBuilder, int32 SizeX, int32 SizeY, int32 Slice, const TCHAR* TextureName)
	// {
	// 	FRDGTextureRef OutputTextureRef;
	// 	return CreateWriteTextureArray_Custom_Out(GraphBuilder, OutputTextureRef, SizeX, SizeY, Slice, TextureName, PF_FloatRGBA);
	// }


	/**
		 * @brief Creates a read only buffer which using data from the cpu inside the shader, the format inside the gpu is defined custom with the format parameter
		 * @tparam T The type of data we want to push inside the shader, when we specify a FVector4f we have a float4 array inside the shader when we choose PF_FloatRGBA as format
		 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
		 * @param Array The array we want push inside the shader, so that we can access the data inside the shader
		 * @param BufferName The name of the buffer for debugging
		 * @param Format The PixelFormat which is PF_R32_FLOAT for a float array and PF_FloatRGBA for a float4 array in the shader, use this format you need
		 * @param NoCopy
		 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
		 */
	template <class T>
	static FORCEINLINE_DEBUGGABLE FRDGBufferSRVRef TCreateStructuredReadBufferFromTArray_Custom_Out(
		FRDGBuilder& GraphBuilder, const TArray<T>& Array, const TCHAR* BufferName, const EPixelFormat& Format,
		bool NoCopy = false)
	{
		FRDGBufferRef OutputBuffer;
		return TCreateStructuredReadBufferFromTArray_Custom_Out(GraphBuilder, Array, OutputBuffer, BufferName, Format,
		                                                        NoCopy);
	}


	/**
	 * @brief Creates a read only buffer which using data from the cpu inside the shader, the format inside the gpu is defined custom with the format parameter
	 * @tparam T The type of data we want to push inside the shader, when we specify a FVector4f we have a float4 array inside the shader when we choose PF_FloatRGBA as format
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param Array The array we want push inside the shader, so that we can access the data inside the shader
	 * @param OutputBuffer The input needs a empty buffer reference and outputs a valid buffer reference, usually useful for making more operations with the data 
	 * @param BufferName The name of the buffer for debugging
	 * @param Format The PixelFormat which is PF_R32_FLOAT for a float array and PF_FloatRGBA for a float4 array in the shader, use this format you need
	 * @param NoCopy
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	 */
	template <class T>
	static FORCEINLINE_DEBUGGABLE FRDGBufferSRVRef TCreateStructuredReadBufferFromTArray_Custom_Out(
		FRDGBuilder& GraphBuilder, const TArray<T>& Array, FRDGBufferRef& OutputBuffer, const TCHAR* BufferName,
		const EPixelFormat& Format, bool NoCopy = false)
	{
		// Get the number of inputs from the array
		const int32 NumInputs = Array.Num();
		// Get the size of the type we want upload
		constexpr int32 InputSize = sizeof(T);
		// Create the structured buffer which is a buffer type which can use data from the cpu
		OutputBuffer = CreateStructuredBuffer(GraphBuilder, BufferName, InputSize, NumInputs, Array.GetData(),
		                                      InputSize * NumInputs,
		                                      NoCopy ? ERDGInitialDataFlags::NoCopy : ERDGInitialDataFlags::None);


		// Create the shader resource view and return it
		return GraphBuilder.CreateSRV(FRDGBufferSRVDesc(OutputBuffer, Format));
	}

	/**
	 * @brief Creates a read only buffer which using data from the cpu inside the shader, the format inside the gpu is float4
	 * @tparam T The type of data we want to push inside the shader, when we specify a FVector4f we have a float4 array inside the shader
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param Array The array we want push inside the shader, so that we can access the data inside the shader
	 * @param OutputBuffer The input needs a empty buffer reference and outputs a valid buffer reference, usually useful for making more operations with the data 
	 * @param BufferName The name of the buffer for debugging
	 * @param NoCopy
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	*/
	static FORCEINLINE_DEBUGGABLE FRDGBufferSRVRef CreateStructuredReadBufferFromTArray_Half4_Out(
		FRDGBuilder& GraphBuilder, const TArray<FVector4f>& Array, FRDGBufferRef& OutputBuffer, const TCHAR* BufferName,
		bool NoCopy = false)
	{
		return TCreateStructuredReadBufferFromTArray_Custom_Out<FVector4f>(
			GraphBuilder, Array, OutputBuffer, BufferName, PF_FloatRGBA, NoCopy); // PF_FloatRGBA -> 16 bit
	}

	/**
	 * @brief Creates a read only buffer which using data from the cpu inside the shader, the format inside the gpu is float4
	 * @tparam T The type of data we want to push inside the shader, when we specify a FVector4f we have a float4 array inside the shader
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param Array The array we want push inside the shader, so that we can access the data inside the shader
	 * @param BufferName The name of the buffer for debugging
	 * @param NoCopy
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	*/
	static FORCEINLINE_DEBUGGABLE FRDGBufferSRVRef CreateStructuredReadBufferFromTArray_Half4_Out(
		FRDGBuilder& GraphBuilder, const TArray<FVector4f>& Array, const TCHAR* BufferName, bool NoCopy = false)
	{
		FRDGBufferRef OutputBuffer;
		return CreateStructuredReadBufferFromTArray_Half4_Out(GraphBuilder, Array, OutputBuffer, BufferName, NoCopy);
	}


	/**
	 * @brief Creates a read only texture which using a render target texture as input and uploads the data into the gpu, the format inside the gpu is float4
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param PooledRenderTarget Sometimes PooledRenderTarget is useful for copy a texture after rdg executes, to have this option we need to define PooledRenderTarget outside the scope
	 * @param OutputTextureRef The input needs a empty texture reference and outputs a valid texture reference, usually useful for making more operations with the data
	 * @param Texture The actual render target texture you like to upload into the gpu
	 * @param TextureName The name of the texture for debugging
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGTextureSRVRef CreateReadRenderTargetTexture_Half4_Out(
		FRDGBuilder& GraphBuilder, TRefCountPtr<IPooledRenderTarget>& PooledRenderTarget,
		FRDGTextureRef& OutputTextureRef, UTextureRenderTarget2D& Texture, const TCHAR* TextureName,
		const FStaticShaderPlatform& ShaderPlatform)
	{
		// Get the render target resource
		const FRenderTarget* RenderTargetResource = Texture.GetRenderTargetResource();
		// Get the RHI texture from the resource
		const FTexture2DRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();
		// Create the description of the upload,
		// here it's important to use TexCreate_RenderTargetable and TexCreate_ShaderResource

		ETextureCreateFlags TexCreateFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV;


		const FPooledRenderTargetDesc RenderTargetDescription = FPooledRenderTargetDesc::Create2DDesc(
			RenderTargetResource->GetSizeXY(),
			PF_FloatRGBA,
			FClearValueBinding::Transparent,
			TexCreate_None,
			TexCreateFlags,
			false);

		// Create the render target item
		FSceneRenderTargetItem RenderTargetItem;
		// Assign the RHI texture into the render target item
		RenderTargetItem.TargetableTexture = RenderTargetRHI;
		RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;
		// This part connects the render target with the PooledRenderTarget because PooledRenderTarget can have 2 textures sometimes for the MIPs
		GRenderTargetPool.CreateUntrackedElement(RenderTargetDescription, PooledRenderTarget, RenderTargetItem);
		// Allocate the texture
		OutputTextureRef = GraphBuilder.RegisterExternalTexture(PooledRenderTarget, TextureName);

		// Create the shader resource view and return it
		return GraphBuilder.CreateSRV(FRDGTextureSRVDesc(OutputTextureRef));
	}

	static FORCEINLINE_DEBUGGABLE FRDGTextureSRVRef CreateReadRenderTargetArrayTexture_Half4_Out(
		FRDGBuilder& GraphBuilder, UTextureRenderTarget2DArray& Texture, const TCHAR* TextureName)
	{
		TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
		FRDGTextureRef OutputTextureRef;
		return CreateReadRenderTargetArrayTexture_Custom_Out(GraphBuilder, PooledRenderTarget, OutputTextureRef,
		                                                     Texture, TextureName, PF_FloatRGBA);
	}

	static FORCEINLINE_DEBUGGABLE FRDGTextureSRVRef CreateReadRenderTargetArrayTexture_Custom_Out(
		FRDGBuilder& GraphBuilder, TRefCountPtr<IPooledRenderTarget>& PooledRenderTarget,
		FRDGTextureRef& OutputTextureRef, UTextureRenderTarget2DArray& Texture, const TCHAR* TextureName,
		const EPixelFormat& Format)
	{
		// Get the render target resource
		const FRenderTarget* RenderTargetResource = Texture.GetRenderTargetResource();
		// Get the RHI texture from the resource
		const FTexture2DRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();
		// Create the description of the upload,
		// here it's important to use TexCreate_RenderTargetable and TexCreate_ShaderResource

		ETextureCreateFlags TexCreateFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV;


		const FPooledRenderTargetDesc RenderTargetDescription = FPooledRenderTargetDesc::Create2DArrayDesc(
			RenderTargetResource->GetSizeXY(),
			Format,
			FClearValueBinding::Transparent,
			TexCreate_None,
			TexCreateFlags,
			false,
			Texture.Slices);

		// Create the render target item
		FSceneRenderTargetItem RenderTargetItem;
		// Assign the RHI texture into the render target item
		RenderTargetItem.TargetableTexture = RenderTargetRHI;
		RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;
		// This part connects the render target with the PooledRenderTarget because PooledRenderTarget can have 2 textures sometimes for the MIPs
		GRenderTargetPool.CreateUntrackedElement(RenderTargetDescription, PooledRenderTarget, RenderTargetItem);
		// Allocate the texture
		OutputTextureRef = GraphBuilder.RegisterExternalTexture(PooledRenderTarget, TextureName);

		// Create the shader resource view and return it
		return GraphBuilder.CreateSRV(FRDGTextureSRVDesc(OutputTextureRef));
	}

	static FORCEINLINE_DEBUGGABLE TRefCountPtr<IPooledRenderTarget> CreateRenderTarget2DArray(
		FRHITexture* Texture, const TCHAR* Name, uint16 Slice, const FStaticShaderPlatform& ShaderPlatform)
	{
		check(Texture);

		FSceneRenderTargetItem Item;
		Item.TargetableTexture = Texture;
		Item.ShaderResourceTexture = Texture;


		ETextureCreateFlags TexCreateFlags = TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_UAV;


		const FPooledRenderTargetDesc Desc = FPooledRenderTargetDesc::Create2DArrayDesc(
			Texture->GetSizeXY(),
			PF_FloatRGBA,
			FClearValueBinding::Transparent,
			TexCreate_None,
			TexCreateFlags,
			false,
			Slice);

		TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
		GRenderTargetPool.CreateUntrackedElement(Desc, PooledRenderTarget, Item);
		return MoveTemp(PooledRenderTarget);
	}

	/**
	 * @brief Creates a read only texture which using a render target texture as input and uploads the data into the gpu, the format inside the gpu is float4
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param PooledRenderTarget Sometimes PooledRenderTarget is useful for copy a texture after rdg executes, to have this option we need to define PooledRenderTarget outside the scope
	 * @param Texture The actual render target texture you like to upload into the gpu
	 * @param TextureName The name of the texture for debugging
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGTextureSRVRef CreateReadRenderTargetTexture_Half4_Out(
		FRDGBuilder& GraphBuilder, TRefCountPtr<IPooledRenderTarget>& PooledRenderTarget,
		UTextureRenderTarget2D& Texture, const TCHAR* TextureName, const FStaticShaderPlatform& ShaderPlatform)
	{
		FRDGTextureRef OutputTextureRef;
		return CreateReadRenderTargetTexture_Half4_Out(GraphBuilder, PooledRenderTarget, OutputTextureRef, Texture,
		                                               TextureName, ShaderPlatform);
	}

	/**
	 * @brief Creates a read only texture which using a render target texture as input and uploads the data into the gpu, the format inside the gpu is float4
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param Texture The actual render target texture you like to upload into the gpu
	 * @param TextureName The name of the texture for debugging
	 * @return returns the final pass parameter to use it for your shader allocation as shader resource view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGTextureSRVRef CreateReadRenderTargetTexture_Half4_Out(
		FRDGBuilder& GraphBuilder, UTextureRenderTarget2D& Texture, const TCHAR* TextureName,
		const FStaticShaderPlatform& ShaderPlatform)
	{
		TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;
		FRDGTextureRef OutputTextureRef;
		return CreateReadRenderTargetTexture_Half4_Out(GraphBuilder, PooledRenderTarget, OutputTextureRef, Texture,
		                                               TextureName, ShaderPlatform);
	}


	/**
	 * @brief Creates a debug buffer with the size of 4 floats to debug a shader by returning values
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param OutputBufferRef The input needs a empty buffer reference and outputs a valid buffer, usually really important for more operations on this data stream
	 * @return returns the final pass parameter to use it for your shader allocation as unordered access view
	 */
	static FORCEINLINE_DEBUGGABLE FRDGBufferUAVRef CreateDebugWriteBuffer(FRDGBuilder& GraphBuilder,
	                                                                      FRDGBufferRef& OutputBufferRef)
	{
		// Create a default write buffer with 4 floats
		return TCreateWriteBuffer<float>(GraphBuilder, GET4_NUMBER, TEXT("Debug_R_Buffer_4f"), PF_R32_FLOAT,
		                                 OutputBufferRef);
	}

	/**
	 * @brief Callbacks the values of the shaders from the input of CreateDebugWriteBuffer
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param SourceBuffer The source buffer declared in CreateDebugWriteBuffer
	 * @param AsyncCallback The callback which returns the values
	 */
	static FORCEINLINE_DEBUGGABLE void CallbackDebugValues(FRDGBuilder& GraphBuilder, const FRDGBufferRef& SourceBuffer,
	                                                       TFunction<void(FVector4f DebugValues)> AsyncCallback)
	{
		// Create a readback buffer
		FRHIGPUBufferReadback* GPUBufferReadbackBuffer = new FRHIGPUBufferReadback(TEXT("Debug_R_Buffer_4f_Readback"));
		// Copy SourceBuffer into GPUBufferReadbackBuffer
		AddEnqueueCopyPass(GraphBuilder, GPUBufferReadbackBuffer, SourceBuffer, GET0_NUMBER);

		// Declare the runner function
		auto RunnerFunction = [GPUBufferReadbackBuffer, AsyncCallback](auto&& RunnerFunction_Internal) -> void
		{
			// 2. When we are inside the render thread we can read from the buffer
			if (GPUBufferReadbackBuffer->IsReady())
			{
				// Lock the GPUBufferReadbackBuffer which gives us the output buffer
				const float* Buffer = static_cast<float*>(GPUBufferReadbackBuffer->Lock(GET1_NUMBER));
				// Assign the 4 floats into a FVector4f
				FVector4f OutValue(Buffer[GET0_NUMBER], Buffer[GET1_NUMBER], Buffer[GET2_NUMBER], Buffer[GET3_NUMBER]);

				// Unlock the buffer to not freeze the thread
				GPUBufferReadbackBuffer->Unlock();

				// 4. Send OutValue to the game thread where we can read it
				AsyncTask(ENamedThreads::GameThread, [AsyncCallback, OutValue]
				{
					AsyncCallback(OutValue);
				});

				// Dispose the GPUBufferReadbackBuffer
				delete GPUBufferReadbackBuffer;
			}
			else
			{
				// 3. When we are not in the render thread for some reason
				// ( like calling this function on the game thread ) we call it again to enter the render thread
				AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunction_Internal]
				{
					RunnerFunction_Internal(RunnerFunction_Internal);
				});
			}
		};

		// 1. Invoke the RunnerFunction from the render thread
		AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunction]
		{
			RunnerFunction(RunnerFunction);
		});
	}

	/**
	 * @brief Callbacks the values of the shaders from the input of CreateDebugWriteBuffer
	 * @param GraphBuilder The GraphBuilder as reference to have access to rdg
	 * @param SourceBuffer The source buffer declared in CreateDebugWriteBuffer
	 * @param DataInput The Data Input to have a reference to the created data
	 * @param AsyncCallback The callback which returns the values
	 */
	static FORCEINLINE_DEBUGGABLE void CallbackDebugValues(FRDGBuilder& GraphBuilder, const FRDGBufferRef& SourceBuffer,
	                                                       TArray<float>& DataInput,
	                                                       TFunction<void(TArray<float>& DebugValues)> AsyncCallback)
	{
		// Create a readback buffer
		FRHIGPUBufferReadback* GPUBufferReadbackBuffer = new FRHIGPUBufferReadback(TEXT("Debug_R_Buffer_4f_Readback"));
		// Copy SourceBuffer into GPUBufferReadbackBuffer
		AddEnqueueCopyPass(GraphBuilder, GPUBufferReadbackBuffer, SourceBuffer, DataInput.Num() * sizeof(float));

		// Declare the runner function
		auto RunnerFunction = [GPUBufferReadbackBuffer, AsyncCallback, &DataInput](
			auto&& RunnerFunction_Internal) -> void
		{
			// 2. When we are inside the render thread we can read from the buffer
			if (GPUBufferReadbackBuffer->IsReady())
			{
				// Lock the GPUBufferReadbackBuffer which gives us the output buffer
				const float* Buffer = static_cast<float*>(GPUBufferReadbackBuffer->
					Lock(DataInput.Num() * sizeof(float)));

				int32 NumData = DataInput.Num();
				for (int32 i = GET0_NUMBER; i < NumData; ++i)
				{
					DataInput[i] = Buffer[i];
				}

				// Unlock the buffer to not freeze the thread
				GPUBufferReadbackBuffer->Unlock();

				// 4. Send OutValue to the game thread where we can read it
				AsyncTask(ENamedThreads::GameThread, [AsyncCallback, &DataInput]
				{
					AsyncCallback(DataInput);
				});

				// Dispose the GPUBufferReadbackBuffer
				delete GPUBufferReadbackBuffer;
			}
			else
			{
				// 3. When we are not in the render thread for some reason
				// ( like calling this function on the game thread ) we call it again to enter the render thread
				AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunction_Internal]
				{
					RunnerFunction_Internal(RunnerFunction_Internal);
				});
			}
		};

		// 1. Invoke the RunnerFunction from the render thread
		AsyncTask(ENamedThreads::ActualRenderingThread, [RunnerFunction]
		{
			RunnerFunction(RunnerFunction);
		});
	}

	// https://forums.unrealengine.com/t/get-assets-by-class-how-to-find-all-bp-assets-of-given-class/687899/6
	static FORCEINLINE_DEBUGGABLE bool GetBlueprintsFromBaseClass(UClass* InBaseClass,
	                                                              TArray<FAssetData>& FoundAssets)
	{
		if (!IsValid(InBaseClass))
		{
			return false;
		}

		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			"AssetRegistry");

		TMultiMap<FName, FString> TagsValues;

		// All native CDOs should be in memory - we can look up native subclasses of InNativeClass
		for (FThreadSafeObjectIterator It(InBaseClass); It; ++It)
		{
			const UClass* Class = It->GetClass();
			if (Class->IsNative() && Class->ClassDefaultObject == *It)
			{
				TagsValues.AddUnique(FBlueprintTags::NativeParentClassPath,
				                     FObjectPropertyBase::GetExportPath(Class));
			}
		}

		return AssetRegistryModule.Get().GetAssetsByTagValues(TagsValues, FoundAssets);
	}


	static FORCEINLINE_DEBUGGABLE FString GetConfigPath()
	{
#if WITH_EDITOR
		return FConfigCacheIni::NormalizeConfigIniPath(FPaths::Combine(FPaths::GeneratedConfigDir(),
		                                                               GetTargetPlatformManagerRef().
		                                                               GetRunningTargetPlatform()->PlatformName(),
		                                                               PluginConfigName));
#else
		return FString("");
#endif
	}

	static FORCEINLINE_DEBUGGABLE void GetStringConfigSetting(FString& String, const TCHAR* Section,
	                                                          const TCHAR* Key)
	{
		GConfig->GetString(
			Section,
			Key,
			String,
			GetConfigPath());
	}

	static FORCEINLINE_DEBUGGABLE bool GetBoolConfigSetting(bool& bBool, const TCHAR* Section, const TCHAR* Key)
	{
		return GConfig->GetBool(
			Section,
			Key,
			bBool,
			GetConfigPath());
	}

	static FORCEINLINE_DEBUGGABLE FString FormatDebugName(const FString& String, uint32 ID)
	{
		return FString::Format(*String, {*FString::FormatAsNumber(ID)});
	}

	static FORCEINLINE_DEBUGGABLE void SaveAsset(const TObjectPtr<UObject> Asset)
	{
		UPackage* Package = Asset->GetOutermost();
		if (!Package->MarkPackageDirty())
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "Package -> %s couldn't mark dirty, make sure to save the asset manually to make sure it's not losing any progress..."
			       ), *Package->GetName())
		}

		FSavePackageArgs Args;
		Args.TopLevelFlags = RF_Public | RF_Standalone;
		Args.Error = GError;
		//Args.bForceByteSwapping = true;
		Args.bWarnOfLongFilename = true;
		Args.SaveFlags = SAVE_KeepGUID;


		const FString PackagePath = FPackageName::LongPackageNameToFilename(
			Package->GetName(), FPackageName::GetAssetPackageExtension());
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.FileExists(*PackagePath))
		{
			// Save the package and all its contents
			if (!PlatformFile.IsReadOnly(*PackagePath) && UPackage::SavePackage(Package, Asset, *PackagePath, Args))
			{
				FAssetData AssetData(Asset);
				TArray<FAssetData> Data;
				Data.Add(AssetData);
				FAssetRegistryModule::AssetsSaved(MoveTemp(Data));
				//FAssetRegistryModule::AssetSaved(*Asset);
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Saved Asset -> %s"), *Package->GetName());
			}
		}
	}

	static FORCEINLINE_DEBUGGABLE void SortAssetsByPathName(TArray<FAssetData>& FromData)
	{
		FromData.Sort([](const FAssetData& A, const FAssetData& B)
		{
			const FString& APath = A.GetAsset()->GetPathName();
			const FString& BPath = B.GetAsset()->GetPathName();

			return APath < BPath;
		});
	}

	template <class Key, class Value>
	static FORCEINLINE_DEBUGGABLE void SortKeyFromStartLowToEndHigh(TMap<Key, Value>& Map)
	{
		Map.KeySort([](const Key& A, const Key& B) { return A < B; });
	}

	static FORCEINLINE_DEBUGGABLE void SaveNewAsset(const TObjectPtr<UObject> Asset,
	                                                const FString& UniqueIdentifier = FString(""))
	{
		UPackage* Package = Asset->GetOutermost();
		if (!Package->MarkPackageDirty())
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT(
				       "Package -> %s couldn't mark dirty, make sure to save the asset manually to make sure it's not losing any progress..."
			       ), *Package->GetName())
		}

		//Asset->SetFlags(EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | EObjectFlags::RF_Transactional);


		FSavePackageArgs Args;
		Args.TopLevelFlags = RF_Public | RF_Standalone | RF_Transactional;
		Args.Error = GError;
		//Args.bForceByteSwapping = true;
		Args.bWarnOfLongFilename = true;
		Args.SaveFlags = SAVE_KeepGUID;

		const FString PackagePath = FPackageName::LongPackageNameToFilename(
			Package->GetName() + UniqueIdentifier, FPackageName::GetAssetPackageExtension());
		// Save the package and all its contents
		if (UPackage::SavePackage(Package, Asset, *PackagePath, Args))
		{
			FAssetData AssetData(Asset);
			TArray<FAssetData> Data;
			Data.Add(AssetData);
			FAssetRegistryModule::AssetsSaved(MoveTemp(Data));
			//FAssetRegistryModule::AssetSaved(*Asset);
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Saved Asset -> %s"), *Package->GetName());
		}
	}

	template <class T>
	static FORCEINLINE_DEBUGGABLE TObjectPtr<T> LoadAssetFromReferencePath(FString& FromPath)
	{
		FSoftObjectPath SkinWeightTextureSoftObjectPath(FromPath);
		if (SkinWeightTextureSoftObjectPath.IsValid())
		{
			TObjectPtr<UObject> LoadedAsset = SkinWeightTextureSoftObjectPath.TryLoad();
			if (IsValid(LoadedAsset))
			{
				// The asset was successfully loaded. You can now use it.
				// For example, if it's a texture:
				TObjectPtr<T> LoadedTexture = Cast<T>(LoadedAsset);
				if (IsValid(LoadedTexture))
				{
					return LoadedTexture;
				}
			}
		}
		return nullptr;
	}

	static FORCEINLINE_DEBUGGABLE TObjectPtr<UTextureRenderTarget2DArray> GenerateBlankRenderTargetArray(
		const FString& InPath, const FString& InName, uint16 SizeXY, uint8 SizeZ,
		const EPixelFormat& Format)
	{
		if (FString PackageName; FPackageName::TryConvertFilenameToLongPackageName(InPath, PackageName))
		{
			UPackage* Package = CreatePackage(*PackageName);
			check(Package);

			TObjectPtr<UTextureRenderTarget2DArray> Texture = NewObject<UTextureRenderTarget2DArray>(
				Package, *InName, RF_Public | RF_Standalone);

			Texture->ClearColor = FLinearColor::Transparent;
			Texture->Init(SizeXY, SizeXY, SizeZ, Format);
			Texture->UpdateResourceImmediate(true);
#if WITH_EDITOR
			Texture->PostEditChange();

			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(Texture);

			SaveNewAsset(Texture);
#endif

			return Texture;
		}

		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT("The Directory is not an valid Project Directory | Path -> %s..."), *InPath);

		return nullptr;
	}

	static TObjectPtr<UTexture2DArray> GenerateTexture2DArrayFromRenderTarget2DArray(
		const TObjectPtr<UTextureRenderTarget2DArray> FromRenderTarget, const FString& InPath,
		const FString& TextureName, bool bSave, bool bCheckIfTextureIsFilled,
		float FillTolerance, bool& OutIsFilled)
	{
		if (FString PackageName; FPackageName::TryConvertFilenameToLongPackageName(InPath, PackageName))
		{
			if (const TObjectPtr<UTexture2DArray> Texture = FromRenderTarget->ConstructTexture2DArray(
				CreatePackage(*PackageName), TextureName,
				FromRenderTarget->GetMaskedFlags() | RF_Public | RF_Standalone); IsValid(Texture))
			{
#if WITH_EDITOR
				if (bSave)
				{
					// Notify asset registry of new asset
					FAssetRegistryModule::AssetCreated(Texture);

					SaveNewAsset(Texture);
				}

				Texture->UpdateResource();

				if (bCheckIfTextureIsFilled)
				{
					TArray<FFloat16Color> DestinationArray; // The destination TArray

					const int32 DstMipSize = CalculateImageBytes(Texture->GetSizeX(), Texture->GetSizeY(),
					                                             GET1_NUMBER, PF_FloatRGBA);

					// Resize the destination array to accommodate the copied data
					DestinationArray.SetNumZeroed(
						FMath::CeilToInt32(
							static_cast<float>(DstMipSize) / static_cast<float>(sizeof(FFloat16Color))));

					// Copy data from the source memory buffer to the destination array
					uint8* SliceData = Texture->Source.LockMip(GET0_NUMBER);

					FMemory::Memcpy(DestinationArray.GetData(), SliceData, DstMipSize);

					Texture->Source.UnlockMip(GET0_NUMBER);

					bool bValidPixels = false;
					int32 NumData = DestinationArray.Num();
					for (int32 i = GET0_NUMBER; i < NumData; ++i)
					{
						const FFloat16Color& Color = DestinationArray[i];

						if (!FMath::IsNearlyZero(Color.R.GetFloat(), FillTolerance) || !
							FMath::IsNearlyZero(Color.G.GetFloat(), FillTolerance) || !
							FMath::IsNearlyZero(Color.B.GetFloat(), FillTolerance) || !FMath::IsNearlyZero(
								Color.A.GetFloat(), FillTolerance))
						{
							bValidPixels = true;
							break;
						}
					}
					OutIsFilled = bValidPixels;
				}
#endif

				return Texture;
			}

			UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Failed to create Texture 2D Array from Render Target..."));

			return nullptr;
		}

		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT("The Directory is not an valid Project Directory | Path -> %s..."), *InPath);

		return nullptr;
	}
};

USTRUCT()
struct TURBOSEQUENCE_HELPERMODULE_LF_API FBool3_Lf
{
	GENERATED_BODY()

	FBool3_Lf()
	{
	}

	~FBool3_Lf()
	{
	}

	UPROPERTY(EditAnywhere)
	bool bX = false;

	UPROPERTY(EditAnywhere)
	bool bY = false;

	UPROPERTY(EditAnywhere)
	bool bZ = false;

	FORCEINLINE_DEBUGGABLE bool IsAnyAxisChecked() const
	{
		return bX || bY || bZ;
	}
};

USTRUCT()
struct TURBOSEQUENCE_HELPERMODULE_LF_API FBool2_Lf
{
	GENERATED_BODY()

	FBool2_Lf()
	{
	}

	~FBool2_Lf()
	{
	}

	UPROPERTY(EditAnywhere)
	bool bX = false;

	UPROPERTY(EditAnywhere)
	bool bY = false;

	FORCEINLINE_DEBUGGABLE bool IsAnyAxisChecked() const
	{
		return bX || bY;
	}
};

UCLASS()
class TURBOSEQUENCE_HELPERMODULE_LF_API UTurboSequence_Helper_BlueprintFunctions_Lf : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool TurboSequence_NeverCalledFunction_Lf()
	{
		static bool bCalled = false;
		if (!bCalled)
		{
			bCalled = true;
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable)
	static int32 TurboSequence_GetNumCPUThreads_Lf()
	{
		return FTurboSequence_Helper_Lf::NumCPUThreads();
	}

	UFUNCTION(BlueprintCallable)
	static int64 TurboSequence_GetThreadID_Lf()
	{
		return FPlatformTLS::GetCurrentThreadId();
	}

	UFUNCTION(BlueprintCallable)
	static bool TurboSequence_IsGameThread_Lf()
	{
		return IsInGameThread();
	}

	UFUNCTION(BlueprintCallable)
	static void TurboSequence_LogToConsole_Lf(const FString Message)
	{
		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("%s"), *Message);
	}

	UFUNCTION(BlueprintCallable, meta=(Keywords="World, World Context, Static World, Get World, Turbo Sequence, TS"))
	static UWorld* TurboSequence_GetWorldFromStaticFunction()
	{
		return GEngine->GetCurrentPlayWorld();
	}
};
