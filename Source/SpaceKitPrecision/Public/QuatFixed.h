// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#pragma once

#include "SpaceKitPrecision/Public/RealFixed.h"
#include "SpaceKitPrecision/Public/VectorFixed.h"

#include "QuatFixed.generated.h"

// Forward declaration for the fixed-point rotator.
struct FRotatorFixed;

// Similar to an FQuat, but using deterministic fixed-point reals instead of floats
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

	// Constructor for an identity quaternion
    FQuatFixed()
        : X("0.0"), Y("0.0"), Z("0.0"), W("1.0")
    {
    }

	// Constructor with individual axes (X, y, z, W). To create a quaternion from axis and angle, see the other constructor or MakeFromAxisAngle.
    FQuatFixed(const FRealFixed& nX, const FRealFixed& nY, const FRealFixed& nZ, const FRealFixed& nW)
        : X(nX), Y(nY), Z(nZ), W(nW)
    {
    }

    // Builds a quaternion from a rotation axis and the angle to rotate in degrees. Axis is expected to be normalized!
    FQuatFixed(const FVectorFixed& Axis, FRealFixed AngleDeg)
    {
        const FRealFixed HalfAngle = FRealFixed("0.5") * AngleDeg;
        const FRealFixed Cos = URealFixedMath::CosDeg(HalfAngle);
        const FRealFixed Sin = URealFixedMath::SinDeg(HalfAngle);

        X = Sin * Axis.X;
        Y = Sin * Axis.Y;
        Z = Sin * Axis.Z;
        W = Cos;
    }

    // Builds a quaternion from a standard FQuat
    explicit FQuatFixed(const FQuat& Quat)
        : X(Quat.X), Y(Quat.Y), Z(Quat.Z), W(Quat.W)
    {
    }
    
    // Builds a quaternion from a fixed-point rotator
    explicit FQuatFixed(const FRotatorFixed& Rotator);

    // Rotates a given vector by this quaternion. Assumes this quaternion is normalized.
    FVectorFixed RotateVector(const FVectorFixed& Vec) const
    {
        // Engine implementation: Q.RotateVector(V) = V + 2 * cross(Q.v, cross(Q.v, V) + Q.w * V)
        const FVectorFixed Q_xyz(X, Y, Z);
        const FVectorFixed T = FVectorFixed::CrossProduct(Q_xyz, Vec) * FRealFixed(2);
        return Vec + (T * W) + FVectorFixed::CrossProduct(Q_xyz, T);
    }

	// Rotates backward a given vector by this quaternion. Assumes this quaternion is normalized.
    FVectorFixed UnrotateVector(const FVectorFixed& Vec) const
    {
        const FVectorFixed Q_xyz(-X, -Y, -Z);
        const FVectorFixed T = FVectorFixed::CrossProduct(Q_xyz, Vec) * FRealFixed(2);
        return Vec + (T * W) + FVectorFixed::CrossProduct(Q_xyz, T);
    }

	// Normalizes this quaternion. Note that you can only apply a rotation to a vector using a normalized quaternion.
    void Normalize(FRealFixed Tolerance = FRealFixed("0.000001"))
    {
        const FRealFixed SquareSum = X * X + Y * Y + Z * Z + W * W;
        if (SquareSum > Tolerance)
        {
            const FRealFixed Scale = URealFixedMath::InvSqrt(SquareSum);
            X *= Scale;
            Y *= Scale;
            Z *= Scale;
            W *= Scale;
        }
        else
        {
            *this = FQuatFixed::Identity;
        }
    }

    FQuatFixed GetNormalized(FRealFixed Tolerance = FRealFixed("0.000001")) const
    {
        FQuatFixed Result(*this);
        Result.Normalize(Tolerance);
        return Result;
    }

    // Combines two quaternions, so that for given quaternions Q1 and Q2, and QR = Q1*Q2, QR's rotation corresponds to applying the rotation of Q1, then the rotation of Q2
    FQuatFixed operator*(const FQuatFixed& Other) const
    {
        // Standard Hamiltonian product
        return FQuatFixed(
            (W * Other.X) + (X * Other.W) + (Y * Other.Z) - (Z * Other.Y),
            (W * Other.Y) - (X * Other.Z) + (Y * Other.W) + (Z * Other.X),
            (W * Other.Z) + (X * Other.Y) - (Y * Other.X) + (Z * Other.W),
            (W * Other.W) - (X * Other.X) - (Y * Other.Y) - (Z * Other.Z)
        );
    }

    FQuatFixed& operator*=(const FQuatFixed& Other)
    {
        *this = *this * Other;
        return *this;
    }

    bool Equals(const FQuatFixed& Other, FRealFixed Tolerance = FRealFixed("0.000001")) const
    {
        // Check for component-wise equality, and also for negative equality (q == -q)
        return (URealFixedMath::Abs(X - Other.X) <= Tolerance && URealFixedMath::Abs(Y - Other.Y) <= Tolerance && URealFixedMath::Abs(Z - Other.Z) <= Tolerance && URealFixedMath::Abs(W - Other.W) <= Tolerance)
            || (URealFixedMath::Abs(X + Other.X) <= Tolerance && URealFixedMath::Abs(Y + Other.Y) <= Tolerance && URealFixedMath::Abs(Z + Other.Z) <= Tolerance && URealFixedMath::Abs(W + Other.W) <= Tolerance);
    }
    
    bool operator==(const FQuatFixed& Other) const
    {
        return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
    }

    bool operator!=(const FQuatFixed& Other) const
    {
        return !(*this == Other);
    }

    FQuatFixed Inverse() const
    {
        // For a unit quaternion, the inverse is the conjugate.
        return FQuatFixed(-X, -Y, -Z, W);
    }

	// Converts this to a string.
    FString ToString() const
    {
        return FString::Printf(TEXT("(X=%s, Y=%s, Z=%s, W=%s)"), *X.ToString(), *Y.ToString(), *Z.ToString(), *W.ToString());
    }

    // Converts this to a standard FQuat
    FQuat ToFQuat() const
    {
        return FQuat(X.ToFloat(), Y.ToFloat(), Z.ToFloat(), W.ToFloat());
    }
	
    // Gets the axis and angle in degrees of this quaternion.
    void ToAxisAndAngle(FVectorFixed& OutAxis, FRealFixed& OutAngleDeg) const
    {
        // Clamp W to avoid domain errors with Acos
        const FRealFixed ClampedW = URealFixedMath::Clamp(W, FRealFixed(-1), FRealFixed(1));
        OutAngleDeg = FRealFixed(2) * URealFixedMath::AcosDeg(ClampedW);
        
        const FRealFixed S = URealFixedMath::Sqrt(FRealFixed(1) - (ClampedW * ClampedW));

        if (S > FRealFixed("0.0001"))
        {
            OutAxis = FVectorFixed(X / S, Y / S, Z / S);
        }
        else
        {
            // If S is zero, the angle is 0 or 360, axis is irrelevant.
            OutAxis = FVectorFixed(1, 0, 0);
        }
    }
};


/**
 * Blueprints math library for QuatFixed
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UQuatFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
// Basic conversions
public:

    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "FQuat to QuatFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FQuatFixed ConvFQuatToQuatFixed(const FQuat& Quat);

    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "QuatFixed to FQuat", CompactNodeTitle = "->", BlueprintAutocast))
    static FQuat ConvQuatFixedToFQuat(const FQuatFixed& Quat);
    
    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Make Quat from Axis and Angle", Keywords = "construct build quaternion", CompactNodeTitle = "Make"))
    static FQuatFixed MakeQuatFromAxisAngle(const FVectorFixed& Axis, const FRealFixed& AngleDeg);

    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Break Quat into Axis and Angle", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakQuatToAxisAngle(const FQuatFixed& Quat, FVectorFixed& Axis, FRealFixed& AngleDeg);

// Basic QuatFixed math
public:
    
    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta=(DisplayName = "Inverse", CompactNodeTitle="Inv"))
    static FQuatFixed Inverse(const FQuatFixed& Quat);

    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Equals (QuatFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool QuatEqualsQuat(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance);
    
	UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Not Equal (QuatFixed)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
    static bool QuatNotEqualsQuat(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance);
    
    UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Combine Quaternions", CompactNodeTitle = "*", Keywords = "multiply combine"))
    static FQuatFixed Multiply(const FQuatFixed& A, const FQuatFixed& B);

	UFUNCTION(BlueprintPure, category = "Math|QuatFixed", meta = (DisplayName = "Slerp (QuatFixed)", Keywords = "interpolate interpolation", CompactNodeTitle = "Slerp"))
    static FQuatFixed Slerp(FQuatFixed A, FQuatFixed B, const FRealFixed& Alpha);

};