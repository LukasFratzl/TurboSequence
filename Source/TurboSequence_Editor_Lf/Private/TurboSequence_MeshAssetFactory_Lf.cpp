// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_MeshAssetFactory_Lf.h"

#include "TurboSequence_MeshAsset_Lf.h"
#include "UObject/ConstructorHelpers.h"


UTurboSequence_MeshAssetFactory_Lf::UTurboSequence_MeshAssetFactory_Lf()
{
	SupportedClass = UTurboSequence_MeshAsset_Lf::StaticClass();
	bCreateNew = true;

	FString DefaultRenderingNiagaraSystem;

	FTurboSequence_Helper_Lf::GetStringConfigSetting(DefaultRenderingNiagaraSystem,
	                                                 TEXT(
		                                                 "/Script/TurboSequence_Editor_Lf.TurboSequence_RefSettings_Lf"),
	                                                 TEXT("Default_Rendering_NiagaraSystem"));

	if (DefaultRenderingNiagaraSystem.IsEmpty())
	{
		DefaultRenderingNiagaraSystem = FTurboSequence_Helper_Lf::ReferenceTurboSequenceNiagaraSystem;
	}


	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystem(*DefaultRenderingNiagaraSystem);

	if (IsValid(NiagaraSystem.Object))
	{
		DefaultAssetNiagaraSystem = NiagaraSystem.Object;
	}
	else
	{
		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT(
			       "Can not find Niagara System, it should at .../Plugins/TurboSequence_Lf/Resources/FXS_TurboSequence_Mesh_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths"
		       ))
	}
}

UObject* UTurboSequence_MeshAssetFactory_Lf::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                                              EObjectFlags Flags, UObject* Context,
                                                              FFeedbackContext* Warn)
{
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = NewObject<UTurboSequence_MeshAsset_Lf>(
		InParent, Class, Name, Flags, Context);

	Asset->bNeedGeneratedNextEngineStart = false;

	const FAssetRegistryModule& AssetRegistry = FModuleManager::LoadModuleChecked<
		FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> GlobalAssetData;
	AssetRegistry.Get().GetAssetsByClass(FTopLevelAssetPath(UTurboSequence_GlobalData_Lf::StaticClass()->GetPathName()),
	                                     GlobalAssetData);
	if (GlobalAssetData.Num())
	{
		FTurboSequence_Helper_Lf::SortAssetsByPathName(GlobalAssetData);
		Asset->GlobalData = Cast<UTurboSequence_GlobalData_Lf>(GlobalAssetData[GET0_NUMBER].GetAsset());
	}
	else
	{
		UE_LOG(LogTurboSequence_Lf, Error,
		       TEXT(
			       "Can not find the Global Data asset -> This is really bad, without it Turbo Sequence does not work, you can recover it by creating an UTurboSequence_GlobalData_Lf Data Asset, Right click in the content browser anywhere in the Project, select Data Asset and choose UTurboSequence_GlobalData_Lf, save it and restart the editor"
		       ));
	}


	if (IsValid(DefaultAssetNiagaraSystem))
	{
		Asset->RendererSystem = DefaultAssetNiagaraSystem;
	}
	else
	{
		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT(
			       "Can not find Niagara System, it should at .../Plugins/TurboSequence_Lf/Resources/FXS_TurboSequence_Mesh_Lf, please assign it manually in the Project settings under TurboSequence Lf -> Reference Paths"
		       ))
	}

	return Asset; //NewObject<UTurboSequence_MeshAsset_Lf>(InParent, Class, Name, Flags, Context);
}

FString UTurboSequence_MeshAssetFactory_Lf::GetDefaultNewAssetName() const
{
	return FString(TEXT("TS_MeshAsset"));
}
