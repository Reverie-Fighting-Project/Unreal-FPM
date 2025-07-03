// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#include "SpaceKitPrecision/Public/RealFixed.h"
#include "RealFloat.h"


FRealFixed::FRealFixed()
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = 0.0;
}

FRealFixed::FRealFixed(const FRealFixed& InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue.Value;
}

FRealFixed::FRealFixed(const real_fixed_type& InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(int32 InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(int64 InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(float InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(double InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}
FRealFixed::FRealFixed(const char* InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(const std::string& InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

FRealFixed::FRealFixed(const FString& InValue)
    : Value(*reinterpret_cast<real_fixed_type*>(InternalValue))
{
    Value = InValue;
}

// Converts this number to a double number. Note that this can lead to huge precision loss
double FRealFixed::ToDouble() const
{
    return Value.ToDouble();
}

// Converts this number to a float number. Note that this can lead to huge precision loss
float FRealFixed::ToFloat() const
{
    return Value.ToFloat();
}

// Converts this number to a floating-point big number. This may not lead to precision loss
real_fixed_type::ttBigType FRealFixed::ToBig() const
{
    return Value.ToBig();
}

FString FRealFixed::ToString() const
{
    return Value.ToString();
}

bool FRealFixed::ExportTextItem(FString& ValueStr, FRealFixed const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
    ValueStr += FString::Printf(TEXT("(%s)"), *ToString());
    return true;
}

bool FRealFixed::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
    FString MutableString = Buffer;
    const int32 Len = MutableString.Find(")") + 1;
    Buffer += Len;

    MutableString = MutableString.Left(Len);
    MutableString.RemoveFromStart("(");
    MutableString.RemoveFromEnd(")");

    Value = MutableString;
    return true;
}


// Conversions

FString URealFixedMath::ConvRealToString(const FRealFixed& InReal)
{
    return FString::Printf(TEXT("%s"), *InReal.ToString());
}

float URealFixedMath::ConvRealToFloat(const FRealFixed& Val)
{
    return Val.Value.ToFloat();
}

FRealFixed URealFixedMath::ConvStringToReal(const FString& InString)
{
    return FRealFixed(InString);
}

FRealFixed URealFixedMath::ConvFloatToReal(float Val)
{
    return FRealFixed(Val);
}

// Real math

FRealFixed URealFixedMath::RealPlusReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First + Second;
}

FRealFixed URealFixedMath::RealMinusReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First - Second;
}

FRealFixed URealFixedMath::RealMultReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First * Second;
}

FRealFixed URealFixedMath::RealDivReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First / Second;
}

FRealFixed URealFixedMath::RealModReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First % Second;
}

bool URealFixedMath::RealEqualsReal(const FRealFixed& First, const FRealFixed& Second, const FRealFixed& Tolerance)
{
    return URealFixedMath::Abs(First - Second) < Tolerance;
}

bool URealFixedMath::RealNotEqualsReal(const FRealFixed& First, const FRealFixed& Second, const FRealFixed& Tolerance)
{
    return !URealFixedMath::RealEqualsReal(First, Second, Tolerance);
}

bool URealFixedMath::RealInfReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First < Second;
}

bool URealFixedMath::RealInfEqReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First <= Second;
}

bool URealFixedMath::RealSupReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First > Second;
}

bool URealFixedMath::RealSupEqReal(const FRealFixed& First, const FRealFixed& Second)
{
    return First >= Second;
}

// Advanced FRealFixed math

FRealFixed URealFixedMath::Sqrt(const FRealFixed& Val)
{
    return FRealFixed(ttmath::Sqrt((Val.ToBig())));
}

FRealFixed URealFixedMath::LogE(const FRealFixed& Val)
{
    return FRealFixed(ttmath::Ln(Val.ToBig()));
}

FRealFixed URealFixedMath::Log2(const FRealFixed& Val)
{
    return FRealFixed(ttmath::Log(Val.ToBig(), real_fixed_type::ttBigType(2)));
}

FRealFixed URealFixedMath::Log10(const FRealFixed& Val)
{
    return FRealFixed(ttmath::Log(Val.ToBig(), real_fixed_type::ttBigType(10)));
}

FRealFixed URealFixedMath::Min(const FRealFixed& Val, const FRealFixed& InMin)
{
    return FRealFixed(Val > InMin ? Val : InMin);
}

FRealFixed URealFixedMath::Max(const FRealFixed& Val, const FRealFixed& InMax)
{
    return FRealFixed(Val > InMax ? Val : InMax);
}

FRealFixed URealFixedMath::Clamp(const FRealFixed& Val, const FRealFixed& InMin, const FRealFixed& InMax)
{
    return Max(Min(Val, InMin), InMax);
}

FRealFixed URealFixedMath::Abs(const FRealFixed& Val)
{
	return Val < 0_fx ? -Val : Val;
}

FRealFixed operator""_fx(const char* str)
{
    return FRealFixed(str);
}

FRealFixed URealFixedMath::DegreesToRadians(const FRealFixed& Deg)
{
    // Assumes FRealFixed::DegToRad is a static member like in FRealFloat
    return Deg * FRealFixed::DegToRad;
}

FRealFixed URealFixedMath::RadiansToDegrees(const FRealFixed& Rad)
{
    // Assumes FRealFixed::RadToDeg is a static member
    return Rad * FRealFixed::RadToDeg;
}

FRealFixed URealFixedMath::Sign(const FRealFixed& Val)
{
    // The original implementation in RealFloatMath was incorrect.
    // This version correctly returns -1 for negative, 1 for positive, and 0 for zero.
    // It is equivalent to: (Val > FRealFixed(0)) ? FRealFixed(1) : ((Val < FRealFixed(0)) ? FRealFixed(-1) : FRealFixed(0));
    return FRealFixed(ttmath::Sgn(Val.Value));
}

FRealFixed URealFixedMath::InvSqrt(const FRealFixed& Val)
{
    // 1.0 / Sqrt(Val)
    return FRealFixed(1) / Sqrt(Val);
}

// 03-07-2025 - Further Parity between RealFloat and RealFixed - Koenji

FRealFixed FRealFixed::Pi = FRealFixed("3.1415926535897932384626433832795");
FRealFixed FRealFixed::HalfPi = FRealFixed::Pi / FRealFixed(2);
FRealFixed FRealFixed::DegToRad = FRealFixed::Pi / FRealFixed(180);
FRealFixed FRealFixed::RadToDeg = FRealFixed(180) / FRealFixed::Pi;


// -- ANGLE NORMALIZATION --

FRealFixed URealFixedMath::NormalizeAngleRad(const FRealFixed& InVal)
{
    // Wraps the angle to the range [-Pi, Pi]
    const FRealFixed TwoPi = FRealFixed::Pi * FRealFixed(2);
    FRealFixed Result = RealModReal(InVal, TwoPi);

    if (Result > FRealFixed::Pi)
    {
        Result -= TwoPi;
    }
    else if (Result < -FRealFixed::Pi)
    {
        Result += TwoPi;
    }
    return Result;
}

FRealFixed URealFixedMath::NormalizeAngleDeg(const FRealFixed& InVal)
{
    // Wraps the angle to the range [-180, 180]
    FRealFixed Result = RealModReal(InVal, FRealFixed(360));

    if (Result > FRealFixed(180))
    {
        Result -= FRealFixed(360);
    }
    else if (Result < FRealFixed(-180))
    {
        Result += FRealFixed(360);
    }
    return Result;
}


// -- TRIGONOMETRIC FUNCTIONS --

FRealFixed URealFixedMath::SinRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::Sin(InVal.Value));
}

FRealFixed URealFixedMath::CosRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::Cos(InVal.Value));
}

FRealFixed URealFixedMath::TanRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::Tan(InVal.Value));
}

FRealFixed URealFixedMath::SinDeg(const FRealFixed& InVal)
{
    return SinRad(DegreesToRadians(InVal));
}

FRealFixed URealFixedMath::CosDeg(const FRealFixed& InVal)
{
    return CosRad(DegreesToRadians(InVal));
}

FRealFixed URealFixedMath::TanDeg(const FRealFixed& InVal)
{
    return TanRad(DegreesToRadians(InVal));
}


// -- INVERSE TRIGONOMETRIC FUNCTIONS --

FRealFixed URealFixedMath::AsinRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::ASin(InVal.Value));
}

FRealFixed URealFixedMath::AcosRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::ACos(InVal.Value));
}

FRealFixed URealFixedMath::AtanRad(const FRealFixed& InVal)
{
    return FRealFixed(ttmath::ATan(InVal.Value));
}

FRealFixed URealFixedMath::Atan2Rad(const FRealFixed& Y, const FRealFixed& X)
{
    // This is a direct port of the logic from URealFloatMath::Atan2Rad.

    const FRealFixed Zero(0);

    // Handle the undefined case at the origin
    if (X == Zero && Y == Zero)
    {
        return Zero;
    }

    // Handle cases on the X-axis
    if (Y == Zero)
    {
        if (X > Zero)
        {
            return Zero;
        }
        else
        {
            // The original library uses -Pi, which is coterminal with Pi. We will be consistent.
            return -FRealFixed::Pi;
        }
    }

    // Handle cases on the Y-axis
    if (X == Zero)
    {
        if (Y > Zero)
        {
            return FRealFixed::HalfPi;
        }
        else
        {
            return -FRealFixed::HalfPi;
        }
    }

    // General case for the four quadrants
    FRealFixed Angle = AtanRad(Y / X);

    // Correct the angle for the left-half plane (Quadrants II and III)
    if (X < Zero)
    {
        // Adding Pi corrects the angle. Normalization handles Quadrant III correctly.
        Angle = NormalizeAngleRad(Angle + FRealFixed::Pi);
    }

    return Angle;
}

FRealFixed URealFixedMath::AsinDeg(const FRealFixed& InVal)
{
    return RadiansToDegrees(AsinRad(InVal));
}

FRealFixed URealFixedMath::AcosDeg(const FRealFixed& InVal)
{
    return RadiansToDegrees(AcosRad(InVal));
}

FRealFixed URealFixedMath::AtanDeg(const FRealFixed& InVal)
{
    return RadiansToDegrees(AtanRad(InVal));
}

FRealFixed URealFixedMath::Atan2Deg(const FRealFixed& Y, const FRealFixed& X)
{
    return RadiansToDegrees(Atan2Rad(Y, X));
}


// -- EXPONENTIAL AND POWER FUNCTIONS --

FRealFixed URealFixedMath::Pow(const FRealFixed& Base, const FRealFixed& Exp)
{
    // a^b = e^(b*ln(a))
    return FRealFixed(ttmath::Exp(Exp.Value * ttmath::Ln(Base.Value)));
}

FRealFixed URealFixedMath::Exp(const FRealFixed& Val)
{
    return FRealFixed(ttmath::Exp(Val.Value));
}
