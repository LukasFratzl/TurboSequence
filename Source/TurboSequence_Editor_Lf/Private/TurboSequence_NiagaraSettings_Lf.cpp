// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_NiagaraSettings_Lf.h"

#include "TurboSequence_ControlWidget_Lf.h"
#include "TurboSequence_Editor_Lf.h"

#define LOCTEXT_NAMESPACE "TurboSequence Lf Niagara Settings"

UTurboSequence_NiagaraSettings_Lf::UTurboSequence_NiagaraSettings_Lf()
	: Super()

{
}

UTurboSequence_NiagaraSettings_Lf::~UTurboSequence_NiagaraSettings_Lf()
{
}

FName UTurboSequence_NiagaraSettings_Lf::GetContainerName() const
{
	return Super::GetContainerName();
}

FName UTurboSequence_NiagaraSettings_Lf::GetCategoryName() const
{
	return TEXT("TurboSequence Lf Settings");
}

FName UTurboSequence_NiagaraSettings_Lf::GetSectionName() const
{
	return Super::GetSectionName();
}

#if WITH_EDITOR
FText UTurboSequence_NiagaraSettings_Lf::GetSectionText() const
{
	return LOCTEXT("UTurboSequence_RefSettings_Lf::GetSectionText", "Niagara");
}

FText UTurboSequence_NiagaraSettings_Lf::GetSectionDescription() const
{
	return LOCTEXT("UTurboSequence_RefSettings_Lf::GetSectionDescription",
	               "Turbo Sequence is using Niagara to render the Instances, here can change settings for the niagara system");
}
#endif

#if WITH_EDITOR
void UTurboSequence_NiagaraSettings_Lf::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!PropertyChangedEvent.Property)
	{
		return;
	}

	bool bEditedData = false;
	if (IsValid(FTurboSequence_Editor_LfModule::GlobalData))
	{
		if (NameNiagaraEmitter != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraEmitter.ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraEmitter = FName(NameNiagaraEmitter);
		}

		if (NameNiagaraMeshObject != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraMeshObject)
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraMeshObject = NameNiagaraMeshObject;
		}

		if (NameNiagaraMaterialObject != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraMaterialObject)
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraMaterialObject = NameNiagaraMaterialObject;
		}

		if (NameNiagaraParticleIDs != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleIDMap.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleIDMap = FName(
				NameNiagaraParticleIDs);
		}

		if (NameNiagaraParticleLocations != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleLocations.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleLocations = FName(
				NameNiagaraParticleLocations);
		}

		if (NameNiagaraParticleRotations != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleRotations.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleRotations = FName(
				NameNiagaraParticleRotations);
		}

		if (NameNiagaraParticleScales != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleScales.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleScales = FName(NameNiagaraParticleScales);
		}

		if (NameNiagaraLevelOfDetailIndex != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraLevelOfDetailIndex.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraLevelOfDetailIndex = FName(
				NameNiagaraLevelOfDetailIndex);
		}

		if (NameNiagaraParticleRemove != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleRemove.
			ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraParticleRemove = FName(NameNiagaraParticleRemove);
		}

		if (NameNiagaraCustomData != FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraCustomData.ToString())
		{
			bEditedData = true;
			FTurboSequence_Editor_LfModule::GlobalData->NameNiagaraCustomData = FName(NameNiagaraCustomData);
		}

		if (bEditedData)
		{
			FTurboSequence_Helper_Lf::SaveAsset(FTurboSequence_Editor_LfModule::GlobalData);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

#undef LOCTEXT_NAMESPACE
