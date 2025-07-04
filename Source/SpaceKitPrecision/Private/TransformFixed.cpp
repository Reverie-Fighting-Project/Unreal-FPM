// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#include "TransformFixed.h"
#include "SpaceKitPrecision/Public/QuatFixed.h" // Required for robust rotation calculations

// Define static Identity member
const FTransformFixed FTransformFixed::Identity = FTransformFixed();

FTransformFixed::FTransformFixed(const FTransform& InTransform)
    : Translation(InTransform.GetLocation()), Rotation(InTransform.GetRotation().Rotator()), Scale3D(InTransform.GetScale3D())
{
}

FTransform FTransformFixed::ToFTransform() const
{
    return FTransform(Rotation.ToFRotator(), Translation.ToFVector(), Scale3D.ToFVector());
}

FString FTransformFixed::ToString() const
{
    return FString::Printf(TEXT("T:%s R:%s S:%s"), *Translation.ToString(), *Rotation.ToString(), *Scale3D.ToString());
}

bool FTransformFixed::Equals(const FTransformFixed& Other, FRealFixed Tolerance) const
{
    return Translation.Equals(Other.Translation, Tolerance)
        && Rotation.Equals(Other.Rotation, Tolerance)
        && Scale3D.Equals(Other.Scale3D, Tolerance);
}

FTransformFixed FTransformFixed::Inverse() const
{
    // Convert rotator to quaternion for robust inversion
    FQuatFixed RotQuat(Rotation);
    FQuatFixed InvRotQuat = RotQuat.Inverse();
    
    // Inverse scale is 1/Scale
    FVectorFixed InvScale3D = FVectorFixed(FRealFixed(1) / Scale3D.X, FRealFixed(1) / Scale3D.Y, FRealFixed(1) / Scale3D.Z);
    
    // Inverse translation is -Translation, rotated by the inverse rotation and scaled
    FVectorFixed InvTranslation = InvRotQuat.RotateVector(-Translation) * InvScale3D;

    return FTransformFixed(InvTranslation, FRotatorFixed(InvRotQuat), InvScale3D);
}

FVectorFixed FTransformFixed::TransformPosition(const FVectorFixed& V) const
{
    // Scale -> Rotate -> Translate
    FQuatFixed RotQuat(Rotation);
    FVectorFixed Result = RotQuat.RotateVector(V * Scale3D);
    Result += Translation;
    return Result;
}

FVectorFixed FTransformFixed::InverseTransformPosition(const FVectorFixed& V) const
{
    // Inverse Translate -> Inverse Rotate -> Inverse Scale
    FVectorFixed Result = V - Translation;
    FQuatFixed RotQuat(Rotation);
    Result = RotQuat.UnrotateVector(Result);
    if (!Scale3D.IsNearlyZero())
    {
        Result /= Scale3D;
    }
    return Result;
}

FVectorFixed FTransformFixed::TransformVector(const FVectorFixed& V) const
{
    // Scale -> Rotate. No translation for vectors.
    FQuatFixed RotQuat(Rotation);
    return RotQuat.RotateVector(V * Scale3D);
}

FVectorFixed FTransformFixed::InverseTransformVector(const FVectorFixed& V) const
{
    // Inverse Rotate -> Inverse Scale. No translation.
    FQuatFixed RotQuat(Rotation);
    FVectorFixed Result = RotQuat.UnrotateVector(V);
    if (!Scale3D.IsNearlyZero())
    {
        Result /= Scale3D;
    }
    return Result;
}

FTransformFixed FTransformFixed::operator*(const FTransformFixed& Other) const
{
    FQuatFixed AQuat(Rotation);
    FQuatFixed BQuat(Other.Rotation);

    // New rotation is combination of both rotations
    FQuatFixed NewQuat = AQuat * BQuat;
    NewQuat.Normalize();

    // New scale is component-wise multiplication
    FVectorFixed NewScale = Scale3D * Other.Scale3D;

    // New translation is A's translation plus A's rotated & scaled version of B's translation
    FVectorFixed NewTranslation = Translation + AQuat.RotateVector(Other.Translation * Scale3D);

    return FTransformFixed(NewTranslation, FRotatorFixed(NewQuat), NewScale);
}

FTransformFixed& FTransformFixed::operator*=(const FTransformFixed& Other)
{
    *this = *this * Other;
    return *this;
}


// UTransformFixedMath implementation

FTransformFixed UTransformFixedMath::ConvFTransformToTransformFixed(const FTransform& InTransform)
{
    return FTransformFixed(InTransform);
}

FTransform UTransformFixedMath::ConvTransformFixedToFTransform(const FTransformFixed& InTransform)
{
    return InTransform.ToFTransform();
}

FTransformFixed UTransformFixedMath::MakeTransformFixed(const FVectorFixed& Translation, const FRotatorFixed& Rotation, const FVectorFixed& Scale)
{
    return FTransformFixed(Translation, Rotation, Scale);
}

void UTransformFixedMath::BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Translation, FRotatorFixed& Rotation, FVectorFixed& Scale)
{
    Translation = Transform.GetTranslation();
    Rotation = Transform.GetRotation();
    Scale = Transform.GetScale3D();
}

FTransformFixed UTransformFixedMath::Inverse(const FTransformFixed& Transform)
{
    return Transform.Inverse();
}

FTransformFixed UTransformFixedMath::Multiply(const FTransformFixed& A, const FTransformFixed& B)
{
    return A * B;
}

FVectorFixed UTransformFixedMath::TransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.TransformPosition(Position);
}

FVectorFixed UTransformFixedMath::InverseTransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.InverseTransformPosition(Position);
}

FVectorFixed UTransformFixedMath::TransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.TransformVector(Vector);
}

FVectorFixed UTransformFixedMath::InverseTransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.InverseTransformVector(Vector);
}

bool UTransformFixedMath::Equals(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance)
{
    return A.Equals(B, Tolerance);
}