// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RealFixedGeneric.h"
#include "PrecisionSettings.h"

#include "RealFixed.generated.h"

struct FRealFloat;
/**
 * Type for a real number, that uses fixed-point math.
 * It is meant to be used as a native type: you can do arithmetic directly using instances of this class.
 * This is declared as an USTRUCT() only to be integrated into Blueprints. USTRUCT does not add any performance overhead.
 */
USTRUCT(BlueprintType, meta=(HasNativeMake="SpaceKitPrecision.RealFixedMath.MakeReal"))
struct SPACEKITPRECISION_API FRealFixed
{
    GENERATED_BODY()

/*
 * After a LOT of trial and error, we chose this solution to interface big numbers to Unreal Engine:
 * It permits to serialize data automatically, in a way that works with all Unreal Engine systems, including FProperty (only using a custom serializer is not sufficient for editor related stuff).
 * The idea here is to store the data in a c-style (stack allocated) array, that will be used by Unreal Engine internally.
 * It's exposed, for *internal* C++ usage, with the Value reference, that is just a reinterpret_cast reference to the bytes of InternalValue.
 */
protected:
    UPROPERTY()
    uint8 InternalValue[sizeof(real_fixed_type)];
public:
    real_fixed_type& Value;

public:

    FRealFixed();

    FRealFixed(const FRealFixed& InValue);

	explicit FRealFixed(const real_fixed_type& InValue);

    explicit FRealFixed(int32 InValue);

    explicit FRealFixed(int64 InValue);

    explicit FRealFixed(float InValue);

    explicit FRealFixed(double InValue);

    explicit FRealFixed(const char* InValue);

    explicit FRealFixed(const std::string& InValue);

    explicit FRealFixed(const FString& InValue);

    FRealFixed& operator=(const FRealFixed& Other)
    {
        Value = Other.Value;
        return *this;
    };

    // Converts this number to a double number. Note that this can lead to huge precision loss
    double ToDouble() const;

    // Converts this number to a float number. Note that this can lead to huge precision loss
    float ToFloat() const;

    // Converts this number to a floating-point big number. This may not lead to precision loss
    real_fixed_type::ttBigType ToBig() const;

    FString ToString() const;

    explicit operator int32() const
    {
        return ToDouble();
    }

    bool ExportTextItem(FString& ValueStr, FRealFixed const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;
    bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);

    static FRealFixed GetMaxValue()
    {
        return FRealFixed(real_fixed_type::GetMaxValue());
    }

    static FRealFixed GetMinValue()
    {
        return FRealFixed(real_fixed_type::GetMinValue());
    }

    static FRealFixed Pi;
    static FRealFixed HalfPi;
    static FRealFixed DegToRad;
    static FRealFixed RadToDeg;
};

FRealFixed operator""_fx(const char* str);

// Type traits, so Unreal Engine knows FRealFixed implements ExportTextItem and ImportTextItem
template<>
struct TStructOpsTypeTraits<FRealFixed> : public TStructOpsTypeTraitsBase2<FRealFixed>
{
    enum
    {
        WithExportTextItem = true,
        WithImportTextItem = true,
    };
};

inline FRealFixed operator+(const FRealFixed& x, const FRealFixed& y)
{
    return FRealFixed(x.Value + y.Value);
}

inline FRealFixed operator+=(FRealFixed& x, const FRealFixed& y)
{
    return x = x + y;
}

inline FRealFixed operator-(const FRealFixed& x, const FRealFixed& y)
{
    return FRealFixed(x.Value - y.Value);
}

inline FRealFixed operator-(double x, const FRealFixed& y)
{
    return FRealFixed(x) - y;
}

inline FRealFixed operator-=(FRealFixed& x, const FRealFixed& y)
{
    return x = FRealFixed(x - y);
}

inline FRealFixed operator-(const FRealFixed& x)
{
    return FRealFixed(-x.Value);
}

inline FRealFixed operator*(const FRealFixed& x, const FRealFixed& y)
{
    return FRealFixed(x.Value * y.Value);
}

inline FRealFixed operator*=(FRealFixed& x, const FRealFixed& y)
{
    return x = x * y;
}

inline FRealFixed operator/(const FRealFixed& x, const FRealFixed& y)
{
    return FRealFixed(x.Value / y.Value);
}

inline FRealFixed operator/=(FRealFixed& x, const FRealFixed& y)
{
    return x = x / y;
}

inline FRealFixed operator%(const FRealFixed& x, const FRealFixed& y)
{
    return FRealFixed(x.Value % y.Value);
}

inline FRealFixed operator%=(FRealFixed& x, const FRealFixed& y)
{
    return x = x % y;
}

inline bool operator<(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value < y.Value;
}

inline bool operator<=(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value <= y.Value;
}

inline bool operator>=(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value >= y.Value;
}

inline bool operator>(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value > y.Value;
}

inline bool operator==(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value == y.Value;
}

inline bool operator==(const FRealFixed& x, double y)
{
    return x == FRealFixed(y);
}

inline bool operator==(double x, const FRealFixed& y)
{
    return FRealFixed(x) == y;
}

inline bool operator!=(const FRealFixed& x, const FRealFixed& y)
{
    return x.Value != y.Value;
}

/**
 * Blueprints math library for RealFixed
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API URealFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

// Basic conversions
public:

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed to String", CompactNodeTitle = "->", BlueprintAutocast))
    static FString ConvRealToString(const FRealFixed& InReal);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed to Float", CompactNodeTitle = "->", BlueprintAutocast))
    static float ConvRealToFloat(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "String to RealFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FRealFixed ConvStringToReal(const FString& InVal);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Float to RealFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FRealFixed ConvFloatToReal(float InVal);

// Basic RealFixed math
public:

    UFUNCTION(BlueprintPure, category = "RealFixed", meta=(DisplayName = "RealFixed + RealFixed", CompactNodeTitle="+"))
    static FRealFixed RealPlusReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta=(DisplayName = "RealFixed - RealFixed", CompactNodeTitle="-"))
    static FRealFixed RealMinusReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta=(DisplayName = "RealFixed * RealFixed", CompactNodeTitle="*"))
    static FRealFixed RealMultReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta=(DisplayName = "RealFixed / RealFixed", CompactNodeTitle="/"))
    static FRealFixed RealDivReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta=(DisplayName = "RealFixed / RealFixed", CompactNodeTitle="%"))
    static FRealFixed RealModReal(const FRealFixed& First, const FRealFixed& Second);
    
    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed == RealFixed", CompactNodeTitle = "=="))
    static bool RealEqualsReal(const FRealFixed& First, const FRealFixed& Second, const FRealFixed& Tolerance);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed != RealFixed", CompactNodeTitle = "!="))
    static bool RealNotEqualsReal(const FRealFixed& First, const FRealFixed& Second, const FRealFixed& Tolerance);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed < RealFixed", CompactNodeTitle = "<"))
    static bool RealInfReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed <= RealFixed", CompactNodeTitle = "<="))
    static bool RealInfEqReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed > RealFixed", CompactNodeTitle = ">"))
    static bool RealSupReal(const FRealFixed& First, const FRealFixed& Second);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "RealFixed >= RealFixed", CompactNodeTitle = ">="))
    static bool RealSupEqReal(const FRealFixed& First, const FRealFixed& Second);

// Advanced RealFixed math
public:
    
    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Sqrt RealFixed", CompactNodeTitle = "Sqrt"))
    static FRealFixed Sqrt(const FRealFixed& Val);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "LogE RealFixed", CompactNodeTitle = "LogE"))
    static FRealFixed LogE(const FRealFixed& Val);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Log2 RealFixed", CompactNodeTitle = "Log2"))
    static FRealFixed Log2(const FRealFixed& Val);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Log10 RealFixed", CompactNodeTitle = "Log10"))
    static FRealFixed Log10(const FRealFixed& Val);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Min RealFixed", CompactNodeTitle = "Min"))
    static FRealFixed Min(const FRealFixed& Val, const FRealFixed& Min);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Max RealFixed", CompactNodeTitle = "Max"))
    static FRealFixed Max(const FRealFixed& Val, const FRealFixed& Max);
	
    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Clamp RealFixed", CompactNodeTitle = "Clamp"))
    static FRealFixed Clamp(const FRealFixed& Val, const FRealFixed& Min, const FRealFixed& Max);

    UFUNCTION(BlueprintPure, category = "RealFixed", meta = (DisplayName = "Abs RealFixed", CompactNodeTitle = "Abs"))
    static FRealFixed Abs(const FRealFixed& Val);

    // 03-07-2025 Added Parity with RealFloat on missing functions - Koenji

    // -- MISSING TRIGONOMETRIC AND ANGLE FUNCTIONS --

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Normalize Angle (Radians)"))
    static FRealFixed NormalizeAngleRad(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Sin (Radians)", CompactNodeTitle = "SINr"))
    static FRealFixed SinRad(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Cos (Radians)", CompactNodeTitle = "COSr"))
    static FRealFixed CosRad(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Tan (Radians)", CompactNodeTitle = "TANr"))
    static FRealFixed TanRad(const FRealFixed& InVal);
    
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Normalize Angle (Degrees)"))
    static FRealFixed NormalizeAngleDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Sin (Degrees)", CompactNodeTitle = "SINd"))
    static FRealFixed SinDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Cos (Degrees)", CompactNodeTitle = "COSd"))
    static FRealFixed CosDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Tan (Degrees)", CompactNodeTitle = "TANd"))
    static FRealFixed TanDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Asin (Radians)", CompactNodeTitle = "ASINr"))
    static FRealFixed AsinRad(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Acos (Radians)", CompactNodeTitle = "ACOSr"))
    static FRealFixed AcosRad(const FRealFixed& InVal);
    
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Atan (Radians)", CompactNodeTitle = "ATANr"))
    static FRealFixed AtanRad(const FRealFixed& InVal);
    
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Atan2 (Radians)", CompactNodeTitle = "ATAN2r"))
    static FRealFixed Atan2Rad(const FRealFixed& Y, const FRealFixed& X);
    
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Asin (Degrees)", CompactNodeTitle = "ASINd"))
    static FRealFixed AsinDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Acos (Degrees)", CompactNodeTitle = "ACOSd"))
    static FRealFixed AcosDeg(const FRealFixed& InVal);
    
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Atan (Degrees)", CompactNodeTitle = "ATANd"))
    static FRealFixed AtanDeg(const FRealFixed& InVal);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Atan2 (Degrees)", CompactNodeTitle = "ATAN2d"))
    static FRealFixed Atan2Deg(const FRealFixed& Y, const FRealFixed& X);


    // -- MISSING EXPONENTIAL AND POWER FUNCTIONS --

    UFUNCTION(BlueprintPure, category = "Math|RealFixed", meta = (DisplayName = "Pow", CompactNodeTitle = "Pow"))
    static FRealFixed Pow(const FRealFixed& Base, const FRealFixed& Exp);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed", meta = (DisplayName = "Exp", CompactNodeTitle = "Exp"))
    static FRealFixed Exp(const FRealFixed& Val);


    // -- MISSING UTILITY FUNCTIONS --

    UFUNCTION(BlueprintPure, category = "Math|RealFixed", meta = (DisplayName = "Sign", CompactNodeTitle = "Sign"))
    static FRealFixed Sign(const FRealFixed& Val);

    // Also adding InvSqrt for convenience, as it's often used in vector/quat math.
    UFUNCTION(BlueprintPure, category = "Math|RealFixed", meta = (DisplayName = "Inverse Sqrt", CompactNodeTitle = "InvSqrt", Keywords = "1/sqrt"))
    static FRealFixed InvSqrt(const FRealFixed& Val);

    // Also adding conversion helpers for convenience.
    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Degrees To Radians", CompactNodeTitle = "DegToRad"))
    static FRealFixed DegreesToRadians(const FRealFixed& Deg);

    UFUNCTION(BlueprintPure, category = "Math|RealFixed|Trig", meta = (DisplayName = "Radians To Degrees", CompactNodeTitle = "RadToDeg"))
    static FRealFixed RadiansToDegrees(const FRealFixed& Rad);

};
