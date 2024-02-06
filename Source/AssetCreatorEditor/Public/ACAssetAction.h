// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ACAssetAction.generated.h"

USTRUCT(BlueprintType)
struct FAssetActionSettings
{
	GENERATED_BODY()

	//If this Asset Action is enabled and show up in the set category.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Asset Action")
	bool bEnabled = true;
	
	//The class you want this Asset Action to be responsible for.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Class Settings")
	TSubclassOf<UObject> AssetClass;

	//The name of the category this Asset Action should be in.
	//Example: Weapons
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Asset Category")
	FName CategoryName;

	//Bool to check if sub menu should be considered at all in the drop down menu.
	UPROPERTY()
	bool UseSubMenu = false;

	//The name of the Sub Category this Asset Action will exists in within the category. If left empty, it will count as no SubCategory.
	//NOTE: A sub category can only be created if there are more than one item in the main category.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Asset Category", meta = (EditCondition=UseSubMenu))
	FText SubCategoryName;

	//The color of the asset in the editor menus only. Once the asset is created it will default back to its original color.
	//By default it's the default Blueprint color.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cosmetic")
	FColor AssetColor = FColor( 63, 126, 255 );

	bool IsValid()
	{
		return bEnabled && !CategoryName.IsNone() && AssetClass;
	}
	
};

class UACAssetActionFragment;

//Responsible for adding different functionalities to an asset in the editor.
UCLASS(Blueprintable, NotBlueprintType, Abstract, DisplayName = "Asset Action")
class ASSETCREATOREDITOR_API UACAssetAction : public UObject
{
	GENERATED_BODY()

	public:

	//Checks if the Asset Act
	virtual bool IsAssetActionValid()
	{
		return AssetActionSettings.IsValid();
	};

	//Fires when a actor has spawned from drag dropping an asset into the viewport.
	//This will ONLY fire if this Asset Action has Drag Drop support or is in the Place Actors menu.
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor);
	
	template <typename ResultClass>
	ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}
	const UACAssetActionFragment* FindFragmentByClass(TSubclassOf<UACAssetActionFragment> FragmentClass) const;

	protected:

	//Fires when a actor has spawned from drag dropping an asset into the viewport.
	//This will ONLY fire if this Asset Action has Drag Drop support or is in the Place Actors menu.
	UFUNCTION(BlueprintImplementableEvent, Category = "Asset Action", DisplayName = "Post Spawn Actor")
	void K2_PostSpawnActor(UObject* Asset, AActor* NewActor);

	//Gets the class from the UBlueprint asset class.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Asset Action")
	TSubclassOf<UObject> GetClassFromAsset(UObject* Asset);
	
	public:

	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties), Category = "Asset Action")
	FAssetActionSettings AssetActionSettings;

	protected:

	//Fragments are used to add different supports for the Asset Action.
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Fragments")
	TArray<class UACAssetActionFragment*> Fragments;
	
};
