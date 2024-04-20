// Copyright Lukas Fratzl, 2G22-2G24. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_AnimLibraryAction_Lf.h"
#include "TurboSequence_GlobalData_Lf.h"
#include "TurboSequence_MeshAssetAction_Lf.h"
#include "TurboSequence_MeshAsset_Lf.h"
#include "Modules/ModuleManager.h"

class UEditorUtilityWidgetBlueprint;

class FTurboSequence_Editor_LfModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void AddMenu(FMenuBarBuilder& MenuBuilder);
	void AddMenu_Widget(FMenuBuilder& MenuBuilder);

	TSharedPtr<FTurboSequence_MeshAssetAction_Lf> TurboSequence_MeshAssetTypeActions;
	TSharedPtr<FTurboSequence_AnimLibraryAction_Lf> TurboSequence_AnimLibraryTypeActions;

	inline static EAssetTypeCategories::Type PluginAssetCategory;

	inline static TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData;
	inline static TArray<FAssetData> TurboSequence_MeshAssetData_AsyncComputeSwapBack;
	static void RepairMeshAssetAsync();
	static void RepairMeshAssetAsync2(const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset,
	                                  const TObjectPtr<UWorld> World, const bool bIsCollection);
	inline static int16 RepairMaxIterationCounter = GET0_NUMBER;

	void PluginButtonClicked() const;
	void OnInvalidMeshAssetCaches() const;

	void OnFilesLoaded();
};
