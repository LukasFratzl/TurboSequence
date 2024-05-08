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
	//uint8 FadeActiveCounter = 0;
	bool bIsInUERange = true;
	bool bInit = false;

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
	virtual void OnSetMeshIsVisible_Concurrent(ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
	                                           const bool bDefaultVisibility, const int32 MeshID,
	                                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                           ThreadContext) override;
	virtual void OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride, const int32 MeshID,
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

	virtual void OnManagerUpdated_GameThread(const float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float FadeTime = 0.5f;

	UPROPERTY(EditAnywhere)
	float FadeDistance = 1000;

	UPROPERTY(EditAnywhere)
	FVector CameraLocation = FVector::ZeroVector;

	float LastDeltaTime = 0;

	TMap<int32, FDemoMeshInstance_Lf> MeshesOpen;
	TMap<int32, FDemoMeshInstance_Lf> MeshesClosed;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> MeshActor = nullptr;

	void FadeMesh(const int32 MeshID, const FDemoMeshInstance_Lf& Instance,
	              const TObjectPtr<USkinnedMeshComponent>& Mesh) const;

private:
	GENERATED_BODY()
};
