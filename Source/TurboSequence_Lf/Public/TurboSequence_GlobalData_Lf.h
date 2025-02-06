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
	FName NameNiagaraEmitter = FName("FXE_TurboSequence_Mesh_Unit_Lf");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraMeshObject = FString("User.Mesh_{0}");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FString NameNiagaraMaterialObject = FString("User.Material_{0}");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraParticleLocations = FName("User.Particle_Position");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraParticleIDMap = FName("User.Particle_ID_To_Index");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraParticleRotations = FName("User.Particle_Rotation");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraParticleScales = FName("User.Particle_Scale");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraLevelOfDetailIndex = FName("User.LevelOfDetail_Index");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraParticleRemove = FName("User.Particle_Remove");

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FName NameNiagaraCustomData = FName("User.CustomData");

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
