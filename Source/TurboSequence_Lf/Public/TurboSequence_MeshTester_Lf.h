// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TurboSequence_MinimalData_Lf.h"
#include "GameFramework/Actor.h"
#include "TurboSequence_MeshTester_Lf.generated.h"

UCLASS()
class TURBOSEQUENCE_LF_API ATurboSequence_MeshTester_Lf : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATurboSequence_MeshTester_Lf();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The spawn data to give spawn info in the Actor Details panel
	FTurboSequence_MeshSpawnData_Lf SpawnData;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The Animation to Play
	TObjectPtr<UAnimSequence> MeshAnimation;

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	// The Animation settings to play
	FTurboSequence_AnimPlaySettings_Lf MeshAnimationSettings = FTurboSequence_AnimPlaySettings_Lf();

	UPROPERTY(EditAnywhere, Category="Mesh Tester")
	FTurboSequence_UpdateContext_Lf MeshUpdateContext = FTurboSequence_UpdateContext_Lf();

	void TestMesh(float DeltaTime);
	FTurboSequence_MeshSpawnData_Lf LastSpawnData;

	UPROPERTY(VisibleAnywhere, Category="Mesh Tester")
	FTurboSequence_MinimalMeshData_Lf CurrentMeshID;

	UPROPERTY(VisibleAnywhere, Category="Mesh Tester")
	FTurboSequence_AnimMinimalCollection_Lf CurrentAnimationID;
};
