// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#include "SpaceKitPrecision/Public/QuatFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"

FQuatFixed FQuatFixed::Identity = FQuatFixed();

FQuatFixed::FQuatFixed(const FRotatorFixed& Rotator)
{
    const FRealFixed Half(FRealFixed("0.5"));
    const FRealFixed HalfRadPitch = URealFixedMath::DegreesToRadians(Rotator.Pitch * Half);
    const FRealFixed HalfRadYaw   = URealFixedMath::DegreesToRadians(Rotator.Yaw   * Half);
    const FRealFixed HalfRadRoll  = URealFixedMath::DegreesToRadians(Rotator.Roll  * Half);

    const FRealFixed SinP = URealFixedMath::SinRad(HalfRadPitch);
    const FRealFixed CosP = URealFixedMath::CosRad(HalfRadPitch);
    const FRealFixed SinY = URealFixedMath::SinRad(HalfRadYaw);
    const FRealFixed CosY = URealFixedMath::CosRad(HalfRadYaw);
    const FRealFixed SinR = URealFixedMath::SinRad(HalfRadRoll);
    const FRealFixed CosR = URealFixedMath::CosRad(HalfRadRoll);

	W = CosR * CosP * CosY + SinR * SinP * SinY;
	X = SinR * CosP * CosY - CosR * SinP * SinY;
	Y = CosR * SinP * CosY + SinR * CosP * SinY;
	Z = CosR * CosP * SinY - SinR * SinP * CosY;
}

FQuatFixed::FQuatFixed(const FVectorFixed& Axis, const FRealFixed& AngleDeg)
{
    const FRealFixed HalfAngleRad = URealFixedMath::DegreesToRadians(AngleDeg * FRealFixed("0.5"));
    const FRealFixed Sin = URealFixedMath::SinRad(HalfAngleRad);
    const FRealFixed Cos = URealFixedMath::CosRad(HalfAngleRad);

    X = Sin * Axis.X;
    Y = Sin * Axis.Y;
    Z = Sin * Axis.Z;
    W = Cos;
}

void FQuatFixed::Normalize(const FRealFixed& Tolerance)
{
    const FRealFixed SquareSum = X*X + Y*Y + Z*Z + W*W;
    if (SquareSum > Tolerance)
    {
        const FRealFixed Scale = URealFixedMath::InvSqrt(SquareSum);
        X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
    }
    else
    {
        *this = FQuatFixed::Identity;
    }
}

FQuatFixed FQuatFixed::GetNormalized(const FRealFixed& Tolerance) const
{
    FQuatFixed Result(*this);
    Result.Normalize(Tolerance);
    return Result;
}

FQuatFixed FQuatFixed::operator*(const FQuatFixed& Other) const
{
    // Standard Hamiltonian product
    return FQuatFixed(
        (W * Other.X) + (X * Other.W) + (Y * Other.Z) - (Z * Other.Y),
        (W * Other.Y) - (X * Other.Z) + (Y * Other.W) + (Z * Other.X),
        (W * Other.Z) + (X * Other.Y) - (Y * Other.X) + (Z * Other.W),
        (W * Other.W) - (X * Other.X) - (Y * Other.Y) - (Z * Other.Z)
    );
}

bool FQuatFixed::Equals(const FQuatFixed& Other, const FRealFixed& Tolerance) const
{
    return (URealFixedMath::Abs(X - Other.X) <= Tolerance && URealFixedMath::Abs(Y - Other.Y) <= Tolerance && URealFixedMath::Abs(Z - Other.Z) <= Tolerance && URealFixedMath::Abs(W - Other.W) <= Tolerance)
        || (URealFixedMath::Abs(X + Other.X) <= Tolerance && URealFixedMath::Abs(Y + Other.Y) <= Tolerance && URealFixedMath::Abs(Z + Other.Z) <= Tolerance && URealFixedMath::Abs(W + Other.W) <= Tolerance);
}

void FQuatFixed::ToAxisAndAngle(FVectorFixed& OutAxis, FRealFixed& OutAngleDeg) const
{
    const FRealFixed ClampedW = URealFixedMath::Clamp(W, FRealFixed(-1), FRealFixed(1));
    OutAngleDeg = FRealFixed(2) * URealFixedMath::AcosDeg(ClampedW);
    
    const FRealFixed S = URealFixedMath::Sqrt(FRealFixed(1) - (ClampedW * ClampedW));

    if (S > FRealFixed("0.0001"))
    {
        OutAxis = FVectorFixed(X / S, Y / S, Z / S);
    }
    else
    {
        OutAxis = FVectorFixed(FRealFixed(1), FRealFixed(0), FRealFixed(0)); // Default axis for zero rotation
    }
}

// Blueprint Function Library Implementations

FQuatFixed UQuatFixedMath::ConvFQuatToQuatFixed(const FQuat& Quat)
{
    return FQuatFixed(Quat);
}

FQuat UQuatFixedMath::ConvQuatFixedToFQuat(const FQuatFixed& Quat)
{
    return Quat.ToFQuat();
}

FQuatFixed UQuatFixedMath::MakeQuatFromAxisAngle(const FVectorFixed& Axis, const FRealFixed& AngleDeg)
{
    return FQuatFixed(Axis, AngleDeg); // Ensure axis is normalized
}

void UQuatFixedMath::BreakQuatToAxisAngle(const FQuatFixed& Quat, FVectorFixed& Axis, FRealFixed& AngleDeg)
{
    Quat.ToAxisAndAngle(Axis, AngleDeg);
}

FQuatFixed UQuatFixedMath::Inverse(const FQuatFixed& Quat)
{
    return Quat.Inverse();
}

FQuatFixed UQuatFixedMath::Multiply(const FQuatFixed& A, const FQuatFixed& B)
{
    return A * B;
}

bool UQuatFixedMath::Equals(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance)
{
    return A.Equals(B, Tolerance);
}

bool UQuatFixedMath::NotEqual(const FQuatFixed& A, const FQuatFixed& B, const FRealFixed& Tolerance)
{
    return !A.Equals(B, Tolerance);
}

FQuatFixed UQuatFixedMath::Slerp(FQuatFixed A, FQuatFixed B, const FRealFixed& Alpha)
{
    const FRealFixed Zero(0);
    const FRealFixed One(1);

    // Ensure we take the shortest path
    FRealFixed Dot = (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z) + (A.W * B.W);
    if (Dot < Zero) 
    {
        B = FQuatFixed(-B.X, -B.Y, -B.Z, -B.W);
        Dot = -Dot;
    }

    // If the inputs are too close, lerp and re-normalize
    if (Dot > FRealFixed("0.9995"))
    {
        FQuatFixed Result(
            A.X + Alpha * (B.X - A.X),
            A.Y + Alpha * (B.Y - A.Y),
            A.Z + Alpha * (B.Z - A.Z),
            A.W + Alpha * (B.W - A.W)
        );
        Result.Normalize();
        return Result;
    }

    // Standard slerp
    const FRealFixed Theta_0 = URealFixedMath::AcosRad(Dot);
    const FRealFixed Theta = Theta_0 * Alpha;
    const FRealFixed Sin_Theta = URealFixedMath::SinRad(Theta);
    const FRealFixed Sin_Theta_0 = URealFixedMath::SinRad(Theta_0);
    const FRealFixed Cos_Theta = URealFixedMath::CosRad(Theta);

    const FRealFixed S0 = Cos_Theta - Dot * Sin_Theta / Sin_Theta_0;
    const FRealFixed S1 = Sin_Theta / Sin_Theta_0;

    return FQuatFixed(
        (S0 * A.X) + (S1 * B.X),
        (S0 * A.Y) + (S1 * B.Y),
        (S0 * A.Z) + (S1 * B.Z),
        (S0 * A.W) + (S1 * B.W)
    );
}