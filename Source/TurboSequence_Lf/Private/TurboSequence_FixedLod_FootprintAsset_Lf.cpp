// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_FixedLod_FootprintAsset_Lf.h"

void UTurboSequence_FixedLod_FootprintAsset_Lf::OnSetMeshIsVisible_Concurrent(
	ETurboSequence_IsVisibleOverride_Lf& IsVisibleOverride, const bool bDefaultVisibility, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshIsVisible_Concurrent(IsVisibleOverride, bDefaultVisibility, MeshID, ThreadContext);

	IsVisibleOverride = IsVisible;
}

void UTurboSequence_FixedLod_FootprintAsset_Lf::OnSetMeshIsAnimated_Concurrent(
	ETurboSequence_IsAnimatedOverride_Lf& IsAnimatedOverride, const bool bDefaultIsAnimated, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshIsAnimated_Concurrent(IsAnimatedOverride, bDefaultIsAnimated, MeshID, ThreadContext);

	IsAnimatedOverride = IsAnimated;
}

void UTurboSequence_FixedLod_FootprintAsset_Lf::OnSetMeshLod_Concurrent(int16& LodIndexOverride, const int32 MeshID,
                                                                        const TObjectPtr<
	                                                                        UTurboSequence_ThreadContext_Lf>&
                                                                        ThreadContext)
{
	Super::OnSetMeshLod_Concurrent(LodIndexOverride, MeshID, ThreadContext);

	LodIndexOverride = LodIndex;
}
