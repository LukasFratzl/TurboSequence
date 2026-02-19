// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_ComputeShaders_Lf.h"
#include "TurboSequence_Helper_Lf.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataAsset.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "TurboSequence_GlobalData_Lf.generated.h"

/**
 * 
 */
UCLASS()
class TURBOSEQUENCE_LF_API UTurboSequence_GlobalData_Lf : public UObject
{
public:
	GENERATED_BODY()

	UTurboSequence_GlobalData_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraEmitter = FString("FXE_TurboSequence_Mesh_Unit_Lf");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraMeshObject = FString("User.Mesh_{0}");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraMaterialObject = FString("User.Material_{0}");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraParticleLocations = FString("User.Particle_Position");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraParticleIDMap = FString("User.Particle_ID_To_Index");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraParticleRotations = FString("User.Particle_Rotation");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraParticleScales = FString("User.Particle_Scale");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraLevelOfDetailIndex = FString("User.LevelOfDetail_Index");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraParticleRemove = FString("User.Particle_Remove");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraCustomData = FString("User.CustomData");
	
	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraUseNanite = FString("User.TS_UseNanite");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTextureRenderTarget2DArray> TransformTexture_CurrentFrame;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTextureRenderTarget2DArray> TransformTexture_PreviousFrame;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTextureRenderTarget2DArray> SkinWeightTexture;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UTextureRenderTarget2DArray> AnimationLibraryTexture;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	bool bUseHighPrecisionAnimationMode = true;

	FSettingsComputeShader_Params_Lf CachedMeshDataCreationSettingsParams;
};
