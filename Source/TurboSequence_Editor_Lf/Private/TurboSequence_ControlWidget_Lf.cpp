// Copyright Lukas Fratzl, 2G22-2G24. All Rights Reserved.


#include "TurboSequence_ControlWidget_Lf.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "InputCoreTypes.h"
#include "ObjectTools.h"
#include "TurboSequence_Editor_Lf.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "Engine/StaticMesh.h"

UTurboSequence_ControlWidget_Lf::UTurboSequence_ControlWidget_Lf()
{
}

UTurboSequence_ControlWidget_Lf::~UTurboSequence_ControlWidget_Lf()
{
}

void UTurboSequence_ControlWidget_Lf::OnTick(float DeltaTime)
{
	if (bNeedSaveMainAsset)
	{
		SaveTimerMainAsset -= DeltaTime;
		if (SaveTimerMainAsset < GET0_NUMBER)
		{
			bNeedSaveMainAsset = false;
			if (Main_Asset_To_Edit)
			{
				FTurboSequence_Helper_Lf::SaveAsset(Main_Asset_To_Edit);
			}
		}
	}

	// if (IsValid(FTurboSequence_Editor_LfModule::GlobalData))
	// {
	// 	bUseHighPrecisionAnimationMode = FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode;
	// }
}

void UTurboSequence_ControlWidget_Lf::OnAssignMainAsset()
{
	if (Main_Asset_To_Edit)
	{
		FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit,
		                                   EShow_ControlPanel_Objects_Lf::Asset_Object);

		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(
			EditorObjects, Main_Asset_To_Edit->ReferenceMeshNative, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		Current_SkeletalMesh_Reference = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(
			EditorObjects, Main_Asset_To_Edit->ReferenceMeshEdited,
			EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));

		LevelOfDetails = Main_Asset_To_Edit->InstancedMeshes;

		MaxNumberOfLODs = Main_Asset_To_Edit->MaxLevelOfDetails;
	}
	else // Reset the values that way
	{
		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(
			FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr,
			                                   EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		Current_SkeletalMesh_Reference = Cast<USkeletalMesh>(
			FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr,
			                                   EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));
	}
}

void UTurboSequence_ControlWidget_Lf::OnAssignLodZeroMeshAsset()
{
	if (Main_Asset_To_Edit)
	{
		Main_Asset_To_Edit->ReferenceMeshNative = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(
			EditorObjects, Current_SkeletalMesh_Reference_NoEdit, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		// Save
		FTurboSequence_Helper_Lf::SaveAsset(Main_Asset_To_Edit);
	}
	else
	{
		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(
			FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr,
			                                   EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		PrintMainAssetMissingWarning(TEXT("Skeletal Mesh Lod Zero"), TEXT("SkeletalMesh"));
	}
}

void UTurboSequence_ControlWidget_Lf::CacheSections(UScrollBox* WelcomeScrollBox, UScrollBox* ReferenceScrollBox,
                                                    UScrollBox* GenerateScrollBox, UScrollBox* TweaksScrollBox)
{
	AddSection(WelcomeScrollBox, EShow_ControlPanel_Section_Lf::Welcome);
	AddSection(ReferenceScrollBox, EShow_ControlPanel_Section_Lf::Reference);
	AddSection(GenerateScrollBox, EShow_ControlPanel_Section_Lf::Generate);
	AddSection(TweaksScrollBox, EShow_ControlPanel_Section_Lf::Tweaks);
}

void UTurboSequence_ControlWidget_Lf::CacheButtons(TArray<UButton*> WelcomeButton, UButton* ReferenceButton,
                                                   UButton* GenerateButton, UButton* TweaksButton, UButton* DocsButton,
                                                   UButton* DiscordButton, TArray<UButton*> PreviousSectionButtons,
                                                   TArray<UButton*> NextSectionButtons, UButton* CreateLodButton,
                                                   UButton* CreateGlobalTextureButton)
{
	for (UButton* Button : WelcomeButton)
	{
		AddButton(Button, EShow_ControlPanel_Buttons_Lf::Welcome);
		Button->OnClicked.AddDynamic(this, &UTurboSequence_ControlWidget_Lf::ShowHomeSection);
	}
	AddButton(ReferenceButton, EShow_ControlPanel_Buttons_Lf::Reference);
	AddButton(GenerateButton, EShow_ControlPanel_Buttons_Lf::Generate);
	AddButton(TweaksButton, EShow_ControlPanel_Buttons_Lf::Tweaks);

	AddButton(DocsButton, EShow_ControlPanel_Buttons_Lf::Docs);
	AddButton(DiscordButton, EShow_ControlPanel_Buttons_Lf::Discord);

	for (UButton* Button : PreviousSectionButtons)
	{
		AddButton(Button, EShow_ControlPanel_Buttons_Lf::PreviousSection);
		Button->OnClicked.AddDynamic(this, &UTurboSequence_ControlWidget_Lf::ShowPreviousSection);
	}
	for (UButton* Button : NextSectionButtons)
	{
		AddButton(Button, EShow_ControlPanel_Buttons_Lf::NextSection);
		Button->OnClicked.AddDynamic(this, &UTurboSequence_ControlWidget_Lf::ShowNextSection);
	}

	AddButton(CreateLodButton, EShow_ControlPanel_Buttons_Lf::CreateLod);
	AddButton(CreateGlobalTextureButton, EShow_ControlPanel_Buttons_Lf::TweakGlobalTexture);
}

void UTurboSequence_ControlWidget_Lf::CacheProperties(UPropertyViewBase* MainAsset,
                                                      UPropertyViewBase* LodZeroSkeletalMesh,
                                                      UPropertyViewBase* MaxLevelOfDetailsToGenerate,
                                                      UPropertyViewBase* TweakGlobalTextures)
{
	AddPropertyToArray(MainAsset, EShow_ControlPanel_Properties_Lf::Main_Asset);
	AddPropertyToArray(LodZeroSkeletalMesh, EShow_ControlPanel_Properties_Lf::Skeletal_Mesh);

	AddPropertyToArray(MaxLevelOfDetailsToGenerate, EShow_ControlPanel_Properties_Lf::MaxInstancedLevelOfDetails);

	AddPropertyToArray(TweakGlobalTextures, EShow_ControlPanel_Properties_Lf::TweakGlobalTextureSection);
}

void UTurboSequence_ControlWidget_Lf::CreateProperties()
{
	for (const TTuple<EShow_ControlPanel_Properties_Lf, TArray<TObjectPtr<UPropertyViewBase>>>& EditorProperties :
	     Properties)
	{
		for (const TObjectPtr<UPropertyViewBase>& Property : EditorProperties.Value)
		{
			Property->SetObject(this);
		}
	}
}

void UTurboSequence_ControlWidget_Lf::ShowSection(EShow_ControlPanel_Section_Lf SectionToShow)
{
	// Disable All sections
	DisableSections();

	// Enable Wanted section
	if (Sections.Contains(SectionToShow))
	{
		if (Sections[SectionToShow])
		{
			Sections[SectionToShow]->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// Enable All Buttons
	EnableButtons();

	// Disable Current Section Buttons
	if (Buttons.Contains(GetCommonButtonEnum(SectionToShow)))
	{
		const TArray<TObjectPtr<UButton>>& ButtonArray = Buttons[GetCommonButtonEnum(SectionToShow)];
		for (const TObjectPtr<UButton>& Button : ButtonArray)
		{
			if (Button)
			{
				Button->SetIsEnabled(false);
			}
		}
	}

	// Set the current section to keep track for the Next and Previous Section Buttons
	CurrentSectionWhichIsShowing = SectionToShow;
}

void UTurboSequence_ControlWidget_Lf::ShowHomeSection()
{
	ShowSection(EShow_ControlPanel_Section_Lf::Welcome);
}

void UTurboSequence_ControlWidget_Lf::ShowNextSection()
{
	EShow_ControlPanel_Section_Lf WantedSection = CurrentSectionWhichIsShowing;
	switch (CurrentSectionWhichIsShowing)
	{
	case EShow_ControlPanel_Section_Lf::Welcome:
		// Handle Welcome section
		WantedSection = EShow_ControlPanel_Section_Lf::Reference;
		break;
	case EShow_ControlPanel_Section_Lf::Reference:
		// Handle Reference section
		WantedSection = EShow_ControlPanel_Section_Lf::Generate;
		break;
	case EShow_ControlPanel_Section_Lf::Generate:
		// Handle Generate section
		WantedSection = EShow_ControlPanel_Section_Lf::Tweaks;
		break;
	case EShow_ControlPanel_Section_Lf::Tweaks:
		// Handle Tweaks section
		WantedSection = EShow_ControlPanel_Section_Lf::Reference;
		break;
	default:
		// Handle default case (None)
		break;
	}

	ShowSection(WantedSection);
}

void UTurboSequence_ControlWidget_Lf::ShowPreviousSection()
{
	EShow_ControlPanel_Section_Lf WantedSection = CurrentSectionWhichIsShowing;
	switch (CurrentSectionWhichIsShowing)
	{
	case EShow_ControlPanel_Section_Lf::Welcome:
		// Handle Welcome section
		WantedSection = EShow_ControlPanel_Section_Lf::Tweaks;
		break;
	case EShow_ControlPanel_Section_Lf::Reference:
		// Handle Reference section
		WantedSection = EShow_ControlPanel_Section_Lf::Tweaks;
		break;
	case EShow_ControlPanel_Section_Lf::Generate:
		// Handle Generate section
		WantedSection = EShow_ControlPanel_Section_Lf::Reference;
		break;
	case EShow_ControlPanel_Section_Lf::Tweaks:
		// Handle Tweaks section
		WantedSection = EShow_ControlPanel_Section_Lf::Generate;
		break;
	default:
		// Handle default case (None)
		break;
	}

	ShowSection(WantedSection);
}

void UTurboSequence_ControlWidget_Lf::OnGenerateButtonPressed()
{
	// TODO: Fix naming
	if (!Main_Asset_To_Edit)
	{
		PrintMainAssetMissingWarning(TEXT("LOD"), TEXT("LOD Item"));

		return;
	}

	if (!Main_Asset_To_Edit->ReferenceMeshNative)
	{
		PrintMainAssetMissingWarning(TEXT("Skeletal Mesh Lod Zero"), TEXT("SkeletalMesh"));

		return;
	}

	const FString DefaultPath = FPaths::ProjectContentDir();
	const FString Title = FString(TEXT("Directory for the LOD 0 Data"));
	FString DirectorySelected = FString("");
	FString WantedMeshName = FString("");
	FString WantedMeshPath = FString("");

	bool bEditedAnythingSuccessfully = false;

	if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
	{
		if (DesktopPlatform->OpenDirectoryDialog(nullptr, Title, DefaultPath, DirectorySelected))
		{
			DirectorySelected = FPaths::ConvertRelativePathToFull(DirectorySelected);

			WantedMeshPath = FPaths::Combine(DirectorySelected, Main_Asset_To_Edit->ReferenceMeshNative->GetName());
			WantedMeshPath = FString::Format(
				TEXT("{0}_TurboSequence_Instance{1}"), {*WantedMeshPath, *FPackageName::GetAssetPackageExtension()});
			WantedMeshName = FString(FString::Format(
				TEXT("{0}_TurboSequence_Instance"), {*Main_Asset_To_Edit->ReferenceMeshNative->GetName()}));


			if (FPaths::FileExists(WantedMeshPath))
			{
				// Create a confirmation dialog with "OK" and "Cancel" buttons
				const FText MessageText = FText::FromString(TEXT("Mesh Already Exists, do you want override it?"));
				const FText TitleText = FText::FromString(TEXT("Confirmation"));
				const EAppReturnType::Type ButtonClicked = FMessageDialog::Open(
					EAppMsgType::OkCancel, MessageText, TitleText);

				if (ButtonClicked == EAppReturnType::Ok)
				{
					// The user clicked "OK" on the confirmation dialog
					// Apply changes here
					UPackageTools::LoadPackage(*WantedMeshPath);
				}
				else
				{
					// The user clicked "Cancel" on the confirmation dialog
					// Revert changes here
					WantedMeshPath = FString("");
				}
			}
		}
	}

	if (!WantedMeshPath.IsEmpty() && Main_Asset_To_Edit && Main_Asset_To_Edit->ReferenceMeshNative)
	{
		if (MaxNumberOfLODs > 0)
		{
			TObjectPtr<USkeletalMesh> NewMesh = DuplicateSkeletalMesh(Main_Asset_To_Edit->ReferenceMeshNative,
			                                                          FName(FString::Format(
				                                                          TEXT("{0}_Reference"), {*WantedMeshName})),
			                                                          true);
			uint8 NumIterations = MaxNumberOfLODs / GET5_NUMBER + GET1_NUMBER;
			for (uint8 i = GET1_NUMBER; i <= NumIterations; ++i) // Note: Starting at 1 here
			{
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Running Mesh Reduction Iteration %d"), i);

				int32 MaxIterationNum = i * GET5_NUMBER;

				int32 ReductionLOD = FMath::Min(MaxIterationNum, MaxNumberOfLODs);
				GenerateSkeletalMeshLevelOfDetails(NewMesh, ReductionLOD);
			}

			FString ReferencePath = FPaths::Combine(DirectorySelected,
			                                        Main_Asset_To_Edit->ReferenceMeshNative->GetName());
			ReferencePath = FString::Format(
				TEXT("{0}_TurboSequence_Reference{1}"), {*ReferencePath, *FPackageName::GetAssetPackageExtension()});

			FTurboSequence_Helper_Lf::SaveNewAsset(NewMesh);

			Main_Asset_To_Edit->MaxLevelOfDetails = MaxNumberOfLODs;
			Current_SkeletalMesh_Reference = Main_Asset_To_Edit->ReferenceMeshEdited = Cast<USkeletalMesh>(
				FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, NewMesh,
				                                   EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));

			LevelOfDetails.Empty();
			TArray<FMeshDataOrderView_Lf> MeshDataOrderView;
			for (int32 i = GET0_NUMBER; i < MaxNumberOfLODs; ++i)
			{
				TArray<int32> StaticMeshIndices;
				if (TObjectPtr<UStaticMesh> StaticMesh = GenerateStaticMeshFromSkeletalMesh(
					NewMesh, i, WantedMeshPath,
					FString(FString::Format(TEXT("{0}_Lod_{1}"), {*WantedMeshName, *FString::FormatAsNumber(i)})),
					StaticMeshIndices))
				{
					FMeshItem_Lf Item = FMeshItem_Lf();
					if (i > GET9_NUMBER)
					{
						Item.bIsAnimated = false;
					}
					if (!i)
					{
						Item.bIsFrustumCullingEnabled = false;
					}
					Item.StaticMesh = StaticMesh;
					LevelOfDetails.Add(Item);


					FMeshDataOrderView_Lf Order = FMeshDataOrderView_Lf();
					Order.StaticMeshIndices = StaticMeshIndices;

					MeshDataOrderView.Add(Order);
				}
			}
			if (LevelOfDetails.Num())
			{
				Main_Asset_To_Edit->InstancedMeshes = LevelOfDetails;
				Main_Asset_To_Edit->MeshDataOrderView = MeshDataOrderView;

				if (GIsEditor)
				{
					FNotificationInfo Info(FText::FromString(FString(TEXT("Successfully Converted Meshes"))));
					Info.ExpireDuration = 8.0f;
					Info.bUseLargeFont = false;
					if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().
						AddNotification(Info); Notification.IsValid())
					{
						Notification->SetCompletionState(SNotificationItem::CS_Success);
					}
				}

				bEditedAnythingSuccessfully = true;
			}
		}
	}

	const TObjectPtr<UWorld> World = GetWorld();
	const TObjectPtr<UTurboSequence_MeshAsset_Lf> Asset = Main_Asset_To_Edit;

	FTurboSequence_Editor_LfModule::RepairMaxIterationCounter = GET0_NUMBER;
	auto RunnerFunction = [Asset, World](bool bSuccess)
	{
		if (!bSuccess)
		{
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Returning with ID=5"))
		}
		FTurboSequence_Editor_LfModule::RepairMeshAssetAsync2(Asset, World, false);
	};

	FTurboSequence_Utility_Lf::CreateRawSkinWeightTextureBuffer(Asset, RunnerFunction, World);


	if (bEditedAnythingSuccessfully && Main_Asset_To_Edit)
	{
		FTurboSequence_Helper_Lf::SaveAsset(Main_Asset_To_Edit);
	}
}

void UTurboSequence_ControlWidget_Lf::OnTweakingGlobalTextures()
{
	bool bEditedData = false;
	if (IsValid(FTurboSequence_Editor_LfModule::GlobalData) &&
		IsValid(FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame) &&
		IsValid(FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture) && IsValid(
			FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame))
	{
		constexpr uint16 Resolution = GET512_NUMBER;

		uint32 PowXY = Resolution * Resolution;

		uint64 MaxMeshes = MaxNumMeshes * MaxNumBones * FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer + GET2_NUMBER;

		uint16 NumSlicesTransformTexture = FMath::Min(
			FMath::CeilToInt(static_cast<float>(MaxMeshes / PowXY)) + GET1_NUMBER, 1023);

		uint8 TextureByteType = GET8_NUMBER;
		// if (bUseHighPrecisionAnimationMode)
		// {
		// 	TextureByteType = GET16_NUMBER;
		// }

		uint64 MaxNumTransformByte = PowXY * NumSlicesTransformTexture * TextureByteType;

		uint64 MaxBytes = MaxNumTransformByte;

		uint64 MaxMegaByte = MaxBytes / GET1024_NUMBER / GET1024_NUMBER;

		AverageAllocatedMemory = MaxMegaByte;


		if (!bManuallyAdjustTextureSize)
		{
			if ((FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->SizeX != Resolution ||
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->SizeY
					!= Resolution || NumSlicesTransformTexture != FTurboSequence_Editor_LfModule::GlobalData->
					                                              TransformTexture_CurrentFrame->Slices
					/* || FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode != bUseHighPrecisionAnimationMode*/)
				|| (FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->SizeX !=
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame->SizeX ||
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->SizeY
					!= FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame->SizeY ||
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame->Slices !=
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->Slices ||
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame->GetFormat() !=
					FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->GetFormat()))
			{
				const UPackage* CurrentPackage = FTurboSequence_Editor_LfModule::GlobalData->
				                                 TransformTexture_CurrentFrame->GetOutermost();
				const FString CurrentPackagePath = FPackageName::LongPackageNameToFilename(
					CurrentPackage->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*CurrentPackagePath);

				ETextureRenderTargetFormat Format = RTF_RGBA16f;
				// if (bUseHighPrecisionAnimationMode)
				// {
				// 	Format = RTF_RGBA32f;
				// }

				FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame =
					FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(
						CurrentPackagePath,
						FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_CurrentFrame->GetName(),
						Resolution, NumSlicesTransformTexture, GetPixelFormatFromRenderTargetFormat(Format));

				const UPackage* PreviousPackage = FTurboSequence_Editor_LfModule::GlobalData->
				                                  TransformTexture_PreviousFrame->GetOutermost();
				const FString PreviousPackagePath = FPackageName::LongPackageNameToFilename(
					PreviousPackage->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*PreviousPackagePath);

				FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame =
					FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(
						PreviousPackagePath,
						FTurboSequence_Editor_LfModule::GlobalData->TransformTexture_PreviousFrame->GetName(),
						Resolution, NumSlicesTransformTexture, GetPixelFormatFromRenderTargetFormat(Format));

				bEditedData = true;
			}

			if (bEditedData)
			{
				// if (FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode != bUseHighPrecisionAnimationMode)
				// {
				// 	FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode = bUseHighPrecisionAnimationMode;
				//
				// 	FTurboSequence_Helper_Lf::SaveAsset(FTurboSequence_Editor_LfModule::GlobalData);
				// }

				UE_LOG(LogTurboSequence_Lf, Display, TEXT("--------------------------------------------------"));
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Succesfully Applied new Render Settings...."));
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("--------------------------------------------------"));
			}
		}
		else
		{
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("--------------------------------------------------"));
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Switched to manual texture adjustment mode...."));
			UE_LOG(LogTurboSequence_Lf, Display, TEXT("--------------------------------------------------"));

			const UPackage* PackageTransformTexture = FTurboSequence_Editor_LfModule::GlobalData->
			                                          TransformTexture_CurrentFrame->GetOutermost();
			const FString PackagePathTransformTexture = FPackageName::LongPackageNameToFilename(
				PackageTransformTexture->GetName(), FPackageName::GetAssetPackageExtension());

			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Current Frame Transform Texture is at Path -> %s"),
			       *PackagePathTransformTexture);

			const UPackage* PackageTransformTexturePrevious = FTurboSequence_Editor_LfModule::GlobalData->
			                                                  TransformTexture_PreviousFrame->GetOutermost();
			const FString PackagePathTransformTexturePrevious = FPackageName::LongPackageNameToFilename(
				PackageTransformTexturePrevious->GetName(), FPackageName::GetAssetPackageExtension());

			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Previous Frame Transform Texture is at Path -> %s"),
			       *PackagePathTransformTexturePrevious);

			const UPackage* PackageSkinWeightTexture = FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->
				GetOutermost();
			const FString PackagePathSkinWeightTexture = FPackageName::LongPackageNameToFilename(
				PackageSkinWeightTexture->GetName(), FPackageName::GetAssetPackageExtension());

			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Skin Weight Texture is at Path -> %s"),
			       *PackagePathSkinWeightTexture);
		}
	}
}
