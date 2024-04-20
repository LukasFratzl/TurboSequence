// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.


#include "TurboSequence_AnimLibraryFactory_Lf.h"

#include "TurboSequence_AnimLibrary_Lf.h"


UTurboSequence_AnimLibraryFactory_Lf::UTurboSequence_AnimLibraryFactory_Lf()
{
	SupportedClass = UTurboSequence_AnimLibrary_Lf::StaticClass();
	bCreateNew = true;
}

UObject* UTurboSequence_AnimLibraryFactory_Lf::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name,
                                                                EObjectFlags Flags, UObject* Context,
                                                                FFeedbackContext* Warn)
{
	const TObjectPtr<UTurboSequence_AnimLibrary_Lf> Asset = NewObject<UTurboSequence_AnimLibrary_Lf>(
		InParent, Class, Name, Flags, Context);

	return Asset; //NewObject<UTurboSequence_MeshAsset_Lf>(InParent, Class, Name, Flags, Context);
}

FString UTurboSequence_AnimLibraryFactory_Lf::GetDefaultNewAssetName() const
{
	return FString(TEXT("TS_AnimLibrary"));
}
