// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "TurboSequence_NiagaraSettings_Lf.generated.h"

/**
 * 
 */
UCLASS(config = "TurboSequence_Lf")
class TURBOSEQUENCE_EDITOR_LF_API UTurboSequence_NiagaraSettings_Lf : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// Constructors / Destructors

	UTurboSequence_NiagaraSettings_Lf();
	virtual ~UTurboSequence_NiagaraSettings_Lf() override;

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

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Emitter Name"))
	FString NameNiagaraEmitter = FString("FXE_TurboSequence_Mesh_Unit_Lf");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Mesh Object Name"))
	FString NameNiagaraMeshObject = FString("User.Mesh_{0}");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Material Object Name"))
	FString NameNiagaraMaterialObject = FString("User.Material_{0}");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Particle ID Name"))
	FString NameNiagaraParticleIDs = FString("User.Particle_ID_To_Index");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Particle Position Name"))
	FString NameNiagaraParticleLocations = FString("User.Particle_Position");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Particle Rotation Name"))
	FString NameNiagaraParticleRotations = FString("User.Particle_Rotation");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Particle Scale Name"))
	FString NameNiagaraParticleScales = FString("User.Particle_Scale");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Level of Detail Name"))
	FString NameNiagaraLevelOfDetailIndex = FString("User.LevelOfDetail_Index");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Custom Data Name"))
	FString NameNiagaraCustomData = FString("User.CustomData");

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Niagara Particle Remove Name"))
	FString NameNiagaraParticleRemove = FString("User.Particle_Remove");
};
