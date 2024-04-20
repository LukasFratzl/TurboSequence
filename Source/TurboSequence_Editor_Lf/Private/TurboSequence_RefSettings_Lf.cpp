// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_RefSettings_Lf.h"

// #include "TurboSequence_ControlWidget_Lf.h"
// #include "TurboSequence_Editor_Lf.h"
#include "Engine/DeveloperSettings.h"

#define LOCTEXT_NAMESPACE "TurboSequence Lf Path Settings"

UTurboSequence_RefSettings_Lf::UTurboSequence_RefSettings_Lf()
	: Super()

{
}

UTurboSequence_RefSettings_Lf::~UTurboSequence_RefSettings_Lf()
{
}

FName UTurboSequence_RefSettings_Lf::GetContainerName() const
{
	return Super::GetContainerName();
}

FName UTurboSequence_RefSettings_Lf::GetCategoryName() const
{
	return TEXT("TurboSequence Lf Settings");
}

FName UTurboSequence_RefSettings_Lf::GetSectionName() const
{
	return Super::GetSectionName();
}

#if WITH_EDITOR
FText UTurboSequence_RefSettings_Lf::GetSectionText() const
{
	return LOCTEXT("UTurboSequence_RefSettings_Lf::GetSectionText", "Reference Paths");
}

FText UTurboSequence_RefSettings_Lf::GetSectionDescription() const
{
	return LOCTEXT("UTurboSequence_RefSettings_Lf::GetSectionDescription",
	               "The engine load on startup assets for TurboSequence, here it can get defined the paths");
}
#endif

#if WITH_EDITOR
void UTurboSequence_RefSettings_Lf::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (!PropertyChangedEvent.Property)
	{
		return;
	}


	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

#undef LOCTEXT_NAMESPACE
