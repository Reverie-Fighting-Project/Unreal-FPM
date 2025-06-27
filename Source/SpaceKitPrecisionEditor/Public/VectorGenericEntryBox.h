// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "PropertyHandle.h"
#include "RotatorFloat.h" // Ensure RotatorFloat is included

class FArguments;
class SHorizontalBox;

#define LOCTEXT_NAMESPACE "SVectorGenericEntryBox"

// Widget for editing vector types, with three axes.
template<typename VectorType, typename ScalarType>
class SVectorGenericEntryBox : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS( SVectorGenericEntryBox )
		: _Font( FCoreStyle::Get().GetFontStyle("NormalFont") )
		, _bColorAxisLabels( false )
		{}

		SLATE_ARGUMENT( TSharedPtr<IPropertyHandle>, PropertyHandle )
		SLATE_ATTRIBUTE( TOptional<VectorType>, Vector )
		SLATE_ATTRIBUTE( FSlateFontInfo, Font )
		SLATE_ARGUMENT(FText, LabelX)
		SLATE_ARGUMENT(FText, LabelY)
		SLATE_ARGUMENT(FText, LabelZ)
		SLATE_ARGUMENT(FText, TooltipX)
		SLATE_ARGUMENT(FText, TooltipY)
		SLATE_ARGUMENT(FText, TooltipZ)
		SLATE_ARGUMENT( bool, bColorAxisLabels )
		
		// Note: The delegate is no longer needed here as the widget now handles the property directly.
		
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs )
	{
		TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
		ChildSlot[HorizontalBox];
		
		PropertyHandle = InArgs._PropertyHandle;
		VectorAttribute = InArgs._Vector;

		ConstructAxis(EAxis::X, InArgs, HorizontalBox);
		ConstructAxis(EAxis::Y, InArgs, HorizontalBox);
		ConstructAxis(EAxis::Z, InArgs, HorizontalBox);
	}

private:
	TSharedPtr<IPropertyHandle> PropertyHandle;
	TAttribute<TOptional<VectorType>> VectorAttribute;

	void ConstructAxis(EAxis::Type Axis, const FArguments& InArgs, TSharedRef<SHorizontalBox> HorizontalBox)
	{
		const auto GetAxisValue = [](EAxis::Type InAxis, auto x, auto y, auto z)
		{
			return (InAxis == EAxis::X) ? x : ((InAxis == EAxis::Y) ? y : z);
		};

		const FLinearColor LabelColor = InArgs._bColorAxisLabels
			? GetAxisValue(Axis, SNumericEntryBox<double>::RedLabelBackgroundColor, SNumericEntryBox<double>::GreenLabelBackgroundColor, SNumericEntryBox<double>::BlueLabelBackgroundColor)
			: FLinearColor(0.0f, 0.0f, 0.0f, .5f);

		HorizontalBox->AddSlot()
		.VAlign(VAlign_Center)
		.FillWidth(1.f)
		.Padding(0.0f, 1.f, GetAxisValue(Axis, 2.f, 2.f, 0.f), 1.f)
		[
			SNew(SNumericEntryBox<double>)
			.Font(InArgs._Font)
			.Value(this, &SVectorGenericEntryBox::GetComponentValue, Axis)
			.OnValueCommitted(this, &SVectorGenericEntryBox::OnComponentCommitted, Axis)
			.ToolTipText(GetAxisValue(Axis, InArgs._TooltipX, InArgs._TooltipY, InArgs._TooltipZ))
			.UndeterminedString(LOCTEXT("MultipleValues", "---"))
			.LabelPadding(0)
			.Label()
			[
				SNumericEntryBox<float>::BuildLabel(GetAxisValue(Axis, InArgs._LabelX, InArgs._LabelY, InArgs._LabelZ), FLinearColor::White, LabelColor)
			]
		];		
	}

	TOptional<double> GetComponentValue(EAxis::Type Axis) const
	{
		if (PropertyHandle.IsValid())
		{
			TArray<void*> RawData;
			PropertyHandle->AccessRawData(RawData);
			if (RawData.Num() == 1)
			{
				const VectorType* VecValue = static_cast<const VectorType*>(RawData[0]);
				if constexpr (std::is_same_v<VectorType, FRotatorFloat>)
				{
					if (Axis == EAxis::X) { return VecValue->Roll.ToDouble(); }
					if (Axis == EAxis::Y) { return VecValue->Pitch.ToDouble(); }
					return VecValue->Yaw.ToDouble();
				}
				else
				{
					if (Axis == EAxis::X) { return VecValue->X.ToDouble(); }
					if (Axis == EAxis::Y) { return VecValue->Y.ToDouble(); }
					return VecValue->Z.ToDouble();
				}
			}
		}
		return TOptional<double>(); // Unset for multi-selection
	}

	void OnComponentCommitted(double NewValue, ETextCommit::Type CommitType, EAxis::Type Axis)
	{
		if(PropertyHandle.IsValid())
		{
			// Corrected transaction text.
			FText TransactionText = (Axis == EAxis::X) ? LOCTEXT("SetComponentX", "Set X") : (Axis == EAxis::Y) ? LOCTEXT("SetComponentY", "Set Y") : LOCTEXT("SetComponentZ", "Set Z");
			FScopedTransaction Transaction(TransactionText);

			PropertyHandle->NotifyPreChange();
			
			TArray<void*> RawData;
			PropertyHandle->AccessRawData(RawData);
			for (void* Data : RawData)
			{
				VectorType* VecValue = static_cast<VectorType*>(Data);

				// THE DEFINITIVE FIX: Access members directly for all types.
				if constexpr (std::is_same_v<VectorType, FRotatorFloat>)
				{
					if (Axis == EAxis::X) { VecValue->Roll = ScalarType(NewValue); }
					else if (Axis == EAxis::Y) { VecValue->Pitch = ScalarType(NewValue); }
					else { VecValue->Yaw = ScalarType(NewValue); }
				}
				else
				{
					if (Axis == EAxis::X) { VecValue->X = ScalarType(NewValue); }
					else if (Axis == EAxis::Y) { VecValue->Y = ScalarType(NewValue); }
					else { VecValue->Z = ScalarType(NewValue); }
				}
			}
			
			PropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
		}
	}
};

#undef LOCTEXT_NAMESPACE