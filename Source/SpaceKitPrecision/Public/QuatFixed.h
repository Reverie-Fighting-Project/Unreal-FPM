// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#pragma once

#include "SpaceKitPrecision/Public/VectorFixed.h"
#include "SpaceKitPrecision/Public/RealFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"

#include "QuatFixed.generated.h"

struct FRotatorFixed;

// Similar to an FQuat, but using deterministic fixed-point reals.
USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FQuatFixed
{
    GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuatFixed")
    FRealFixed X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuatFixed")
    FRealFixed Y;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuatFixed")
    FRealFixed Z;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuatFixed")
    FRealFixed W;
	
    static FQuatFixed Identity;

	// Constructor for an identity quaternion.
    FQuatFixed()
        : X(0), Y(0), Z(0), W(1)
    {
    }

	// Constructor with individual components.
    FQuatFixed(const FRealFixed& InX, const FRealFixed& InY, const FRealFixed& InZ, const FRealFixed& InW)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {
    }

    // Builds a quaternion from a rotation axis and an angle in degrees. Axis is expected to be normalized!
    FQuatFixed(const FVectorFixed& Axis, const FRealFixed& AngleDeg);

    // Builds a quaternion from a standard FQuat.
    explicit FQuatFixed(const FQuat& Quat)
        : X(Quat.X), Y(Quat.Y), Z(Quat.Z), W(Quat.W)
    {
    }
    
    // Builds a quaternion from a fixed-point rotator.
    explicit FQuatFixed(const FRotatorFixed& Rotator);

    // Rotates a given vector by this quaternion. Assumes this quaternion is normalized.
    FVectorFixed RotateVector(const FVectorFixed& V) const
    {
        const FVectorFixed Q_xyz(X, Y, Z);
        const FVectorFixed T = FVectorFixed::CrossProduct(Q_xyz, V) * FRealFixed(2);
        return V + (T * W) + FVectorFixed::CrossProduct(Q_xyz, T);
    }

	// Rotates backward a given vector by this quaternion. Assumes this quaternion is normalized.
    FVectorFixed UnrotateVector(const FVectorFixed& V) const
    {
        return Inverse().RotateVector(V);
    }
    
    // Normalizes this quaternion in-place.
    void Normalize(const FRealFixed& Tolerance = FRealFixed("0.000001"));

    // Returns a normalized copy of this quaternion.
    FQuatFixed GetNormalized(const FRealFixed& Tolerance = FRealFixed("0.000001")) const;

    // Standard Hamiltonian product. Combines this rotation with another.
    FQuatFixed operator*(const FQuatFixed& Other) const;

    FQuatFixed& operator*=(const FQuatFixed& Other)
    {
        *this = *this * Other;
        return *this;
    }

    // Checks for component-wise equality, including if the quaternions are opposites (q == -q).
    bool Equals(const FQuatFixed& Other, const FRealFixed& Tolerance = FRealFixed("0.00001")) const;
    
    bool operator==(const FQuatFixed& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
    }

    // For a unit quaternion, the inverse is the conjugate.
    FQuatFixed Inverse() const
    {
        return FQuatFixed(-X, -Y, -Z, W);
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("(X=%s, Y=%s, Z=%s, W=%s)"), *X.ToString(), *Y.ToString(), *Z.ToString(), *W.ToString());
    }

    FQuat ToFQuat() const
    {
        return FQuat(X.ToFloat(), Y.ToFloat(), Z.ToFloat(), W.ToFloat());
    }
	
    // Gets the axis and angle (in degrees) of this quaternion.
    void ToAxisAndAngle(FVectorFixed& OutAxis, FRealFixed& OutAngleDeg) const;
};

/**
 * Blueprints math library for QuatFixed
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UQuatFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "FQuat to QuatFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FQuatFixed ConvFQuatToQuatFixed(const FQuat& Quat);

    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "QuatFixed to FQuat", CompactNodeTitle = "->", BlueprintAutocast))
    static FQuat ConvQuatFixedToFQuat(const FQuatFixed& Quat);
    
    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Make Quat from Axis and Angle", Keywords = "construct build quaternion", CompactNodeTitle = "Make"))
    static FQuatFixed MakeQuatFromAxisAngle(const FVectorFixed& Axis, const FRealFixed& AngleDeg);

    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Break Quat into Axis and Angle", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakQuatToAxisAngle(const FQuatFixed& Quat, FVectorFixed& Axis, FRealFixed& AngleDeg);
    
    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta=(DisplayName = "Inverse (QuatFixed)", CompactNodeTitle="Inv"))
    static FQuatFixed Inverse(const FQuatFixed& Quat);
    
    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Combine (QuatFixed)", CompactNodeTitle = "*", Keywords = "multiply combine"))
    static FQuatFixed Multiply(const FQuatFixed& A, const FQuatFixed& B);

    UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Equals (QuatFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool Equals(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance);
    
	UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Not Equal (QuatFixed)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
    static bool NotEqual(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance);

	UFUNCTION(BlueprintPure, Category = "Math|QuatFixed", meta = (DisplayName = "Slerp (QuatFixed)", Keywords = "interpolate interpolation", CompactNodeTitle = "Slerp"))
    static FQuatFixed Slerp(FQuatFixed A, FQuatFixed B, const FRealFixed& Alpha);
};