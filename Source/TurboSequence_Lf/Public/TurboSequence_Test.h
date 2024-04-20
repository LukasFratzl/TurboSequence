// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Components/PoseableMeshComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/SavePackage.h"
#include "TurboSequence_Test.generated.h"


struct FBoneData123
{
	FTransform AnimatedBoneMatrix;
	FTransform InverseRestPoseBoneMatrix;
};


UCLASS()
class TURBOSEQUENCE_LF_API ATurboSequence_Test : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurboSequence_Test();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// void SpawnCharactersDelayed();
	//
	// UPROPERTY(VisibleAnywhere)
	// TObjectPtr<UHierarchicalInstancedStaticMeshComponent> Renderer;
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UAnimSequence> Animation;
	//
	// // UPROPERTY(EditAnywhere)
	// // TObjectPtr<USkeleton> Skeleton;
	//
	// UPROPERTY(EditAnywhere)
	// FName TestSocketName;
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<USkeletalMesh> Mesh;
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UStaticMesh> RendererMesh;
	//
	// //UPROPERTY(EditAnywhere)
	// TMap<uint16, FBoneData123> Bones;
	//
	// UPROPERTY(EditAnywhere)
	// TArray<FVector4f> BonesMatrixArray;
	//
	// UPROPERTY(EditAnywhere)
	// TArray<FVector4f> RestPoseBonesMatrixArray;
	//
	// UPROPERTY(EditAnywhere)
	// TArray<FVector4f> BoneWeights;
	//
	// UPROPERTY(EditAnywhere)
	// TArray<FVector4f> SettingsData;
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UMaterial> Material;
	//
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset;
	//
	// float AnimationTime;
	// int32 CurrentCharacterCount_Internal;
	// FVector LastSpawnLocation_Internal;
	//
	// UPROPERTY(EditAnywhere)
	// int32 AmountOfCharactersToSpawn = 0;
	//
	// UPROPERTY(EditAnywhere)
	// float DistanceBetweenCharacters = 300;
	//
	// TMap<int32, int32> TrackIndexMap;
	//
	// UPROPERTY(EditAnywhere)
	// TObjectPtr<UPoseableMeshComponent> Poser;
	//
	//
	// static FORCEINLINE_DEBUGGABLE void GetBoneTransformFromAnimationSafe(FTransform& OutAtom, const TObjectPtr<UAnimSequence> Animation, uint16 SkeletonBoneIndex, const TMap<int32, int32>& SkeletonBoneMap, float AnimationTime, const FReferenceSkeleton& ReferenceSkeleton)
	// {
	// 	// if (int32 TrackIndex = SkeletonBoneMap.Contains(SkeletonBoneIndex) ? SkeletonBoneMap[SkeletonBoneIndex] : INDEX_NONE; TrackIndex != INDEX_NONE)
	// 	// {
	// 	// 	Animation->GetBoneTransform(OutAtom, TrackIndex, AnimationTime, !Animation->IsCompressedDataValid());
	// 	// }
	// 	// else
	// 	// {
	// 	// 	OutAtom = ReferenceSkeleton.GetRefBonePose()[SkeletonBoneIndex];
	// 	// }
	// }
	//
	//
	// static void AnimateTestMesh(const TObjectPtr<UPoseableMeshComponent> Poser, const TObjectPtr<UAnimSequence> Animation, float& AnimTime, float DeltaTime)
	// {
	// 	AnimTime += DeltaTime;
	// 	AnimTime = FMath::Fmod(AnimTime, Animation->GetPlayLength());
	//
	// 	TMap<int32, int32> AnimTracks;
	// 	const TArray<FTrackToSkeletonMap>& Tracks = Animation->GetCompressedTrackToSkeletonMapTable();
	// 	for (const auto& Track : Tracks)
	// 	{
	// 		if (!AnimTracks.Contains(Track.BoneTreeIndex))
	// 		{
	// 			AnimTracks.Add(Track.BoneTreeIndex, Track.BoneTreeIndex);
	// 		}
	// 	}
	//
	// 	const FReferenceSkeleton& ReferenceSkeleton = Poser->GetSkinnedAsset()->GetSkeleton()->GetReferenceSkeleton();
	//
	// 	for (int32 i = 0; i < ReferenceSkeleton.GetNum(); ++i)
	// 	{
	// 		FTransform BoneTransform;
	// 		GetBoneTransformFromAnimationSafe(BoneTransform, Animation, i, AnimTracks, AnimTime, ReferenceSkeleton);
	// 		Poser->BoneSpaceTransforms[i] = BoneTransform;
	// 	}
	// }


	// static FORCEINLINE_DEBUGGABLE uint32 BoneIndexToWeightIndex(const TMap<uint16, FBone>& FromBones, int32 FromIndex)
	// {
	// 	if (FromBones.Contains(FromIndex))
	// 	{
	// 		int32 Index = 0;
	// 		for (const TTuple<uint16, FBone>& Bone : FromBones)
	// 		{
	// 			if (Bone.Key == FromIndex)
	// 			{
	// 				return Index;
	// 			}
	// 			
	// 			Index++;
	// 		}
	// 	}
	// 	return 0;
	// }
};
