// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.


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

// void FTurboSequence_MeshAssetAction_Lf::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
// {
// 	// FAssetTypeActions_Base::OpenAssetEditor(InObjects, EditWithinLevelEditor);
// 	//MakeShared<FTurboSequence_Asset_EToolkit>()->InitEditor(InObjects, EditWithinLevelEditor);
// }

// void FTurboSequence_MeshAssetAction_Lf::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
// {
// 	MakeShared<FTurboSequence_Asset_EToolkit>()->InitEditor(InObjects);
// }
