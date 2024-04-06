// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_GlobalData_Lf.h"
#include "Modules/ModuleManager.h"

class FTurboSequence_LfModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnFilesLoaded();

	//inline static TObjectPtr<UTurboSequence_GlobalData_Lf> GlobalData;
};
