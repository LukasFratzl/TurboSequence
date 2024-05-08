// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_FixedLod_FootprintAsset_Lf.h"

void UTurboSequence_FixedLod_FootprintAsset_Lf::OnSetMeshLod_Concurrent(int16& OutLodIndex, const int32 MeshID,
	const TObjectPtr<UTurboSequence_ThreadContext_Lf>& ThreadContext)
{
	Super::OnSetMeshLod_Concurrent(OutLodIndex, MeshID, ThreadContext);

	OutLodIndex = LodIndex;
}
