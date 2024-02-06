// Copyright 2022 OlssonDev

#pragma once

#include "CoreMinimal.h"
#include "ACAssetActionFragment.h"
#include "ACAssetActionFragment_DragDrop.generated.h"

//Adds drag drop support into the viewport to any objects. Same functionality static meshes for example has.
//Example: Drag the static mesh into the viewport and a actor spawns.
UCLASS(DisplayName = "Add Content Browser Drag Drop Support")
class ASSETCREATOREDITOR_API UACAssetActionFragment_DragDrop : public UACAssetActionFragment
{
	GENERATED_BODY()

	public:

	//UACAssetActionFragment interface implementation
	virtual bool IsValidFragment() override
	{
		return Super::IsValidFragment() && IsValid(ActorToSpawn);
	};
	//End of implementation

	public:

	//The Actor to spawn when the Asset Class has been dragged into the level from the content browser.
	UPROPERTY(EditDefaultsOnly, Category = "Drag Drop Settings", meta = (EditCondition=bEnabled))
	TSubclassOf<AActor> ActorToSpawn;
	
};
