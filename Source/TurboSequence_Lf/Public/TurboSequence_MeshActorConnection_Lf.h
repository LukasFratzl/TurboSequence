// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "Components/ActorComponent.h"
#include "TurboSequence_MeshActorConnection_Lf.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TURBOSEQUENCE_LF_API UTurboSequence_MeshActorConnection_Lf : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTurboSequence_MeshActorConnection_Lf();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	FTurboSequence_MinimalMeshData_Lf ActorTSMeshDataID;

	UPROPERTY(VisibleAnywhere, Category="TurboSequence")
	TObjectPtr<UTurboSequence_FootprintAsset_Lf> FootprintAsset;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<USkinnedMeshComponent*> SkinnedMeshComponents;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;


	virtual void OnMeshDataIDSend_GameThread(const FTurboSequence_MinimalMeshData_Lf& MeshData);

	virtual const FTurboSequence_MinimalMeshData_Lf& GetTsMeshData() const
	{
		return ActorTSMeshDataID;
	}

	virtual void OnFootprintAssetTick_GameThread(const float DeltaTime);
};
