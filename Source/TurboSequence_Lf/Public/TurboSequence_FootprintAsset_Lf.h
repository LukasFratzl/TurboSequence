// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "UObject/Object.h"
#include "TurboSequence_FootprintAsset_Lf.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)
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

	TMap<uint32, float> DefaultAnimationTransitions;

	UFUNCTION(BlueprintNativeEvent)
	void OnHybridModeUEInstanceAddRemove_Concurrent(const int64& MeshID, UTurboSequence_ThreadContext_Lf* ThreadContext);

	UFUNCTION(BlueprintCallable)
	void TurboSequence_Default_HybridModeUEInstanceAddRemove_Concurrent_Lf(const int64& MeshID, UTurboSequence_ThreadContext_Lf* ThreadContext);

	virtual void OnHybridModeUEInstanceAddRemove_Concurrent_Override(const int64& MeshID, UTurboSequence_ThreadContext_Lf* ThreadContext)
	{
		OnHybridModeUEInstanceAddRemove_Concurrent(MeshID, ThreadContext);
	}


	UFUNCTION(BlueprintNativeEvent)
	void OnHybridModeUEInstanceSpawn_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld);

	UFUNCTION(BlueprintCallable)
	AActor* TurboSequence_Default_HybridModeUEInstanceSpawn_GameThread_Lf(
		const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld);

	virtual void OnHybridModeUEInstanceSpawn_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                             UWorld* InWorld)
	{
		OnHybridModeUEInstanceSpawn_GameThread(MeshData, InWorld);
	}


	UFUNCTION(BlueprintNativeEvent)
	void OnHybridModeUEInstanceDestroy_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld);

	UFUNCTION(BlueprintCallable)
	void TurboSequence_Default_HybridModeUEInstanceDestroy_GameThread_Lf(
		const FTurboSequence_MinimalMeshData_Lf& MeshData,
		bool bSetTSMeshToUEMeshTransform = true);

	virtual void OnHybridModeUEInstanceDestroy_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                               UWorld* InWorld)
	{
		OnHybridModeUEInstanceDestroy_GameThread(MeshData, InWorld);
	}


	UFUNCTION(BlueprintNativeEvent)
	void OnHybridModeUEInstanceTick_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData, UWorld* InWorld,
	                                           const float& DeltaTime);


	UFUNCTION(BlueprintCallable)
	void TurboSequence_Default_HybridModeUEInstanceTick_GameThread_Lf(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                                  const float& DeltaTime);

	virtual void OnHybridModeUEInstanceTick_GameThread_Override(const FTurboSequence_MinimalMeshData_Lf& MeshData,
	                                                            UWorld* InWorld, const float& DeltaTime)
	{
		OnHybridModeUEInstanceTick_GameThread(MeshData, InWorld, DeltaTime);
	}
};
