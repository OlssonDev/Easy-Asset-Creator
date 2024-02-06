// Copyright 2022 OlssonDev

#pragma once
#include "CoreMinimal.h"
#include "ACAssetActionFragment.h"
#include "AssetCreatorEditorModule.h"
#include "ACAssetActionFragment_PlaceActors.generated.h"

UCLASS(DisplayName = "Add To Place Actors Menu")
class ASSETCREATOREDITOR_API UACAssetActionFragment_PlaceActors : public UACAssetActionFragment
{
	GENERATED_BODY()

	public:

	UACAssetActionFragment_PlaceActors();

	//Gets the icon from StyleSet
	FSlateIcon GetIcon()
	{
		FSlateIcon NewIcon (StyleSet, IconName);

		//Revert back to cube icon if the SlateIcon is not valid.
		if (IconName.IsNone() || NewIcon.GetIcon() == FStyleDefaults::GetNoBrush())
		{
			NewIcon = FSlateIcon(FAssetCreatorEditorModule::GetEditorStyleSet(), "ClassIcon.Cube");
		}

		return NewIcon;
	}

	public:

	UPROPERTY()
	bool UseSortOrder = false;

	/** Optional sort order (lowest first). Overrides default class name sorting. */
	UPROPERTY(EditAnywhere, Category = "Place Actor Menu", meta = (EditCondition=UseSortOrder))
	int32 OptionalSortOrder = 0;
	
	//The StyleSet the Asset Action should try to get the icon from. EditorStyle set is the default.
	//If it didn't find the assigned StyleSet, it will default back to EditorStyle.
	//NOTE: WORKS ONLY FOR UNREAL ENGINE 5!
	UPROPERTY(EditAnywhere, Category = "Advanced | Category Icon")
	FName StyleSet;

	//The name of the icon that we should add to the category icon.
	//If it didn't find the icon in the given StyleSet, it will default back to a Cube icon.
	//NOTE: WORKS ONLY FOR UNREAL ENGINE 5!
	UPROPERTY(EditAnywhere, Category = "Advanced | Category Icon")
	FName IconName = "Cube";
	
};
