// Copyright 2022 OlssonDev

#include "Factory/ACBaseFactory.h"
#include "AssetCreatorClassPicker.h"
#include "AssetCreatorEditorModule.h"
#include "ACEditorSubsystem.h"
#include "Runtime/Launch/Resources/Version.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "UAssetCreator_BaseFactory"

UACBaseFactory::UACBaseFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	DefaultAssetName = UBlueprintFactory::GetDefaultNewAssetName();

	#if ENGINE_MAJOR_VERSION == 5
	#if ENGINE_MINOR_VERSION == 1
	BlueprintType = BPTYPE_Normal;
	#endif
	bSkipClassPicker = true;
	#endif
}

void UACBaseFactory::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (UACEditorSubsystem::HasFactory(ParentClass) == false)
	{
		OldClass = ParentClass;
		SupportedClass = ParentClass;
	}
	else
	{
		ParentClass = OldClass;
	}
}

uint32 UACBaseFactory::GetMenuCategories() const
{
	if (SupportedClass == nullptr || SupportedClass == UBlueprint::StaticClass() || ParentClass == nullptr || GetOuter()->GetName() == "/Engine/Transient")
	{
		return 0;
	}
	
	return Super::GetMenuCategories();
}

bool UACBaseFactory::ConfigureProperties()
{
	if (UseClassPicker)
	{
		TSharedRef<SAssetCreatorClassPicker> Dialog = SNew(SAssetCreatorClassPicker);
		return Dialog->ConfigureProperties(this);
	}

	return true;
}

FString UACBaseFactory::GetDefaultNewAssetName() const
{
	if (DefaultAssetName.IsEmpty())
	{
		return Super::GetDefaultNewAssetName();
	}
	
	return DefaultAssetName;
}

UObject* UACBaseFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass))
	{
		FFormatNamedArguments Args;
		Args.Add( TEXT("ClassName"), (ParentClass != nullptr) ? FText::FromString( ParentClass->GetName() ) : LOCTEXT("Null", "(null)") );
		FMessageDialog::Open( EAppMsgType::Ok, FText::Format( LOCTEXT("CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{0}'."), Args ) );
		return nullptr;
	}
	else
	{
		UClass* BlueprintClass = nullptr;
		UClass* BlueprintGeneratedClass = nullptr;

		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(ParentClass, BlueprintClass, BlueprintGeneratedClass);

		return FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BPTYPE_Normal, BlueprintClass, BlueprintGeneratedClass, CallingContext);
	}
}

FText UACBaseFactory::GetDisplayName() const
{
	//Avoid ensures on startup
	if (!IsValid(ParentClass))
	{
		return LOCTEXT("ACFactory_MissingParentClass", "Missing Parent Class");
	}
	return Super::GetDisplayName();
}

#undef LOCTEXT_NAMESPACE
