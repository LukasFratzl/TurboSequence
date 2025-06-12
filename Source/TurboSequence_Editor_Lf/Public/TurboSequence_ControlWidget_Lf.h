// Copyright Lukas Fratzl, 2022-2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"

#include "TurboSequence_MeshAsset_Lf.h"

#include "LODUtilities.h"
#include "MeshDescription.h"
#include "PackageTools.h"
#include "RawMesh.h"
#include "StaticMeshOperations.h"
#include "TurboSequence_Helper_Lf.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/ScrollBox.h"
#include "Components/SinglePropertyView.h"
#include "Components/TextBlock.h"
#include "Components/Viewport.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Blueprint/WidgetTree.h"
#include "UObject/SavePackage.h"

#include "Components/PoseableMeshComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "BoneContainer.h"
#include "SkeletalMeshAttributes.h"
#include "SkeletalMeshTypes.h"
#include "TurboSequence_Utility_Lf.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SkinnedAssetAsyncCompileUtils.h"
#include "Engine/SkinnedAssetCommon.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Rendering/SkeletalMeshRenderData.h"

#include "TurboSequence_ControlWidget_Lf.generated.h"


UENUM(BlueprintType)
enum class EShow_ControlPanel_Section_Lf : uint8
{
	None,
	Welcome,
	Reference,
	Generate,
	Tweaks
};

UENUM(BlueprintType)
enum class EShow_ControlPanel_Buttons_Lf : uint8
{
	None,
	Welcome,
	Reference,
	Generate,
	Tweaks,
	NextSection,
	PreviousSection,
	Docs,
	Discord,
	CreateLod,
	TweakGlobalTexture
};

UENUM(BlueprintType)
enum class EShow_ControlPanel_Objects_Lf : uint8
{
	None,
	Asset_Object,
	Asset_SkeletalMesh,
	Asset_SkeletalMesh_Reference
};

UENUM(BlueprintType)
enum class EShow_ControlPanel_Properties_Lf : uint8
{
	None,
	Main_Asset,
	Skeletal_Mesh,
	Skeletal_Mesh_Reference,
	MaxInstancedLevelOfDetails,
	MeshDataModeToGenerate,
	TweakGlobalTextureSection
};

/**
 * 
 */
UCLASS(BlueprintType)
class TURBOSEQUENCE_EDITOR_LF_API UTurboSequence_ControlWidget_Lf : public UEditorUtilityWidget
{
	GENERATED_BODY()

	/*	=======================================================================================================
												STATIC VALUES
	===========================================================================================================	*/

	inline static const FLinearColor NonSelectionColor = FLinearColor::Yellow;
	inline static const FLinearColor SelectionColor = FLinearColor::Blue;

	inline static const FName ColorName = FName("Color");
	inline static const FName AlphaName = FName("Alpha");

	inline static const FName DebugColorName = FName("DebugColor");

	inline static bool bNeedSaveMainAsset = false;
	inline static float SaveTimerMainAsset = GET1_NUMBER;

	/*	=======================================================================================================
												UI FUNCTIONALITY
	===========================================================================================================	*/
public:
	UTurboSequence_ControlWidget_Lf();
	virtual ~UTurboSequence_ControlWidget_Lf() override;

	TMap<EShow_ControlPanel_Objects_Lf, TObjectPtr<UObject>> EditorObjects;

	FORCEINLINE_DEBUGGABLE void AddObject(const TObjectPtr<UObject> Object,
	                                      const EShow_ControlPanel_Objects_Lf& Category, bool SuppressInfo = false)
	{
		if (!Object && !SuppressInfo)
		{
			UE_LOG(LogTurboSequence_Lf, Warning,
			       TEXT("Missing Object in the Editor Control Panel UI, this should not happen ...., Category -> %s"),
			       *UEnum::GetValueAsString(Category));
		}
		FTurboSequence_Helper_Lf::SetOrAdd(EditorObjects, Object, Category);
	}

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void OnTick(float DeltaTime);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TObjectPtr<UTurboSequence_MeshAsset_Lf> Main_Asset_To_Edit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TObjectPtr<USkeletalMesh> Current_SkeletalMesh_Reference_NoEdit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = "1", ClampMax = "32"), Category="TurboSequence")
	int32 MaxNumberOfLODs = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	ETurboSequence_MeshDataMode_Lf MeshDataMode = ETurboSequence_MeshDataMode_Lf::UV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="TurboSequence")
	TObjectPtr<USkeletalMesh> Current_SkeletalMesh_Reference;

	UPROPERTY(EditAnywhere, Category="TurboSequence")
	TArray<FMeshItem_Lf> LevelOfDetails;

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void OnAssignMainAsset();

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void OnAssignLodZeroMeshAsset();

	static FORCEINLINE_DEBUGGABLE void PrintMainAssetMissingWarning(const FName& ItemName, const FName& ItemType)
	{
		UE_LOG(LogTurboSequence_Lf, Warning,
		       TEXT(
			       "Trying to set %s without having the Main Asset Assigned ( This one in the Reference Section ), the progress won't save, please assign the Main Asset and set the %s again..."
		       ), *ItemName.ToString(), *ItemType.ToString());
	}

	static FORCEINLINE_DEBUGGABLE void SaveAssetByTimer(float TimeInSeconds)
	{
		bNeedSaveMainAsset = true;
		SaveTimerMainAsset = TimeInSeconds;
	}

	TMap<EShow_ControlPanel_Section_Lf, TObjectPtr<UScrollBox>> Sections;
	TMap<EShow_ControlPanel_Buttons_Lf, TArray<TObjectPtr<UButton>>> Buttons;
	TMap<EShow_ControlPanel_Properties_Lf, TArray<TObjectPtr<UPropertyViewBase>>> Properties;

	EShow_ControlPanel_Section_Lf CurrentSectionWhichIsShowing = EShow_ControlPanel_Section_Lf::Reference;

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void CacheSections(UScrollBox* WelcomeScrollBox, UScrollBox* ReferenceScrollBox, UScrollBox* GenerateScrollBox,
	                   UScrollBox* TweaksScrollBox);

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void CacheButtons(TArray<UButton*> WelcomeButton, UButton* ReferenceButton, UButton* GenerateButton,
	                  UButton* TweaksButton, UButton* DocsButton, UButton* DiscordButton,
	                  TArray<UButton*> PreviousSectionButtons, TArray<UButton*> NextSectionButtons,
	                  UButton* CreateLodButton, UButton* CreateGlobalTextureButton);

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void CacheProperties(UPropertyViewBase* MainAsset, UPropertyViewBase* LodZeroSkeletalMesh,
	                     UPropertyViewBase* MaxLevelOfDetailsToGenerate, UPropertyViewBase* MeshDataModeToGenerate,
	                     UPropertyViewBase* TweakGlobalTextures);

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void CreateProperties();

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void ShowSection(EShow_ControlPanel_Section_Lf SectionToShow);

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void ShowHomeSection();

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void ShowNextSection();

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void ShowPreviousSection();

	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void OnGenerateButtonPressed();


	FORCEINLINE_DEBUGGABLE void AddSection(const TObjectPtr<UScrollBox> Section,
	                                       const EShow_ControlPanel_Section_Lf& Category)
	{
		if (Section && !Sections.Contains(Category))
		{
			Sections.Add(Category, Section);
		}
	}

	FORCEINLINE_DEBUGGABLE void AddButton(const TObjectPtr<UButton> Button,
	                                      const EShow_ControlPanel_Buttons_Lf& Category)
	{
		if (Button)
		{
			const bool Contains = Buttons.Contains(Category);
			if (!Contains)
			{
				TArray<TObjectPtr<UButton>> ButtonArray;
				ButtonArray.Add(Button);
				Buttons.Add(Category, ButtonArray);
			}

			if (Contains && !Buttons[Category].Contains(Button))
			{
				Buttons[Category].Add(Button);
			}
		}
	}

	FORCEINLINE_DEBUGGABLE void AddPropertyToArray(const TObjectPtr<UPropertyViewBase> Property,
	                                               const EShow_ControlPanel_Properties_Lf& Category)
	{
		if (Property)
		{
			const bool Contains = Properties.Contains(Category);
			if (!Contains)
			{
				TArray<TObjectPtr<UPropertyViewBase>> PropertyViewsArray;
				PropertyViewsArray.Add(Property);
				Properties.Add(Category, PropertyViewsArray);
			}

			if (Contains && !Properties[Category].Contains(Property))
			{
				Properties[Category].Add(Property);
			}
		}
	}

	FORCEINLINE_DEBUGGABLE void DisableSections()
	{
		for (const TTuple<EShow_ControlPanel_Section_Lf, TObjectPtr<UScrollBox>>& Section : Sections)
		{
			if (Section.Value)
			{
				Section.Value->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	FORCEINLINE_DEBUGGABLE void EnableButtons()
	{
		for (const TTuple<EShow_ControlPanel_Buttons_Lf, TArray<TObjectPtr<UButton>>>& ButtonArray : Buttons)
		{
			for (const TObjectPtr<UButton>& Button : ButtonArray.Value)
			{
				if (Button)
				{
					Button->SetIsEnabled(true);
				}
			}
		}
	}

	static FORCEINLINE_DEBUGGABLE EShow_ControlPanel_Buttons_Lf GetCommonButtonEnum(
		const EShow_ControlPanel_Section_Lf& RequestEnum)
	{
		switch (RequestEnum)
		{
		case EShow_ControlPanel_Section_Lf::None:
			// Handle None value (common with Buttons enum)
			return EShow_ControlPanel_Buttons_Lf::None;
		case EShow_ControlPanel_Section_Lf::Welcome:
			// Handle Welcome value (common with Buttons enum)
			return EShow_ControlPanel_Buttons_Lf::Welcome;
		case EShow_ControlPanel_Section_Lf::Reference:
			// Handle Reference value (common with Buttons enum)
			return EShow_ControlPanel_Buttons_Lf::Reference;
		case EShow_ControlPanel_Section_Lf::Generate:
			// Handle Generate value (common with Buttons enum)
			return EShow_ControlPanel_Buttons_Lf::Generate;
		case EShow_ControlPanel_Section_Lf::Tweaks:
			// Handle Tweaks value (common with Buttons enum)
			return EShow_ControlPanel_Buttons_Lf::Tweaks;
		}

		return EShow_ControlPanel_Buttons_Lf::None;
	}

	/*	==============================================================================================================
												TWEAKING BEHAVIOUR
	==============================================================================================================	*/

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Max Amount of Instances ( Including Customization Parts )", ClampMin = "100", ClampMax =
			"200000"), Category="Texture Tweaking")
	uint32 MaxNumMeshes = 20000;

	UPROPERTY(EditAnywhere, Config,
		meta = (DisplayName = "Max Amount of Bones In all Skeletons", ClampMin = "5", ClampMax = "200"),
		Category="Texture Tweaking")
	uint32 MaxNumBones = 75;

	//UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Use high precision 32 bit Textures for Animations"), Category="Texture Tweaking")
	//bool bUseHighPrecisionAnimationMode = true;

	UPROPERTY(VisibleAnywhere, Config, meta = (DisplayName = "Allocated GPU Memory in MB"), Category="Texture Tweaking")
	uint32 AverageAllocatedMemory = 0;

	UPROPERTY(EditAnywhere, Config, meta = (DisplayName = "Manually Adjust the Textures"), Category="Texture Tweaking")
	bool bManuallyAdjustTextureSize = false;


	UFUNCTION(BlueprintCallable, Category="TurboSequence")
	void OnTweakingGlobalTextures();


	/*	=======================================================================================================
												GENERATOR FUNCTIONALITY
	===========================================================================================================	*/

	// TODO: move in helpers
	static FORCEINLINE_DEBUGGABLE TObjectPtr<USkeletalMesh> DuplicateSkeletalMesh(
		const TObjectPtr<USkeletalMesh> FromMesh, const FName& AssetName, bool bRemoveLODs)
	{
		const TObjectPtr<USkeletalMesh> DuplicatedMesh = DuplicateObject(FromMesh, FromMesh->GetOuter(), AssetName);

		if (bRemoveLODs)
		{
			GenerateSkeletalMeshLevelOfDetails(DuplicatedMesh, GET1_NUMBER);
		}
		DuplicatedMesh->NeverStream = true;
		return DuplicatedMesh;
	}

	// TODO: move in helpers
	static FORCEINLINE_DEBUGGABLE TObjectPtr<USkeletalMesh> GenerateSkeletalMeshLevelOfDetails(
		const TObjectPtr<USkeletalMesh> FromMesh, int32 NewLODCount)
	{
		UE_LOG(LogTurboSequence_Lf, Display, TEXT("Reducing Mesh for Platform -> %s"),
		       *GetTargetPlatformManagerRef().GetRunningTargetPlatform()->DisplayName().ToString());

		FLODUtilities::RegenerateLOD(FromMesh, GetTargetPlatformManagerRef().GetRunningTargetPlatform(), NewLODCount,
		                             true, false);
		FromMesh->NeverStream = true;
		return FromMesh;
	}


	// TODO: move in helpers
	static TObjectPtr<UStaticMesh> GenerateStaticMeshFromSkeletalMesh(const TObjectPtr<USkeletalMesh> SkeletalMesh,
	                                                                  int32 LodIndex, const FString& InPath,
	                                                                  const FString& InAssetName,
	                                                                  TArray<int32>& OutMeshIndicesOrder,
	                                                                  const ETurboSequence_MeshDataMode_Lf MeshDataMode)
	{
		if (FString PackageName; FPackageName::TryConvertFilenameToLongPackageName(InPath, PackageName))
		{
			if (!IsValid(SkeletalMesh))
			{
				UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Seems like a caching error while generating %s"),
				       *PackageName);
				return nullptr;
			}

			// Then find/create it.
			UPackage* Package = CreatePackage(*PackageName);
			check(Package);

			// Create StaticMesh object
			TObjectPtr<UStaticMesh> StaticMesh = NewObject<UStaticMesh>(
				Package, *InAssetName, RF_Public | RF_Standalone);
			StaticMesh->InitResources();

			StaticMesh->SetLightingGuid();

			uint32 MaxNumTextureCoordinate = GET0_NUMBER;
			const FSkeletalMeshRenderData* RenderData = SkeletalMesh->GetResourceForRendering();


			const FSkeletalMeshLODModel& LodModel = SkeletalMesh->GetImportedModel()->LODModels[LodIndex];
			uint32 SkinnedMeshVertices = LodModel.NumVertices;

			FMeshDescription MeshDescription;
			LodModel.GetMeshDescription(SkeletalMesh, LodIndex, MeshDescription);

			const FSkeletalMeshLODRenderData& LodResource = RenderData->LODRenderData[LodIndex];
			uint32 MaxNumTextCoord = LodResource.StaticVertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords();

			FSkeletalMeshAttributes MeshAttributes(MeshDescription);

			if (MeshDataMode == ETurboSequence_MeshDataMode_Lf::UV)
			{
				MaxNumTextureCoordinate = FMath::Max(MaxNumTextCoord + GET1_NUMBER, MaxNumTextureCoordinate);

				TVertexInstanceAttributesRef<FVector2f> VertexInstanceUVs = MeshAttributes.GetVertexInstanceUVs();

				VertexInstanceUVs.InsertChannel(MaxNumTextCoord);

				int32 NumIndices = LodModel.IndexBuffer.Num();
				for (int32 Idx = GET0_NUMBER; Idx < NumIndices; ++Idx)
				{
					const int32 VertexID = LodModel.IndexBuffer[Idx];

					if (!OutMeshIndicesOrder.Contains(VertexID))
					{
						OutMeshIndicesOrder.Add(VertexID);
					}

					const FIntVector2 BitValues = FTurboSequence_Helper_Lf::DecodeUInt32ToUInt16(VertexID);

					VertexInstanceUVs.Set(Idx, MaxNumTextCoord, FVector2f(BitValues.X, BitValues.Y));
				}
			}
			else // Generate Vertex Color Instead
			{
				//const bool bHasVertexColors = EnumHasAllFlags(SkeletalMesh->GetVertexBufferFlags(), ESkeletalMeshVertexFlags::HasVertexColors);

				TVertexInstanceAttributesRef<FVector4f> VertexInstanceColor = MeshAttributes.GetVertexInstanceColors();

				// if (!bHasVertexColors)
				// {
				//
				// }

				int32 NumIndices = LodModel.IndexBuffer.Num();
				for (int32 Idx = GET0_NUMBER; Idx < NumIndices; ++Idx)
				{
					const int32 VertexID = LodModel.IndexBuffer[Idx];

					if (!OutMeshIndicesOrder.Contains(VertexID))
					{
						OutMeshIndicesOrder.Add(VertexID);
					}

					const FVector4f BitValues = FVector4f(FTurboSequence_Helper_Lf::DecodeUInt32ToColor(VertexID));

					VertexInstanceColor.Set(Idx, BitValues);
				}
			}


			FRawMesh RawMesh;
			FStaticMeshOperations::ConvertToRawMesh(MeshDescription, RawMesh, TMap<FName, int32>());

			// Add source to new StaticMesh
			if (RawMesh.IsValidOrFixable())
			{
				FStaticMeshSourceModel& SrcModel = StaticMesh->AddSourceModel();
				SrcModel.BuildSettings.bRecomputeNormals = false;
				SrcModel.BuildSettings.bRecomputeTangents = false;
				SrcModel.BuildSettings.bRemoveDegenerates = false;
				SrcModel.BuildSettings.bUseHighPrecisionTangentBasis = false;
				SrcModel.BuildSettings.bUseFullPrecisionUVs = true;
				SrcModel.BuildSettings.bGenerateLightmapUVs = false;
				SrcModel.BuildSettings.SrcLightmapIndex = GET0_NUMBER;
				SrcModel.BuildSettings.DstLightmapIndex = GET0_NUMBER;
				SrcModel.SaveRawMesh(RawMesh);
			}
			else
			{
				UE_LOG(LogTurboSequence_Lf, Warning, TEXT("Mesh seems not valid...."))
			}

			// Copy materials to new mesh
			for (const FSkeletalMaterial& Material : SkeletalMesh->GetMaterials())
			{
				StaticMesh->GetStaticMaterials().Add(FStaticMaterial(Material.MaterialInterface));
			}

			//Set the Imported version before calling the build
			StaticMesh->ImportVersion = LastVersion;

			// Set light map coordinate index to match DstLightmapIndex
			StaticMesh->SetLightMapCoordinateIndex(GET0_NUMBER);

			// setup section info map
			TArray<int32> UniqueMaterialIndices;
			for (int32 MaterialIndex : RawMesh.FaceMaterialIndices)
			{
				UniqueMaterialIndices.AddUnique(MaterialIndex);
			}

			int32 SectionIndex = GET0_NUMBER;
			for (int32 UniqueMaterialIndex : UniqueMaterialIndices)
			{
				StaticMesh->GetSectionInfoMap().Set(GET0_NUMBER, SectionIndex, FMeshSectionInfo(UniqueMaterialIndex));
				SectionIndex++;
			}
			StaticMesh->GetOriginalSectionInfoMap().CopyFrom(StaticMesh->GetSectionInfoMap());

			StaticMesh->Build(false);
			if (!StaticMesh->GetRenderData())
			{
				UE_LOG(LogTurboSequence_Lf, Warning,
				       TEXT(
					       "Seems like a caching error while generating %s, please delete this asset manually and try again"
				       ), *PackageName);
				return nullptr;
			}
			StaticMesh->PostEditChange();

			FBoxSphereBounds Bounds = StaticMesh->GetRenderData()->Bounds;
			const FVector MinMax = Bounds.BoxExtent * GET2_NUMBER;
			float WantedZ = MinMax.Z;
			Bounds = FBoxSphereBounds(Bounds.Origin, FVector::OneVector * WantedZ, WantedZ);

			StaticMesh->GetRenderData()->Bounds = Bounds;
			StaticMesh->SetExtendedBounds(Bounds);
			StaticMesh->NeverStream = true;

			if (SkinnedMeshVertices != StaticMesh->GetNumVertices(GET0_NUMBER))
			{
				UE_LOG(LogTurboSequence_Lf, Error,
				       TEXT(
					       "It seems the created mesh has different amounts of vertices, it's not possible to skin weight the mesh without the same amount of vertices, the mesh would deform at runtime not great, -> Source Vertices %d, Converted Vertices -> %d"
				       ), SkinnedMeshVertices, StaticMesh->GetNumVertices(GET0_NUMBER));
				return nullptr;
			}

			StaticMesh->MarkPackageDirty();

			// Notify asset registry of new asset
			FAssetRegistryModule::AssetCreated(StaticMesh);

			FTurboSequence_Helper_Lf::SaveNewAsset(StaticMesh);


			return StaticMesh;
		}
		UE_LOG(LogTurboSequence_Lf, Warning, TEXT("The Directory is not an valid Project Directory..."));

		return nullptr;
	}
};
