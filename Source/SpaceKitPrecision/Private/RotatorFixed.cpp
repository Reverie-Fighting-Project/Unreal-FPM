// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#include "SpaceKitPrecision/Public/RotatorFixed.h"
#include "SpaceKitPrecision/Public/QuatFixed.h"
#include "SpaceKitPrecision/Public/RealFixed.h" // Assuming this is where URealFixedMath resides

// Initialize the static Identity member
FRotatorFixed FRotatorFixed::Identity = FRotatorFixed();

FRotatorFixed::FRotatorFixed(const FQuatFixed& InQuat)
{
    // Standard conversion from Quaternion to Euler angles (Pitch, Yaw, Roll)
    const FRealFixed Two(2);
    const FRealFixed One(1);
    const FRealFixed Ninety(90);

    // Roll (X-axis rotation)
    const FRealFixed sinr_cosp = Two * (InQuat.W * InQuat.X + InQuat.Y * InQuat.Z);
    const FRealFixed cosr_cosp = One - Two * (InQuat.X * InQuat.X + InQuat.Y * InQuat.Y);
    Roll = URealFixedMath::Atan2Deg(sinr_cosp, cosr_cosp);

    // Pitch (Y-axis rotation)
    const FRealFixed sinp = Two * (InQuat.W * InQuat.Y - InQuat.Z * InQuat.X);
    if (URealFixedMath::Abs(sinp) >= One)
    {
        Pitch = URealFixedMath::Sign(sinp) * Ninety; // Use 90 degrees if out of range (gimbal lock)
    }
    else
    {
        Pitch = URealFixedMath::AsinDeg(sinp);
    }

    // Yaw (Z-axis rotation)
    const FRealFixed siny_cosp = Two * (InQuat.W * InQuat.Z + InQuat.X * InQuat.Y);
    const FRealFixed cosy_cosp = One - Two * (InQuat.Y * InQuat.Y + InQuat.Z * InQuat.Z);
    Yaw = URealFixedMath::Atan2Deg(siny_cosp, cosy_cosp);
}

FVectorFixed FRotatorFixed::RotateVector(const FVectorFixed& Vec) const
{
    return FQuatFixed(*this).RotateVector(Vec);
}

FVectorFixed FRotatorFixed::UnrotateVector(const FVectorFixed& Vec) const
{
    return FQuatFixed(*this).UnrotateVector(Vec);
}

// Blueprint Function Library Implementations

FRotatorFixed URotatorFixedMath::ConvFRotatorToRotatorFixed(const FRotator& InRot)
{
	return FRotatorFixed(InRot);
}

FRotator URotatorFixedMath::ConvRotatorFixedToFRotator(const FRotatorFixed& InRot)
{
	return InRot.ToFRotator();
}

FRotatorFixed URotatorFixedMath::MakeRotatorFixed(const FRealFixed& Pitch, const FRealFixed& Yaw, const FRealFixed& Roll)
{
	return FRotatorFixed(Pitch, Yaw, Roll);
}

void URotatorFixedMath::BreakRotatorFixed(const FRotatorFixed& Rot, FRealFixed& Pitch, FRealFixed& Yaw, FRealFixed& Roll)
{
	Pitch = Rot.Pitch;
	Yaw = Rot.Yaw;
	Roll = Rot.Roll;
}

FRotatorFixed URotatorFixedMath::Add(const FRotatorFixed& A, const FRotatorFixed& B)
{
	return A + B;
}

FRotatorFixed URotatorFixedMath::Subtract(const FRotatorFixed& A, const FRotatorFixed& B)
{
	return A - B;
}

FRotatorFixed URotatorFixedMath::MultiplyByReal(const FRotatorFixed& A, const FRealFixed& B)
{
	return A * B;
}

FRotatorFixed URotatorFixedMath::MultiplyByRotator(const FRotatorFixed& A, const FRotatorFixed& B)
{
    // Note: This is a component-wise multiplication, not a standard rotation combination.
	return A * B;
}

FRotatorFixed URotatorFixedMath::DivideByReal(const FRotatorFixed& A, const FRealFixed& B)
{
	return A / B;
}

bool URotatorFixedMath::Equals(const FRotatorFixed& A, const FRotatorFixed& B, const FRealFixed& Tolerance)
{
	return A.Equals(B, Tolerance);
}

bool URotatorFixedMath::NotEqual(const FRotatorFixed& A, const FRotatorFixed& B, const FRealFixed& Tolerance)
{
	return !A.Equals(B, Tolerance);
}