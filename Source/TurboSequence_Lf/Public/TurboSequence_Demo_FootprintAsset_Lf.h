// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "Engine/DataAsset.h"
#include "TurboSequence_Demo_FootprintAsset_Lf.generated.h"


USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoMeshInstance_Lf
{
	GENERATED_BODY()

	float FadeTimeRuntime = 0.3f;
	bool bIsInUERange = true;
	bool bInit = false;
	int8 NumFrames = 0;

	UPROPERTY()
	TObjectPtr<AActor> Mesh;
};


/**
 *
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_Demo_FootprintAsset_Lf : public UTurboSequence_FootprintAsset_Lf
{
public:
	GENERATED_BODY()

	virtual void OnSetMeshIsVisible_Concurrent(ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
	                                           const bool bDefaultVisibility, const int32 MeshID,
	                                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                           ThreadContext) override;
	virtual void OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride, const int32 MeshID,
	                                               const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                               ThreadContext) override;

	virtual void OnSetMeshIsAnimated_Concurrent(ETurboSequence_IsAnimatedOverride_Lf& IsAnimatedOverride,
	                                            const bool bDefaultIsAnimated, const int32 MeshID,
	                                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                            ThreadContext) override;

	virtual void OnManagerEndPlay_GameThread(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnAddedMeshInstance_GameThread(const int32 MeshID,
	                                            const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset) override;
	virtual void OnRemovedMeshInstance_GameThread(const int32 MeshID,
	                                              const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset) override;

	virtual void OnMeshPreSolveAnimationMeta_Concurrent(const int32 MeshID,
	                                                    const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                                    ThreadContext) override;

	virtual void OnPostManagerUpdated_GameThread(const float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float FadeTime = 0.5f;

	UPROPERTY(EditAnywhere)
	float FadeDistance = 1000;

	float LastDeltaTime = 0;

	TMap<int32, FDemoMeshInstance_Lf> MeshesOpen;
	TMap<int32, FDemoMeshInstance_Lf> MeshesClosed;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MeshActor = nullptr;

	virtual bool CanShowUEMesh(const int32 MeshID, const float MeshDistanceToCamera) const
	{
		return MeshDistanceToCamera < FadeDistance;
	}

	virtual void FadeMesh(const int32 MeshID, const FDemoMeshInstance_Lf& Instance,
	                      const TObjectPtr<USkinnedMeshComponent>& Mesh);
};
