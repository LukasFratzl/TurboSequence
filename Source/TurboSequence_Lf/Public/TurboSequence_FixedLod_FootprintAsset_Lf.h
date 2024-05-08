// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_FootprintAsset_Lf.h"
#include "Engine/DataAsset.h"
#include "TurboSequence_FixedLod_FootprintAsset_Lf.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_LF_API UTurboSequence_FixedLod_FootprintAsset_Lf : public UTurboSequence_FootprintAsset_Lf
{
	GENERATED_BODY()

public:
	virtual void OnSetMeshLod_Concurrent(int16& OutLodIndex, const int32 MeshID,
		const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext) override;

	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="31"))
	int32 LodIndex = 0;
};
