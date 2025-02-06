// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "TurboSequence_MeshActorConnection_Lf.h"
#include "Engine/DataAsset.h"
#include "TurboSequence_DemoSimple_FootprintAsset_Lf.generated.h"

USTRUCT()
struct TURBOSEQUENCE_LF_API FDemoMeshData_Lf
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<USkinnedMeshComponent> Mesh;

	int8 FrameDelay = 0;
	bool bIsUEVisible = false;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshActorConnection_Lf> ActorConnection;
};

/**
 *
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_DemoSimple_FootprintAsset_Lf : public UTurboSequence_FootprintAsset_Lf
{
public:
	GENERATED_BODY()

	TMap<int32, FDemoMeshData_Lf> MeshDataCollection;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TSubclassOf<AActor> UEMeshClass;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	float UEMeshShowDistance = 1000.0f;


	virtual bool CanShowUEMesh(const int32 MeshID, const float MeshDistanceToCamera) const
	{
		return MeshDistanceToCamera < UEMeshShowDistance;
	}

	virtual void OnSetMeshIsVisible_Concurrent(ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
		const bool bDefaultVisibility, const int32 MeshID,
		const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext) override;
	virtual void OnSetMeshIsUpdatingLod_Concurrent(bool& bIsUpdatingLodOverride, const int32 MeshID,
		const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext) override;
	virtual void OnAddedMeshInstance_GameThread(const int32 MeshID,
		const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset) override;
	virtual void OnRemovedMeshInstance_GameThread(const int32 MeshID,
		const TObjectPtr<UTurboSequence_MeshAsset_Lf>& FromAsset) override;
	virtual void OnPostManagerUpdated_GameThread(const float DeltaTime) override;
	virtual void OnManagerEndPlay_GameThread(const EEndPlayReason::Type EndPlayReason) override;
};
