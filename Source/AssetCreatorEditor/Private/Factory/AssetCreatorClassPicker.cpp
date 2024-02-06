// Copyright 2022 OlssonDev

#include "AssetCreatorClassPicker.h"
#include "ClassViewerModule.h"
#include "EditorStyleSet.h"
#include "Runtime/Launch/Resources/Version.h"
#include "IDocumentation.h"
#include "SlateOptMacros.h"
#include "Factories/BlueprintFactory.h"
#include "Widgets/Layout/SUniformGridPanel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

#define LOCTEXT_NAMESPACE "SAdventureClassPicker"

void SAssetCreatorClassPicker::Construct(const FArguments& InArgs)
{
	
}

bool SAssetCreatorClassPicker::ConfigureProperties(TWeakObjectPtr<UBlueprintFactory> InBlueprintFactory)
{
	bOkClicked = false;
	BlueprintFactory = InBlueprintFactory;
	
	ChildSlot
	[
		SNew(SBorder)
		.Visibility(EVisibility::Visible)
		.BorderImage(GetBrush("Menu.Background"))
		[
			SNew(SBox)
			.Visibility(EVisibility::Visible)
			.WidthOverride(1500.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1)
				[
					SNew(SBorder)
					.BorderImage(GetBrush("ToolPanel.GroupBorder"))
					.Content()
					[
						SAssignNew(ParentClassContainer, SVerticalBox)
					]
				]
				// Ok/Cancel buttons
				+ SVerticalBox::Slot()
				  .AutoHeight()
				  .HAlign(HAlign_Center)
				  .VAlign(VAlign_Bottom)
				  .Padding(8)
				[
					SNew(SUniformGridPanel)
						.SlotPadding(GetMargin("StandardDialog.SlotPadding"))
						.MinDesiredSlotWidth(GetFloat("StandardDialog.MinDesiredSlotWidth"))
						.MinDesiredSlotHeight(GetFloat("StandardDialog.MinDesiredSlotHeight"))
					+ SUniformGridPanel::Slot(0, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.ContentPadding(GetMargin("StandardDialog.ContentPadding"))
						.OnClicked(this, &SAssetCreatorClassPicker::OkClicked)
						.Text(LOCTEXT("AssetCreatorClassPickerOk", "OK"))
					]
					+ SUniformGridPanel::Slot(1, 0)
					[
						SNew(SButton)
						.HAlign(HAlign_Center)
						.ContentPadding(GetMargin("StandardDialog.ContentPadding"))
						.OnClicked(this, &SAssetCreatorClassPicker::CancelClicked)
						.Text(LOCTEXT("AssetCreatorClassPickerCancel", "Cancel"))
					]
				]
			]
		]
	];
	
	MakeParentClassPicker();
	
	TSharedRef<SWindow> Window = SNew(SWindow)
	.Title(FText::Format(LOCTEXT("CreateBlueprintOptions", "Create New {0}"), BlueprintFactory->GetDisplayName()))
	.ClientSize(FVector2D(400, 700))
	.SupportsMinimize(false).SupportsMaximize(false)
	[
		AsShared()
	];

	PickerWindow = Window;

	GEditor->EditorAddModalWindow(Window);
	BlueprintFactory.Reset();
	return bOkClicked;
}

FReply SAssetCreatorClassPicker::OkClicked()
{
	if (BlueprintFactory.IsValid())
	{
		BlueprintFactory->ParentClass = ParentClass.Get();
	}

	CloseDialog(true);

	return FReply::Handled();
}

void SAssetCreatorClassPicker::CloseDialog(bool bWasPicked)
{
	bOkClicked = bWasPicked;
	if (PickerWindow.IsValid())
	{
		PickerWindow.Pin()->RequestDestroyWindow();
	}
}

FReply SAssetCreatorClassPicker::CancelClicked()
{
	CloseDialog();
	return FReply::Handled();
}

FReply SAssetCreatorClassPicker::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		CloseDialog();
		return FReply::Handled();
	}
	
	return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

const FSlateBrush* SAssetCreatorClassPicker::GetBrush(FName PropertyName)
{
	#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1
	return FAppStyle::GetBrush(PropertyName);
	#else
	return FEditorStyle::GetBrush(PropertyName);
	#endif
}

const FMargin& SAssetCreatorClassPicker::GetMargin(FName PropertyName)
{
	#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1
	return FAppStyle::GetMargin(PropertyName);
	#else
	return FEditorStyle::GetMargin(PropertyName);
	#endif
}

float SAssetCreatorClassPicker::GetFloat(FName PropertyName)
{
	#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION == 1
	return FAppStyle::GetFloat(PropertyName);
	#else
	return FEditorStyle::GetFloat(PropertyName);
	#endif
}

bool AdventureClassPickerFilter::IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
}

bool AdventureClassPickerFilter::IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef<const IUnloadedBlueprintData> InUnloadedClassData, TSharedRef<FClassViewerFilterFuncs> InFilterFuncs)
{
	return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
}

void SAssetCreatorClassPicker::MakeParentClassPicker()
{
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");
	
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;
	Options.bIsBlueprintBaseOnly = true;

	TSharedPtr<AdventureClassPickerFilter> Filter = MakeShareable(new AdventureClassPickerFilter);
	Filter->AllowedChildrenOfClasses.Add(BlueprintFactory->ParentClass);

	#if ENGINE_MAJOR_VERSION == 5
	Options.ClassFilters.Add(Filter.ToSharedRef());
	#else
	Options.ClassFilter = Filter;
	#endif
	
	Options.NameTypeToDisplay = EClassViewerNameTypeToDisplay::DisplayName;

	ParentClassContainer->ClearChildren();
	ParentClassContainer->AddSlot()
	.AutoHeight()
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ParentClass", "Search for Class:"))
		.ShadowOffset(FVector2D(1.0f, 1.0f))
	];

	ParentClassContainer->AddSlot()
	[
		ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SAssetCreatorClassPicker::OnClassPicked))
	];
}

void SAssetCreatorClassPicker::OnClassPicked(UClass* ChosenClass)
{
	ParentClass = ChosenClass;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
