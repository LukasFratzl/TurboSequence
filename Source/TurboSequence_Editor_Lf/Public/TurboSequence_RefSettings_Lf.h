// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TurboSequence_RefSettings_Lf.generated.h"

/**
 * 
 */
UCLASS(config = "TurboSequence_Lf")
class TURBOSEQUENCE_EDITOR_LF_API UTurboSequence_RefSettings_Lf : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Constructors / Destructors

	UTurboSequence_RefSettings_Lf();
	virtual ~UTurboSequence_RefSettings_Lf() override;

	//~ Begin UDeveloperSettings Interface
	virtual FName GetContainerName() const override;
	virtual FName GetCategoryName() const override;
	virtual FName GetSectionName() const override;

#if WITH_EDITOR
	virtual FText GetSectionText() const override;
	virtual FText GetSectionDescription() const override;
#endif
	// End of UDeveloperSettings

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Default Rendering NiagaraSystem", ConfigRestartRequired = true), Category="TurboSequence")
	FString Default_Rendering_NiagaraSystem = FString(
		"/Script/Niagara.NiagaraSystem'/TurboSequence_Lf/Resources/FXS_TurboSequence_Mesh_Lf.FXS_TurboSequence_Mesh_Lf'");

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Default Rendering Transform Texture Current Frame", ConfigRestartRequired = true), Category="TurboSequence")
	FString Default_Rendering_TransformTexture_CurrentFrame = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_CurrentFrame_Lf.T_TurboSequence_TransformTexture_CurrentFrame_Lf'");

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Default Rendering Skin Weight Texture", ConfigRestartRequired = true), Category="TurboSequence")
	FString Default_Rendering_SkinWeightTexture = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_SkinWeightTexture_Lf.T_TurboSequence_SkinWeightTexture_Lf'");

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Default Rendering Transform Texture Previous Frame", ConfigRestartRequired = true), Category="TurboSequence")
	FString Default_Rendering_TransformTexture_PreviousFrame = FString(
		"/Script/Engine.TextureRenderTarget2DArray'/TurboSequence_Lf/Resources/T_TurboSequence_TransformTexture_PreviousFrame_Lf.T_TurboSequence_TransformTexture_PreviousFrame_Lf'");
};
