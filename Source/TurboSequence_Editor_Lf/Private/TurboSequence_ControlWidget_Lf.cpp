// Copyright Lukas Fratzl, 2GET0_NUMBER22-2GET0_NUMBER23. All Rights Reserved.


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
	// /Script/Engine.Material'/TurboSequence_Lf/Editor/Materials/M_TurboSequence_CharacterPreview_Material.M_TurboSequence_CharacterPreview_Material'

	// ConstructorHelpers::FObjectFinder<UMaterial> PreviewMaterialObjectFinder(TEXT("/Script/Engine.Material'/TurboSequence_Lf/Editor/Materials/M_TurboSequence_CharacterPreview_Material.M_TurboSequence_CharacterPreview_Material'"));
	// CharacterPreviewMaterial = PreviewMaterialObjectFinder.Object;
}

UTurboSequence_ControlWidget_Lf::~UTurboSequence_ControlWidget_Lf()
{
}

// void UTurboSequence_ControlWidget_Lf::CacheObjects(UStaticMesh* ViewportSkybox, UStaticMesh* ViewportGround, UStaticMesh* ViewportCharacterBoneSelectMesh, UMaterial* CharacterAdditiveMaterial, UMaterial* SelectionMaterial)
// {
// 	AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_Object, true);
// 	AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference, true);
// 	//AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_Skeleton, true);
//
// 	// AddObject(ViewportSkybox, EShow_ControlPanel_Objects_Lf::Viewport_Skybox);
// 	// AddObject(ViewportGround, EShow_ControlPanel_Objects_Lf::Viewport_Ground);
// 	// AddObject(ViewportCharacterBoneSelectMesh, EShow_ControlPanel_Objects_Lf::Viewport_Bone_Select_Mesh);
// 	// AddObject(CharacterAdditiveMaterial, EShow_ControlPanel_Objects_Lf::Viewport_Character_Material);
// 	// AddObject(SelectionMaterial, EShow_ControlPanel_Objects_Lf::Viewport_Selection_Material);
// 	//
// 	// AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_Lod_InstancedMesh, true);
// 	// // AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_SettingsTexture, true);
// 	// // AddObject(nullptr, EShow_ControlPanel_Objects_Lf::Asset_TransformTexture, true);
// }

// void UTurboSequence_ControlWidget_Lf::CacheParents(UPanelWidget* AutoRigBoneTreeParent, UPanelWidget* AutoRigLodListParent)
// {
// 	AddParent(AutoRigBoneTreeParent, EShow_ControlPanel_Parents_Lf::AutoRig_BoneTree);
// 	AddParent(AutoRigLodListParent, EShow_ControlPanel_Parents_Lf::AutoRig_LodSelect);
// }

void UTurboSequence_ControlWidget_Lf::OnTick(const float& DeltaTime)
{
	// for (TTuple<EShow_ControlPanel_Viewports_Lf, FViewportSectionPair_Lf>& Viewport : Viewports)
	// {
	// 	MoveViewportCamera(Viewport.Value, DeltaTime);
	// 	//UpdateClickableBoneMeshes(Viewport.Value.ClickableBonesMeshes);
	// }

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
}

void UTurboSequence_ControlWidget_Lf::OnAssignMainAsset()
{
	if (Main_Asset_To_Edit)
	{
		FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit, EShow_ControlPanel_Objects_Lf::Asset_Object);

		//FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->ReferenceSkeleton, EShow_ControlPanel_Objects_Lf::Asset_Skeleton);

		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->ReferenceMeshNative, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		Current_SkeletalMesh_Reference = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->ReferenceMeshEdited, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));

		//Current_Lod_Zero_Instanced_Mesh = Cast<UStaticMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->Lod_Zero_Instanced_Mesh, EShow_ControlPanel_Objects_Lf::Asset_Lod_InstancedMesh));
		LevelOfDetails = Main_Asset_To_Edit->InstancedMeshes;

		// Current_SettingsTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->AnimationFramesTexture, EShow_ControlPanel_Objects_Lf::Asset_SettingsTexture));
		//
		// Current_SkinWeightTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->SkinWeightTexture, EShow_ControlPanel_Objects_Lf::Asset_SkinWeightTexture));
		//
		// Current_TransformTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->TransformTexture, EShow_ControlPanel_Objects_Lf::Asset_TransformTexture));

		//Current_RenderDataTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Main_Asset_To_Edit->RenderDataTexture, EShow_ControlPanel_Objects_Lf::Asset_RenderDataTexture));

		MaxNumberOfLODs = Main_Asset_To_Edit->MaxLevelOfDetails;

		// Set up Viewports ready
		//ConstructViewportCharacters();
	}
	else // Reset the values that way
	{
		//FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_Skeleton);

		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		Current_SkeletalMesh_Reference = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));

		// Current_SettingsTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_SettingsTexture));
		//
		// Current_SkinWeightTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_SkinWeightTexture));
		//
		// Current_TransformTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_TransformTexture));

		//Current_RenderDataTexture = Cast<UTextureRenderTarget2DArray>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_RenderDataTexture));
	}
}

void UTurboSequence_ControlWidget_Lf::OnAssignLodZeroMeshAsset()
{
	if (Main_Asset_To_Edit)
	{
		Main_Asset_To_Edit->ReferenceMeshNative = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Current_SkeletalMesh_Reference_NoEdit, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		// Save
		FTurboSequence_Helper_Lf::SaveAsset(Main_Asset_To_Edit);
		//bNeedSaveMainAsset = true;

		// Set up Viewports ready
		//ConstructViewportCharacters();
	}
	else
	{
		Current_SkeletalMesh_Reference_NoEdit = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, nullptr, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh));

		PrintMainAssetMissingWarning(TEXT("Skeletal Mesh Lod Zero"), TEXT("SkeletalMesh"));
	}
}

void UTurboSequence_ControlWidget_Lf::CacheSections(UScrollBox* WelcomeScrollBox, UScrollBox* ReferenceScrollBox, UScrollBox* GenerateScrollBox, UScrollBox* TweaksScrollBox)
{
	AddSection(WelcomeScrollBox, EShow_ControlPanel_Section_Lf::Welcome);
	AddSection(ReferenceScrollBox, EShow_ControlPanel_Section_Lf::Reference);
	AddSection(GenerateScrollBox, EShow_ControlPanel_Section_Lf::Generate);
	//AddSection(AutoRigScrollBox, EShow_ControlPanel_Section_Lf::Auto_Rig);
	//AddSection(OtherLodScrollBox, EShow_ControlPanel_Section_Lf::Other_Lod);
	AddSection(TweaksScrollBox, EShow_ControlPanel_Section_Lf::Tweaks);
}

void UTurboSequence_ControlWidget_Lf::CacheButtons(TArray<UButton*> WelcomeButton, UButton* ReferenceButton, UButton* GenerateButton, UButton* TweaksButton, UButton* DocsButton, UButton* DiscordButton, TArray<UButton*> PreviousSectionButtons, TArray<UButton*> NextSectionButtons, UButton* CreateLodButton, UButton* CreateGlobalTextureButton)
{
	for (UButton* Button : WelcomeButton)
	{
		AddButton(Button, EShow_ControlPanel_Buttons_Lf::Welcome);
		Button->OnClicked.AddDynamic(this, &UTurboSequence_ControlWidget_Lf::ShowHomeSection);
	}
	AddButton(ReferenceButton, EShow_ControlPanel_Buttons_Lf::Reference);
	AddButton(GenerateButton, EShow_ControlPanel_Buttons_Lf::Generate);
	//AddButton(AutoRigButton, EShow_ControlPanel_Buttons_Lf::Auto_Rig);
	//AddButton(OtherLodButton, EShow_ControlPanel_Buttons_Lf::Other_Lod);
	AddButton(TweaksButton, EShow_ControlPanel_Buttons_Lf::Tweaks);

	//AddButton(ManualSaveButton, EShow_ControlPanel_Buttons_Lf::ManualSave);
	AddButton(DocsButton, EShow_ControlPanel_Buttons_Lf::Docs);
	AddButton(DiscordButton, EShow_ControlPanel_Buttons_Lf::Discord);
	//AddButton(VideosButton, EShow_ControlPanel_Buttons_Lf::Videos);
	//AddButton(EMailButton, EShow_ControlPanel_Buttons_Lf::EMail);

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

	//AddButton(TutorialAutoRig, EShow_ControlPanel_Buttons_Lf::Tutorial_AutoRig);

	AddButton(CreateLodButton, EShow_ControlPanel_Buttons_Lf::CreateLod);
	AddButton(CreateGlobalTextureButton, EShow_ControlPanel_Buttons_Lf::TweakGlobalTexture);
}

void UTurboSequence_ControlWidget_Lf::CacheProperties(UPropertyViewBase* MainAsset, UPropertyViewBase* LodZeroSkeletalMesh, UPropertyViewBase* MaxLevelOfDetailsToGenerate, UPropertyViewBase* TweakGlobalTextures)
{
	AddPropertyToArray(MainAsset, EShow_ControlPanel_Properties_Lf::Main_Asset);
	AddPropertyToArray(LodZeroSkeletalMesh, EShow_ControlPanel_Properties_Lf::Skeletal_Mesh);

	// for (UPropertyViewBase* Property : ViewportSelectionModes)
	// {
	// 	AddPropertyToArray(Property, EShow_ControlPanel_Properties_Lf::Viewport_Selection_Mode);
	// }
	// for (UPropertyViewBase* Property : ViewportCameraMoveSpeeds)
	// {
	// 	AddPropertyToArray(Property, EShow_ControlPanel_Properties_Lf::Viewport_Camera_Move_Speed);
	// }
	// for (UPropertyViewBase* Property : ViewportCameraRotateSpeeds)
	// {
	// 	AddPropertyToArray(Property, EShow_ControlPanel_Properties_Lf::Viewport_Camera_Rotate_Speed);
	// }
	// for (UPropertyViewBase* Property : ViewportBoneSizeIndicators)
	// {
	// 	AddPropertyToArray(Property, EShow_ControlPanel_Properties_Lf::Viewport_BoneSize_Indicator);
	// }
	//
	// for (UPropertyViewBase* Property : LodProperties)
	// {
	// 	AddPropertyToArray(Property, EShow_ControlPanel_Properties_Lf::Lod);
	// }
	//
	// AddPropertyToArray(AutoRigBoneAssignNameErrorProperty, EShow_ControlPanel_Properties_Lf::AutoRigBoneAssignErrorProperty);

	AddPropertyToArray(MaxLevelOfDetailsToGenerate, EShow_ControlPanel_Properties_Lf::MaxInstancedLevelOfDetails);
	// AddPropertyToArray(InstancedLevelOfDetails, EShow_ControlPanel_Properties_Lf::Lod_InstancedMesh);
	// AddPropertyToArray(SettingsTexture, EShow_ControlPanel_Properties_Lf::SettingsTexture);
	// AddPropertyToArray(SkinWeightTexture, EShow_ControlPanel_Properties_Lf::SkinWeightTexture);
	// AddPropertyToArray(TransformTexture, EShow_ControlPanel_Properties_Lf::TransformTexture);

	AddPropertyToArray(TweakGlobalTextures, EShow_ControlPanel_Properties_Lf::TweakGlobalTextureSection);
}

void UTurboSequence_ControlWidget_Lf::CreateProperties()
{
	for (const TTuple<EShow_ControlPanel_Properties_Lf, TArray<TObjectPtr<UPropertyViewBase>>>& EditorProperties : Properties)
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
		for (const TObjectPtr<UButton> Button : ButtonArray)
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
			WantedMeshPath = FString::Format(TEXT("{0}_TurboSequence_Instance{1}"), {*WantedMeshPath, *FPackageName::GetAssetPackageExtension()});
			WantedMeshName = FString(FString::Format(TEXT("{0}_TurboSequence_Instance"), {*Main_Asset_To_Edit->ReferenceMeshNative->GetName()}));


			if (FPaths::FileExists(WantedMeshPath))
			{
				// Create a confirmation dialog with "OK" and "Cancel" buttons
				const FText MessageText = FText::FromString(TEXT("Mesh Already Exists, do you want override it?"));
				const FText TitleText = FText::FromString(TEXT("Confirmation"));
				const EAppReturnType::Type ButtonClicked = FMessageDialog::Open(EAppMsgType::OkCancel, MessageText, TitleText);

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
			TObjectPtr<USkeletalMesh> NewMesh = DuplicateSkeletalMesh(Main_Asset_To_Edit->ReferenceMeshNative, FName(FString::Format(TEXT("{0}_Reference"), {*WantedMeshName})), true);
			const uint8& NumIterations = MaxNumberOfLODs / GET5_NUMBER + GET1_NUMBER;
			for (uint8 i = GET1_NUMBER; i <= NumIterations; ++i) // Note: Starting at 1 here
			{
				UE_LOG(LogTurboSequence_Lf, Display, TEXT("Running Mesh Reduction Iteration %d"), i);

				const int32& MaxIterationNum = i * GET5_NUMBER;

				const int32& ReductionLOD = FMath::Min(MaxIterationNum, MaxNumberOfLODs);
				GenerateSkeletalMeshLevelOfDetails(NewMesh, ReductionLOD);
			}

			FString ReferencePath = FPaths::Combine(DirectorySelected, Main_Asset_To_Edit->ReferenceMeshNative->GetName());
			ReferencePath = FString::Format(TEXT("{0}_TurboSequence_Reference{1}"), {*ReferencePath, *FPackageName::GetAssetPackageExtension()});

			FTurboSequence_Helper_Lf::SaveNewAsset(NewMesh);

			//Main_Asset_To_Edit->ReferenceSkeleton = Cast<USkeleton>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, NewMesh->GetSkeleton(), EShow_ControlPanel_Objects_Lf::Asset_Skeleton));
			Main_Asset_To_Edit->MaxLevelOfDetails = MaxNumberOfLODs;
			Current_SkeletalMesh_Reference = Main_Asset_To_Edit->ReferenceMeshEdited = Cast<USkeletalMesh>(FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, NewMesh, EShow_ControlPanel_Objects_Lf::Asset_SkeletalMesh_Reference));

			LevelOfDetails.Empty();
			TArray<FMeshDataOrderView_Lf> MeshDataOrderView;
			for (int32 i = GET0_NUMBER; i < MaxNumberOfLODs; ++i)
			{
				TArray<int32> StaticMeshIndices;
				if (TObjectPtr<UStaticMesh> StaticMesh = GenerateStaticMeshFromSkeletalMesh(NewMesh, i, WantedMeshPath, FString(FString::Format(TEXT("{0}_Lod_{1}"), {*WantedMeshName, *FString::FormatAsNumber(i)})), StaticMeshIndices))
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
					//CreateVertexMaps(Main_Asset_To_Edit, i, Item.VertexData, Item.BoneIndices);
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
					if (TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(Info); Notification.IsValid())
					{
						Notification->SetCompletionState(SNotificationItem::CS_Success);
					}
				}

				bEditedAnythingSuccessfully = true;
			}

			//TArray<FAssetData> DeleteData;
			//DeleteData.Add(NewMesh->GetOuter());
			//ObjectTools::DeleteAssets(DeleteData);
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
	if (IsValid(FTurboSequence_Editor_LfModule::GlobalData) && IsValid(FTurboSequence_Editor_LfModule::GlobalData->TransformTexture) && IsValid(FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture)/* && IsValid(FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture)*/)
	{
		constexpr uint16 Resolution = GET512_NUMBER;

		//constexpr uint16 DataTextureResolution = GET128_NUMBER;

		const uint32& PowXY = Resolution * Resolution;

		//const uint32& DataTexturePowXY = DataTextureResolution * DataTextureResolution;

		const uint64& MaxMeshes = MaxNumMeshes * MaxNumBones * FTurboSequence_Helper_Lf::NumGPUTextureBoneBuffer + GET2_NUMBER;

		const uint16& NumSlicesTransformTexture = FMath::Min(FMath::CeilToInt(static_cast<float>(MaxMeshes / PowXY)) + GET1_NUMBER, 1023);

		uint8 TextureByteType = GET8_NUMBER;
		if (bUseHighPrecisionAnimationMode)
		{
			TextureByteType = GET16_NUMBER;
		}

		const uint64& MaxNumTransformByte = PowXY * NumSlicesTransformTexture * TextureByteType;

		//const uint64& NumSkinWeightPixels = AverageVertexCount * MaxNumUniqueMeshes * FTurboSequence_Helper_Lf::NumSkinWeightPixels;
		//const uint16& NumSlicesSkinWeightTexture = FMath::Min(FMath::CeilToInt(static_cast<float>(NumSkinWeightPixels / PowXY)) + GET1_NUMBER, 1023);

		//const uint64& MaxCustomData = MaxNumMeshes * FTurboSequence_Helper_Lf::NumInstanceCustomDataPixel;

		//const uint16& NumSlicesCustomData = FMath::Min(FMath::CeilToInt(static_cast<float>(MaxCustomData / DataTexturePowXY)) + GET1_NUMBER, 1023);

		//const uint64& MaxNumCustomDataByte = PowXY * NumSlicesCustomData * GET16_NUMBER;

		//const uint64& MaxNumSkinWeightByte = PowXY * NumSlicesSkinWeightTexture * GET8_NUMBER;

		//const uint64& MaxNumMeshDataBytes = AverageVertexCount * MaxNumUniqueMeshes * GET32_NUMBER;

		const uint64& MaxBytes = MaxNumTransformByte;

		const uint64& MaxMegaByte = MaxBytes / GET1024_NUMBER / GET1024_NUMBER;

		AverageAllocatedMemory = MaxMegaByte;


		if (!bManuallyAdjustTextureSize)
		{
			if (FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->SizeX != Resolution || FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->SizeY
				!= Resolution || NumSlicesTransformTexture != FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->Slices || FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode != bUseHighPrecisionAnimationMode)
			{
				const UPackage* Package = FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->GetOutermost();
				const FString PackagePath = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
				UPackageTools::LoadPackage(*PackagePath);

				ETextureRenderTargetFormat Format = RTF_RGBA16f;
				if (bUseHighPrecisionAnimationMode)
				{
					Format = RTF_RGBA32f;
				}

				FTurboSequence_Editor_LfModule::GlobalData->TransformTexture = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(PackagePath, FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->GetName(), Resolution, NumSlicesTransformTexture, Format);

				bEditedData = true;
			}

			// if (FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->SizeX != Resolution || FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->SizeY != Resolution || NumSlicesSkinWeightTexture != FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->Slices)
			// {
			// 	const UPackage* Package = FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->GetOutermost();
			// 	const FString PackagePath = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
			// 	UPackageTools::LoadPackage(*PackagePath);
			// 	FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(PackagePath, FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->GetName(), Resolution, NumSlicesSkinWeightTexture, RTF_RGBA16f);
			//
			// 	bEditedData = true;
			// }


			// const uint64& CustomDataNum = MaxNumMeshes * FTurboSequence_Helper_Lf::NumInstanceCustomDataPixel;
			//
			// const uint16& SlicesCustomData = FMath::Min(FMath::CeilToInt(static_cast<float>(CustomDataNum / DataTexturePowXY)) + GET1_NUMBER, 1023);
			//
			// if (FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->SizeX != Resolution || FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->SizeY != Resolution || SlicesCustomData != FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->Slices)
			// {
			// 	const UPackage* Package = FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->GetOutermost();
			// 	const FString PackagePath = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
			// 	UPackageTools::LoadPackage(*PackagePath);
			//
			// 	FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture = FTurboSequence_Helper_Lf::GenerateBlankRenderTargetArray(PackagePath, FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->GetName(), DataTextureResolution, SlicesCustomData, RTF_RGBA16f);
			//
			// 	bEditedData = true;
			// }
			if (bEditedData)
			{
				if (FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode != bUseHighPrecisionAnimationMode)
				{
					FTurboSequence_Editor_LfModule::GlobalData->bUseHighPrecisionAnimationMode = bUseHighPrecisionAnimationMode;

					FTurboSequence_Helper_Lf::SaveAsset(FTurboSequence_Editor_LfModule::GlobalData);
				}

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

			const UPackage* PackageTransformTexture = FTurboSequence_Editor_LfModule::GlobalData->TransformTexture->GetOutermost();
			const FString PackagePathTransformTexture = FPackageName::LongPackageNameToFilename(PackageTransformTexture->GetName(), FPackageName::GetAssetPackageExtension());

			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Transform Texture is at Path -> %s"), *PackagePathTransformTexture);

			const UPackage* PackageSkinWeightTexture = FTurboSequence_Editor_LfModule::GlobalData->SkinWeightTexture->GetOutermost();
			const FString PackagePathSkinWeightTexture = FPackageName::LongPackageNameToFilename(PackageSkinWeightTexture->GetName(), FPackageName::GetAssetPackageExtension());

			UE_LOG(LogTurboSequence_Lf, Display, TEXT("Skin Weight Texture is at Path -> %s"), *PackagePathSkinWeightTexture);

			// const UPackage* PackageDataTexture = FTurboSequence_Editor_LfModule::GlobalData->CustomDataTexture->GetOutermost();
			// const FString PackagePathDataTexture = FPackageName::LongPackageNameToFilename(PackageDataTexture->GetName(), FPackageName::GetAssetPackageExtension());
			//
			// UE_LOG(LogTurboSequence_Lf, Display, TEXT("Custom Data Texture is at Path -> %s"), *PackagePathDataTexture);
		}
	}
}
