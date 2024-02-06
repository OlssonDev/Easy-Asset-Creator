// Copyright 2022 OlssonDev


#include "ACEditorSubsystem.h"
#include "ACAssetAction.h"
#include "ACPluginSettings.h"
#include "Runtime/Launch/Resources/Version.h"
#include "AssetCreatorEditorModule.h"
#include "KismetCompilerModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetTools/FAssetTypeActions_AssetCreatorBase.h"
#include "Factory/ACActorFactory.h"
#include "Factory/ACBaseFactory.h"
#include "Fragments/ACAssetActionFragment_DragDrop.h"
#include "Fragments/ACAssetActionFragment_PlaceActors.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "UAssetCreatorSubsystem"

class UACAssetActionFragment_PlaceActors;

void UACEditorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//Listen to when an asset has been deleted. So we can refresh the plugin in case the asset was a class of the plugin's API.
	FEditorDelegates::OnAssetsDeleted.AddUObject(this, &ThisClass::OnAssetsDeleted);

	//Listen to when an object has been modified. So we can refresh the categories.
	FCoreUObjectDelegates::OnObjectPropertyChanged.AddUObject(this, &ThisClass::OnAssetCreatorChanged);

	//We don't want to listen for new Assets before the editor is not opened.
	OnMapOpenedDelegate = FEditorDelegates::OnMapOpened.AddUObject(this, &ThisClass::OnEditorOpened);
	
	//When we start the editor, just refresh the whole plugin.
	RefreshAll();

	Super::Initialize(Collection);
}

void UACEditorSubsystem::OnEditorOpened(const FString& FileName, bool bAsTemplate)
{
	FEditorDelegates::OnMapOpened.Remove(OnMapOpenedDelegate);
	IAssetRegistry* AssetRegistry = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName).Get();
	
	//Listen to when an asset has been added so we can keep those in memory if they're an UACAssetAction or UACBaseFactory.
	AssetRegistry->OnAssetAdded().AddUObject(this, &ThisClass::OnAssetCreated);
}

void UACEditorSubsystem::OnAssetCreated(const FAssetData& AssetData)
{
	UClass* AssetAction = nullptr;
	if (GetClassFromBlueprint(AssetData, UACAssetAction::StaticClass(), AssetAction))
	{
		AssetActions.AddUnique(AssetAction);
		return;
	}

	UClass* Factory = nullptr;
	if (GetClassFromBlueprint(AssetData, UACBaseFactory::StaticClass(), Factory))
	{
		Factories.AddUnique(Factory);
		return;
	}
}

void UACEditorSubsystem::OnAssetsDeleted(const TArray<UClass*>& AssetsToDelete)
{
	//Just refresh everything when an asset has been deleted.
	RefreshAll();
}

UACAssetAction* UACEditorSubsystem::GetAssetAction(UClass* AssetActionClass)
{
	if (AssetActionClass && AssetActionClass->IsChildOf(UACAssetAction::StaticClass()))
	{
		UACAssetAction* AssetAction = CastChecked<UACAssetAction>(AssetActionClass->GetDefaultObject());
		return AssetAction->AssetActionSettings.IsValid() ? AssetAction : nullptr;
	}
	return nullptr;
}

bool UACEditorSubsystem::GetClassFromBlueprint(const FAssetData AssetData, UClass* ClassToFind, UClass*& GeneratedClass)
{
	if (AssetData.IsValid() && ClassToFind)
	{
		FAssetDataTagMapSharedView::FFindTagResult Result = AssetData.TagsAndValues.FindTag(TEXT("NativeParentClass"));
		if (Result.IsSet())
		{
			const FString ClassObjectPath = FPackageName::ExportTextPathToObjectPath(Result.GetValue());
			if (UClass* ParentClass = FindObjectSafe<UClass>(nullptr, *ClassObjectPath, true))
			{
				if (ParentClass->IsChildOf(ClassToFind))
				{
					//TODO: Loading these assets could cause problems on projects with a large number of them.
					UBlueprint* BP = CastChecked<UBlueprint>(AssetData.GetAsset());
					//Can be not valid if a class has just recently been deleted.
					if (IsValid(BP->GeneratedClass))
					{
						GeneratedClass = BP->GeneratedClass;
						return true;
					}
				}
			}
		}	
	}

	return false;
}

void UACEditorSubsystem::RefreshPlacementModeExtensions()
{
	if (GEditor)
	{
		IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
		
		{
			for (auto RegisteredPlaceableItem : RegisteredPlaceableItems)
			{
				PlacementModeModule.UnregisterPlaceableItem(*RegisteredPlaceableItem);
			}
			RegisteredPlaceableItems.Empty();

			for (FPlacementCategoryInfo CategoryInfo : RegisteredPlaceableItemCategories)
			{
				PlacementModeModule.UnregisterPlacementCategory(CategoryInfo.UniqueHandle);
			}
			RegisteredPlaceableItemCategories.Empty();

			for (UActorFactory* ActorFactory : ActorFactories)
			{
				GEditor->ActorFactories.Remove(ActorFactory);
			}
			ActorFactories.Empty();
		}
		
		for (UClass* Class : AssetActions)
		{
			if (UACAssetAction* AssetAction = GetAssetAction(Class))
			{
				auto* PlaceActorsFragment = AssetAction->FindFragmentByClass<UACAssetActionFragment_PlaceActors>();
				if (PlaceActorsFragment && PlaceActorsFragment->IsValidFragment())
				{
					FName CategoryName = AssetAction->AssetActionSettings.CategoryName;
					FPlacementCategoryInfo PlacementCategoryInfo = FPlacementCategoryInfo(
						FText::FromName(CategoryName),
						CategoryName,"PM" + CategoryName.ToString(),
						25);

					#if ENGINE_MAJOR_VERSION == 5
					PlacementCategoryInfo.DisplayIcon = PlaceActorsFragment->GetIcon();
					#endif
				
					PlacementModeModule.RegisterPlacementCategory(PlacementCategoryInfo);
					RegisteredPlaceableItemCategories.Add(PlacementCategoryInfo);

					FPlaceableItem NewPlaceableItem;
					NewPlaceableItem.Factory = MakeActorFactory(AssetAction);
					NewPlaceableItem.AssetData = AssetAction->AssetActionSettings.AssetClass.Get();
					NewPlaceableItem.bAlwaysUseGenericThumbnail = true;
					if (PlaceActorsFragment->UseSortOrder)
					{
						NewPlaceableItem.SortOrder = PlaceActorsFragment->OptionalSortOrder;
					}
					NewPlaceableItem.AssetTypeColorOverride = AssetAction->AssetActionSettings.AssetColor;
					NewPlaceableItem.DisplayName = AssetAction->AssetActionSettings.AssetClass->GetDisplayNameText();
					RegisteredPlaceableItems.Add(PlacementModeModule.RegisterPlaceableItem(PlacementCategoryInfo.UniqueHandle, MakeShared<FPlaceableItem>(NewPlaceableItem)));
				}

				auto* DragDropFragment = AssetAction->FindFragmentByClass<UACAssetActionFragment_DragDrop>();
				if (DragDropFragment && DragDropFragment->IsValidFragment())
				{
					UACActorFactory* NewFactory = MakeActorFactory(AssetAction);
					NewFactory->NewActorClass = DragDropFragment->ActorToSpawn;
					//Insert on 0 because we want our factories to have priority over generic factories such as ActorFactory.
					GEditor->ActorFactories.Insert(NewFactory, 0);
				}
			}
		}
		
		PlacementModeModule.RegenerateItemsForCategory(FBuiltInPlacementCategories::AllClasses());
	}
}

void UACEditorSubsystem::RefreshActions()
{
	RefreshPlacementModeExtensions();
	RefreshAssetTypeActions();
}

void UACEditorSubsystem::RefreshAll()
{
	AssetActions = GatherAllAssetsOfClass(UACAssetAction::StaticClass());
	Factories = GatherAllAssetsOfClass(UACBaseFactory::StaticClass());
	
	RefreshActions();
}

void UACEditorSubsystem::RefreshAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	//Unregister all of the AssetTypeActions first before we register new ones.
	{
		for (int32 i = AssetTypeActions.Num(); i--;)
		{
			UnregisterAssetTypeAction(AssetTools, AssetTypeActions[i]);
		}
	}

	//Register all the valid AssetActions.
	for (UClass* Class : AssetActions)
	{
		if (UACAssetAction* AssetAction = GetAssetAction(Class))
		{
			uint32 Category = AssetTools.RegisterAdvancedAssetCategory(AssetAction->AssetActionSettings.CategoryName,
				FText::Format(LOCTEXT("AssetCreatorCategory", "{0}"),
					FText::FromName(AssetAction->AssetActionSettings.CategoryName)));
			
			RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_AssetCreatorBase(AssetAction->AssetActionSettings, Category)));
		}
	}
}


void UACEditorSubsystem::OnAssetCreatorChanged(UObject* Object, FPropertyChangedEvent& ChangedEvent)
{
	//Only refresh if the change had been made on one of our classes.
	UClass* Class = Object->GetClass();
	if (Class->IsChildOf(UACAssetAction::StaticClass()) || Class->IsChildOf(UACBaseFactory::StaticClass()) || Class == UACPluginSettings::StaticClass())
	{
		RefreshActions();
	}
}

void UACEditorSubsystem::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	AssetTypeActions.Add(Action);
}

void UACEditorSubsystem::UnregisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.UnregisterAssetTypeActions(Action);
	AssetTypeActions.Remove(Action);
}

bool UACEditorSubsystem::HasFactory(UClass* Class)
{
	if (Class)
	{
		UClass* BlueprintClass = nullptr;
		UClass* BlueprintGeneratedClass = nullptr;

		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(Class, BlueprintClass, BlueprintGeneratedClass);
		
		if (UBlueprint* Blueprint = Cast<UBlueprint>(BlueprintClass->ClassDefaultObject))
		{
			if (!Blueprint->SupportedByDefaultBlueprintFactory())
			{
				FNotificationInfo Info(NSLOCTEXT("AssetCreatorEditor", "FAssetCreatorEditorModule", "Selected class can't be used. The class doesn't support to be created in a default Blueprint Factory."));
				Info.ExpireDuration = 4.0f;
				FSlateNotificationManager::Get().AddNotification(Info);
				return true;
			}
		}
	}
	
	return false;
}

UACActorFactory* UACEditorSubsystem::MakeActorFactory(UACAssetAction* AssetAction)
{
	UACActorFactory* NewFactory = nullptr;
	if (IsValid(AssetAction))
	{
		NewFactory = NewObject<UACActorFactory>(GetTransientPackage(), UACActorFactory::StaticClass());
		NewFactory->AssetAction = AssetAction;
		NewFactory->NewActorClass = AssetAction->AssetActionSettings.AssetClass.Get();
		ActorFactories.Add(NewFactory);
	}
	return NewFactory;
}

TArray<UClass*> UACEditorSubsystem::GatherAllAssetsOfClass(UClass* Class)
{
	TArray<UClass*> Classes;

	// Search native classes
	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if (!ClassIt->IsNative() || !ClassIt->IsChildOf(Class))
		{
			continue;
		}

		// Ignore classes that are Abstract, HideDropdown, and Deprecated.
		if (ClassIt->HasAnyClassFlags(CLASS_Abstract | CLASS_HideDropDown | CLASS_Deprecated | CLASS_NewerVersionExists))
		{
			continue;
		}

		Classes.AddUnique(*ClassIt);
	}

	//Search Blueprint classes.
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		
		TArray<FString> ContentPaths;
		ContentPaths.Add(TEXT("/Game"));
		AssetRegistry.ScanPathsSynchronous(ContentPaths);

		FARFilter Filter;
		#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 1
		Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
		#else
		Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
		#endif
		Filter.bRecursiveClasses = true;
		Filter.bRecursivePaths = true;

		TArray<FAssetData> AssetList;
		AssetRegistry.GetAssets(Filter, AssetList);
	
		for (FAssetData& Asset : AssetList)
		{
			UClass* GeneratedClass = nullptr;;
			if (GetClassFromBlueprint(Asset, Class, GeneratedClass))
			{
				Classes.AddUnique(GeneratedClass);
			}
		}
	}
	
	return Classes;
}

#undef LOCTEXT_NAMESPACE
