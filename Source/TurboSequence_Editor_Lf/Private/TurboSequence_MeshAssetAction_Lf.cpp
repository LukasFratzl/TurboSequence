// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_MeshAssetAction_Lf.h"

#include "TurboSequence_Editor_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"


UClass* FTurboSequence_MeshAssetAction_Lf::GetSupportedClass() const
{
	return UTurboSequence_MeshAsset_Lf::StaticClass();
}

FText FTurboSequence_MeshAssetAction_Lf::GetName() const
{
	return INVTEXT("Mesh Asset");
}

FColor FTurboSequence_MeshAssetAction_Lf::GetTypeColor() const
{
	return FColor::Green;
}

uint32 FTurboSequence_MeshAssetAction_Lf::GetCategories()
{
	return FTurboSequence_Editor_LfModule::PluginAssetCategory;
}
