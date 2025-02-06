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
	virtual void OnSetMeshIsVisible_Concurrent(ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride,
	                                           const bool bDefaultVisibility, const int32 MeshID,
	                                           const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                           ThreadContext) override;
	virtual void OnSetMeshIsAnimated_Concurrent(ETurboSequence_IsAnimatedOverride_Lf& IsAnimatedOverride,
	                                            const bool bDefaultIsAnimated, const int32 MeshID,
	                                            const TObjectPtr<UTurboSequence_ThreadContext_Lf>&
	                                            ThreadContext) override;
	virtual void OnSetMeshLod_Concurrent(int16& LodIndexOverride, const int32 MeshID,
	                                     const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext) override;


	UPROPERTY(EditAnywhere, meta=(ClampMin="0", ClampMax="31"), Category="TurboSequence")
	int32 LodIndex = 0;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	ETurboSequence_IsVisibleOverride_Lf IsVisible = ETurboSequence_IsVisibleOverride_Lf::Default;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	ETurboSequence_IsAnimatedOverride_Lf IsAnimated = ETurboSequence_IsAnimatedOverride_Lf::Default;
};
