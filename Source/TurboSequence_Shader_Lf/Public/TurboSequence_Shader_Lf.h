#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FTurboSequence_Shader_LfModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
