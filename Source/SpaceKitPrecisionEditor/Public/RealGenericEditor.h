// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "SGraphPin.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "Runtime/Launch/Resources/Version.h"
#include "RealFloat.h"
#include "RealFixed.h"

//
// THE TRealNumericBoxWrapper AND ALL ASSOCIATED HELPERS HAVE BEEN REMOVED.
// WE WILL USE THE ENGINE'S NATIVE SNumericEntryBox<double> AND CONVERT MANUALLY.
//

// Adds a field to the input pin for a single RealType.
template<typename RealType>
class SRealGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SRealGraphPin<RealType>) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
	{
		SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
	}

public:
	TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(SBox)
		.MinDesiredWidth(18)
		.MaxDesiredWidth(400)
		[
			SNew(SNumericEntryBox<double>)
			.EditableTextBoxStyle(FAppStyle::Get(), "Graph.EditableTextBox")
			.BorderForegroundColor(FSlateColor::UseForeground())
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility) // Use the base class implementation for visibility
			// THE DEFINITIVE FIX: Re-implement the IsEnabled function that was removed.
			.IsEnabled(this, &SRealGraphPin<RealType>::GetDefaultValueIsEnabled)
			.Value(this, &SRealGraphPin<RealType>::GetPinValueAsDouble)
			.OnValueCommitted(this, &SRealGraphPin<RealType>::OnPinValueCommitted)
		];
	}

private:
	// This function must exist for the .IsEnabled() binding to work.
	bool GetDefaultValueIsEnabled() const
	{
		return !GraphPinObj->bDefaultValueIsReadOnly;
	}

	// Gets the pin's string value, converts it to RealType, then to double for the UI.
	TOptional<double> GetPinValueAsDouble() const
	{
		const FString DefaultString = GraphPinObj->GetDefaultAsString();
		if (DefaultString.IsEmpty() || DefaultString.Equals(TEXT("()")))
		{
			return TOptional<double>();
		}
		
		FString MutableDefaultString = DefaultString;
		MutableDefaultString.RemoveFromStart(TEXT("("));
		MutableDefaultString.RemoveFromEnd(TEXT(")"));

		const RealType RealValue(MutableDefaultString);
		return RealValue.ToDouble();
	}

	// Takes the double from the UI, converts it to RealType, then to a string for the pin.
	void OnPinValueCommitted(double NewValue, ETextCommit::Type CommitType)
	{
		const RealType RealValue(NewValue);
		const FString TypeValueString = FString::Printf(TEXT("(%s)"), *RealValue.ToString());

		if (GraphPinObj->GetDefaultAsString() != TypeValueString)
		{
			GraphPinObj->Modify();
			GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, *TypeValueString);
		}
	}
};


#define LOCTEXT_NAMESPACE "FRealStructCustomization"

// Customization for a single RealType property.
template<typename RealType>
class FRealStructCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstance()
	{
		return MakeShareable(new FRealStructCustomization<RealType>());
	}
	
private:
	// Gets the property's string value, converts it to RealType, then to double for the UI.
	TOptional<double> GetPropertyAsDouble(TSharedRef<IPropertyHandle> PropertyHandle) const
	{
		TArray<FString> Values;
		PropertyHandle->GetPerObjectValues(Values);

		if (Values.Num() == 1)
		{
			FString Value = Values[0];
			Value.RemoveFromStart(TEXT("("));
			Value.RemoveFromEnd(TEXT(")"));
			const RealType RealValue(Value);
			return RealValue.ToDouble();
		}
		
		return TOptional<double>(); // Return unset for multiple different values
	}

	// Takes the double from the UI, converts it to RealType, then to a string for the property.
	void SetPropertyFromDouble(double NewValue, ETextCommit::Type CommitType, TSharedRef<IPropertyHandle> PropertyHandle)
	{
		const RealType RealValue(NewValue);
		const FString TypeValueString = FString::Printf(TEXT("(%s)"), *RealValue.ToString());

		FScopedTransaction Transaction(LOCTEXT("SetRealStructValue", "Set Real Struct Value"));
		PropertyHandle->NotifyPreChange();
		PropertyHandle->SetPerObjectValues({ TypeValueString });
		PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		PropertyHandle->NotifyFinishedChangingProperties();
	}

public:
	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SNumericEntryBox<double>)
			.Value(this, &FRealStructCustomization<RealType>::GetPropertyAsDouble, PropertyHandle)
			.OnValueCommitted(this, &FRealStructCustomization<RealType>::SetPropertyFromDouble, PropertyHandle)
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
	}

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}
};

#undef LOCTEXT_NAMESPACE