// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"
#include "Widgets/Text/STextBlock.h"
#include "VectorGenericEntryBox.h"
#include "RotatorFloat.h" 

#define LOCTEXT_NAMESPACE "FVectorGenericStructCustomization"

template<typename VectorType, typename RealType>
class FVectorGenericStructCustomization : public IPropertyTypeCustomization
{
public:
	static TSharedRef<IPropertyTypeCustomization> MakeInstanceDefaults()
	{
		return MakeShareable(new FVectorGenericStructCustomization<VectorType, RealType>(
			LOCTEXT("LabelX", "X"), LOCTEXT("LabelY", "Y"), LOCTEXT("LabelZ", "Z"),
			LOCTEXT("TooltipX", "X"), LOCTEXT("TooltipY", "Y"), LOCTEXT("TooltipZ", "Z")
		));
	}
	
	static TSharedRef<IPropertyTypeCustomization> MakeInstance(
		FText InLabelX, FText InLabelY, FText InLabelZ,
		FText InTooltipX, FText InTooltipY, FText InTooltipZ)
	{
		return MakeShareable(new FVectorGenericStructCustomization<VectorType, RealType>(InLabelX, InLabelY, InLabelZ, InTooltipX, InTooltipY, InTooltipZ));
	}

	FVectorGenericStructCustomization(
		FText InLabelX, FText InLabelY, FText InLabelZ,
		FText InTooltipX, FText InTooltipY, FText InTooltipZ)
		:
		LabelX(InLabelX), LabelY(InLabelY), LabelZ(InLabelZ),
		TooltipX(InTooltipX), TooltipY(InTooltipY), TooltipZ(InTooltipZ)
	{
	}

public:
	void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		// Restore the correct layout using the custom SVectorGenericEntryBox
		using SVectorEntryBox = SVectorGenericEntryBox<VectorType, RealType>;
		
		HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(250)
		[
			SNew(SVectorEntryBox)
			.PropertyHandle(PropertyHandle)
			.LabelX(LabelX)
			.LabelY(LabelY)
			.LabelZ(LabelZ)
			.TooltipX(TooltipX)
			.TooltipY(TooltipY)
			.TooltipZ(TooltipZ)
			.bColorAxisLabels(true)
		];
	}

	void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override
	{
		// This can be empty as the header handles the UI.
	}

protected:
	FText LabelX, LabelY, LabelZ;
	FText TooltipX, TooltipY, TooltipZ;
};

#undef LOCTEXT_NAMESPACE