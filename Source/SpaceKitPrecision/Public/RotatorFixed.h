// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#pragma once

#include "SpaceKitPrecision/Public/RealFixed.h"
#include "SpaceKitPrecision/Public/VectorFixed.h"

#include "RotatorFixed.generated.h"

// Forward declaration for the fixed-point quaternion.
// It's assumed a 'FQuatFixed' struct exists or will be created, similar to this rotator.
struct FQuatFixed;

/*
 * Similar to an FRotator, but using deterministic fixed-point reals instead of floats.
 */
USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FRotatorFixed
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorFixed")
    FRealFixed Pitch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorFixed")
    FRealFixed Yaw;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RotatorFixed")
    FRealFixed Roll;

    static FRotatorFixed Identity;

    FRotatorFixed()
        : Pitch(0.0), Yaw(0.0), Roll(0.0)
    {
    }

    FRotatorFixed(FRealFixed InPitch, FRealFixed InYaw, FRealFixed InRoll)
        : Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
    {
    }

    explicit FRotatorFixed(const FRotator& InRot)
        : Pitch(InRot.Pitch), Yaw(InRot.Yaw), Roll(InRot.Roll)
    {
    }

    // Builds a rotator from a fixed-point quaternion
    explicit FRotatorFixed(const FQuatFixed& Rotator);

// Rotator math
public:
	
    FRotatorFixed operator+(const FRotatorFixed& Other) const
    {
        return FRotatorFixed(Pitch + Other.Pitch, Yaw + Other.Yaw, Roll + Other.Roll);
    }

    FRotatorFixed& operator+=(const FRotatorFixed& Other)
    {
        Pitch += Other.Pitch;
        Yaw += Other.Yaw;
        Roll += Other.Roll;
        return *this;
    }

    FRotatorFixed operator-(const FRotatorFixed& Other) const
    {
        return FRotatorFixed(Pitch - Other.Pitch, Yaw - Other.Yaw, Roll - Other.Roll);
    }

    FRotatorFixed& operator-=(const FRotatorFixed& Other)
    {
        Pitch -= Other.Pitch;
        Yaw -= Other.Yaw;
        Roll -= Other.Roll;
        return *this;
    }

    FRotatorFixed operator*(FRealFixed Other) const
    {
        return FRotatorFixed(Pitch * Other, Yaw * Other, Roll * Other);
    }

    FRotatorFixed& operator*=(FRealFixed Other)
    {
        Pitch *= Other;
        Yaw *= Other;
        Roll *= Other;
        return *this;
    }

    FRotatorFixed operator/(FRealFixed Other) const
    {
        return FRotatorFixed(Pitch / Other, Yaw / Other, Roll / Other);
    }

    FRotatorFixed& operator/=(FRealFixed Other)
    {
        Pitch /= Other;
        Yaw /= Other;
        Roll /= Other;
        return *this;
    }

    FRotatorFixed operator-() const
    {
        return FRotatorFixed(-Pitch, -Yaw, -Roll);
    }

    FRotator ToFRotator() const
    {
        // Note: Standard FRotator constructor is (Pitch, Yaw, Roll)
        return FRotator(Pitch.ToFloat(), Yaw.ToFloat(), Roll.ToFloat());
    }

    bool Equals(const FRotatorFixed& Other, FRealFixed Tolerance = FRealFixed("0.000001")) const
    {
        return (URealFixedMath::Abs(Yaw - Other.Yaw) <= Tolerance)
            && (URealFixedMath::Abs(Pitch - Other.Pitch) <= Tolerance)
            && (URealFixedMath::Abs(Roll - Other.Roll) <= Tolerance);
    }

    bool operator==(const FRotatorFixed& Other) const
    {
        return Pitch == Other.Pitch && Yaw == Other.Yaw && Roll == Other.Roll;
    }

    bool operator!=(const FRotatorFixed& Other) const
    {
        return Pitch != Other.Pitch || Yaw != Other.Yaw || Roll != Other.Roll;
    }

    // Rotates a given vector by this rotator
    FVectorFixed RotateVector(const FVectorFixed& Vec) const;

    // Rotates backward a given vector by this rotator
    FVectorFixed UnrotateVector(const FVectorFixed& Vec) const;

    FString ToString() const
    {
        return FString::Printf(TEXT("(Pitch=%s, Yaw=%s, Roll=%s)"), *Pitch.ToString(), *Yaw.ToString(), *Roll.ToString());
    }

    FRealFixed& GetAxis(EAxis::Type Axis)
    {
        switch (Axis)
        {
        case EAxis::X: return Roll;
        case EAxis::Y: return Pitch;
        case EAxis::Z: default: return Yaw;
        }
    }

    const FRealFixed& GetAxis(EAxis::Type Axis) const
    {
        switch (Axis)
        {
        case EAxis::X: return Roll;
        case EAxis::Y: return Pitch;
        case EAxis::Z: default: return Yaw;
        }
    }
};

/**
 * Blueprints math library for RotatorFixed
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API URotatorFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
// Basic conversions
public:

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "FRotator to RotatorFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FRotatorFixed ConvFRotatorToRotatorFixed(const FRotator& InRot);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "RotatorFixed to FRotator", CompactNodeTitle = "->", BlueprintAutocast))
    static FRotator ConvRotatorFixedToFRotator(const FRotatorFixed& InRot);
    
    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "Make RotatorFixed", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FRotatorFixed MakeRotatorFixed(const FRealFixed& Pitch, const FRealFixed& Yaw, const FRealFixed& Roll);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "Break RotatorFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakRotatorFixed(const FRotatorFixed& Rot, FRealFixed& Pitch, FRealFixed& Yaw, FRealFixed& Roll);

// Basic RotFixed math
public:

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta=(DisplayName = "RotatorFixed + RotatorFixed", CompactNodeTitle="+"))
    static FRotatorFixed RotPlusRot(const FRotatorFixed& A, const FRotatorFixed& B);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta=(DisplayName = "RotatorFixed - RotatorFixed", CompactNodeTitle="-"))
    static FRotatorFixed RotMinusRot(const FRotatorFixed& A, const FRotatorFixed& B);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta=(DisplayName = "RotatorFixed * RealFixed", CompactNodeTitle="*"))
    static FRotatorFixed RotMultReal(const FRotatorFixed& A, const FRealFixed& B);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta=(DisplayName = "RotatorFixed / RealFixed", CompactNodeTitle="/"))
    static FRotatorFixed RotDivReal(const FRotatorFixed& A, const FRealFixed& B);

    UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "Equals (RotatorFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool RotEqualsRot(const FRotatorFixed& A, const FRotatorFixed& B, const FRealFixed& Tolerance);

	UFUNCTION(BlueprintPure, category = "Math|RotatorFixed", meta = (DisplayName = "Not Equal (RotatorFixed)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
    static bool RotNotEqualsRot(const FRotatorFixed& A, const FRotatorFixed& B, const FRealFixed& Tolerance);

};