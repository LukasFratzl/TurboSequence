// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "TurboSequence_AnimLibraryFactory_Lf.generated.h"

class UEditorUtilityWidgetBlueprint;

/**
 * 
 */
UCLASS()
class TURBOSEQUENCE_EDITOR_LF_API UTurboSequence_AnimLibraryFactory_Lf : public UFactory
{
	GENERATED_BODY()

public:
	UTurboSequence_AnimLibraryFactory_Lf();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	virtual FString GetDefaultNewAssetName() const override;
};
