// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Factories/Factory.h"
#include "TurboSequence_MeshAssetFactory_Lf.generated.h"

class UEditorUtilityWidgetBlueprint;

/**
 * 
 */
UCLASS()
class TURBOSEQUENCE_EDITOR_LF_API UTurboSequence_MeshAssetFactory_Lf : public UFactory
{
	GENERATED_BODY()

public:
	UTurboSequence_MeshAssetFactory_Lf();
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	                                  UObject* Context, FFeedbackContext* Warn) override;
	virtual FString GetDefaultNewAssetName() const override;

	inline static TObjectPtr<UNiagaraSystem> DefaultAssetNiagaraSystem;
};
