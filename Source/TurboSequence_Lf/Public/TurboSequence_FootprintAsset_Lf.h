// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "UObject/Object.h"
#include "TurboSequence_FootprintAsset_Lf.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_FootprintAsset_Lf : public UDataAsset
{
	GENERATED_BODY()

public:
	UTurboSequence_FootprintAsset_Lf();


	UPROPERTY(EditAnywhere, Category="Exclude")
	bool bExcludeFromSystem = false;

	UPROPERTY(EditAnywhere, Category="Hybrid Mode")
	TSubclassOf<AActor> UnrealEngineMeshInstance;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="25000"), Category="Hybrid Mode")
	float HybridModeMeshDrawRangeUEInstance = 2500;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="25000"), Category="Hybrid Mode")
	float HybridModeAnimationDrawRangeUEInstance = 5000;

	TMap<int32, float> DefaultAnimationTransitions;

	void TurboSequence_Default_HybridModeUEInstanceAddRemove_Concurrent_Lf(
		int32 MeshID, UTurboSequence_ThreadContext_Lf* ThreadContext);

	virtual void OnHybridModeUEInstanceAddRemove_Concurrent_Override(int32 MeshID,
	                                                                 UTurboSequence_ThreadContext_Lf* ThreadContext)
	{
		TurboSequence_Default_HybridModeUEInstanceAddRemove_Concurrent_Lf(MeshID, ThreadContext);
	}

	AActor* TurboSequence_Default_HybridModeUEInstanceSpawn_GameThread_Lf(
		const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld);

	virtual void OnHybridModeUEInstanceSpawn_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                             UWorld* InWorld)
	{
		TurboSequence_Default_HybridModeUEInstanceSpawn_GameThread_Lf(MeshData, InWorld);
	}

	void TurboSequence_Default_HybridModeUEInstanceDestroy_GameThread_Lf(
		const FTurboSequence_MinimalMeshData_Lf& MeshData,
		bool bSetTSMeshToUEMeshTransform = true);

	virtual void OnHybridModeUEInstanceDestroy_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                               UWorld* InWorld)
	{
		TurboSequence_Default_HybridModeUEInstanceDestroy_GameThread_Lf(MeshData, true);
	}

	void TurboSequence_Default_HybridModeUEInstanceTick_GameThread_Lf(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                                  float DeltaTime);

	virtual void OnHybridModeUEInstanceTick_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                            UWorld* InWorld, float DeltaTime)
	{
		TurboSequence_Default_HybridModeUEInstanceTick_GameThread_Lf(MeshData, DeltaTime);
	}
};
