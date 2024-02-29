// Copyright Lukas Fratzl, 2022-2023. All Rights Reserved.


#include "TurboSequence_AnimLibraryAction_Lf.h"

#include "TurboSequence_AnimLibrary_Lf.h"
#include "TurboSequence_Editor_Lf.h"


UClass* FTurboSequence_AnimLibraryAction_Lf::GetSupportedClass() const
{
	return UTurboSequence_AnimLibrary_Lf::StaticClass();
}

FText FTurboSequence_AnimLibraryAction_Lf::GetName() const
{
	return INVTEXT("Animation Library");
}

FColor FTurboSequence_AnimLibraryAction_Lf::GetTypeColor() const
{
	return FColor::Red;
}

uint32 FTurboSequence_AnimLibraryAction_Lf::GetCategories()
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
