// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimSequence.h"
#include "Animation/BlendSpace.h"
#include "UObject/Object.h"
#include "TurboSequence_AnimLibrary_Lf.generated.h"

USTRUCT()
struct TURBOSEQUENCE_LF_API FAnimationLibraryItem_Lf
{
	GENERATED_BODY()

	FAnimationLibraryItem_Lf()
	{
	}

	~FAnimationLibraryItem_Lf()
	{
	}

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TObjectPtr<UAnimSequence> Animation;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	FVector AnimationSeed_CM_Per_Second = FVector(600, 0, 0);
};


/**
 * 
 */
UCLASS()
class TURBOSEQUENCE_LF_API UTurboSequence_AnimLibrary_Lf : public UObject
{
	GENERATED_BODY()

public:
	UTurboSequence_AnimLibrary_Lf();

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FAnimationLibraryItem_Lf> Animations;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<TObjectPtr<UBlendSpace>> BlendSpaces;
};
