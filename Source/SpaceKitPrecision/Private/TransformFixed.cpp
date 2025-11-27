// Steven | 27 November 2025

#include "SpaceKitPrecision/Public/TransformFixed.h"

FTransformFixed FTransformFixed::Identity = FTransformFixed(
    FQuatFixed(FRealFixed(0), FRealFixed(0), FRealFixed(0), FRealFixed(1)),
    FVectorFixed(FRealFixed(0), FRealFixed(0), FRealFixed(0)),
    FVectorFixed(FRealFixed(1), FRealFixed(1), FRealFixed(1))
);

FTransformFixed UTransformFixedMath::ConvFTransformToTransformFixed(const FTransform& Transform)
{
    return FTransformFixed(Transform);
}

FTransform UTransformFixedMath::ConvTransformFixedToFTransform(const FTransformFixed& Transform)
{
    return Transform.ToFTransform();
}

FTransformFixed UTransformFixedMath::MakeTransformFixed(const FVectorFixed& Translation, const FRotatorFixed& Rotation, const FVectorFixed& Scale)
{
    return FTransformFixed(FQuatFixed(Rotation), Translation, Scale);
}

FTransformFixed UTransformFixedMath::MakeTransformFromLocation(const FVectorFixed& Location)
{
    return FTransformFixed(Location);
}

void UTransformFixedMath::BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Translation, FRotatorFixed& Rotation, FVectorFixed& Scale)
{
    Translation = Transform.Translation;
    Rotation = FRotatorFixed(Transform.Rotation);
    Scale = Transform.Scale3D;
}

FVectorFixed UTransformFixedMath::GetTranslation(const FTransformFixed& Transform)
{
    return Transform.GetTranslation();
}

FQuatFixed UTransformFixedMath::GetRotation(const FTransformFixed& Transform)
{
    return Transform.GetRotation();
}

FRotatorFixed UTransformFixedMath::GetRotator(const FTransformFixed& Transform)
{
    return Transform.Rotator();
}

FVectorFixed UTransformFixedMath::GetScale3D(const FTransformFixed& Transform)
{
    return Transform.GetScale3D();
}

FTransformFixed UTransformFixedMath::SetTranslation(const FTransformFixed& Transform, const FVectorFixed& Translation)
{
    FTransformFixed Result = Transform;
    Result.SetTranslation(Translation);
    return Result;
}

FTransformFixed UTransformFixedMath::SetRotation(const FTransformFixed& Transform, const FQuatFixed& Rotation)
{
    FTransformFixed Result = Transform;
    Result.SetRotation(Rotation);
    return Result;
}

FTransformFixed UTransformFixedMath::SetRotationFromRotator(const FTransformFixed& Transform, const FRotatorFixed& Rotation)
{
    FTransformFixed Result = Transform;
    Result.SetRotation(Rotation);
    return Result;
}

FTransformFixed UTransformFixedMath::SetScale3D(const FTransformFixed& Transform, const FVectorFixed& Scale)
{
    FTransformFixed Result = Transform;
    Result.SetScale3D(Scale);
    return Result;
}

FVectorFixed UTransformFixedMath::TransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.TransformPosition(Position);
}

FVectorFixed UTransformFixedMath::TransformPositionNoScale(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.TransformPositionNoScale(Position);
}

FVectorFixed UTransformFixedMath::TransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.TransformVector(Vector);
}

FVectorFixed UTransformFixedMath::TransformVectorNoScale(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.TransformVectorNoScale(Vector);
}

FVectorFixed UTransformFixedMath::InverseTransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.InverseTransformPosition(Position);
}

FVectorFixed UTransformFixedMath::InverseTransformPositionNoScale(const FTransformFixed& Transform, const FVectorFixed& Position)
{
    return Transform.InverseTransformPositionNoScale(Position);
}

FVectorFixed UTransformFixedMath::InverseTransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.InverseTransformVector(Vector);
}

FVectorFixed UTransformFixedMath::InverseTransformVectorNoScale(const FTransformFixed& Transform, const FVectorFixed& Vector)
{
    return Transform.InverseTransformVectorNoScale(Vector);
}

FTransformFixed UTransformFixedMath::Multiply(const FTransformFixed& A, const FTransformFixed& B)
{
    return A * B;
}

FTransformFixed UTransformFixedMath::Inverse(const FTransformFixed& Transform)
{
    return Transform.Inverse();
}

FTransformFixed UTransformFixedMath::Lerp(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Alpha)
{
    return FTransformFixed::Lerp(A, B, Alpha);
}

bool UTransformFixedMath::Equals(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance)
{
    return A.Equals(B, Tolerance);
}

bool UTransformFixedMath::NotEqual(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance)
{
    return !A.Equals(B, Tolerance);
}

FTransformFixed UTransformFixedMath::NormalizeRotation(const FTransformFixed& Transform)
{
    FTransformFixed Result = Transform;
    Result.NormalizeRotation();
    return Result;
}

bool UTransformFixedMath::IsRotationNormalized(const FTransformFixed& Transform)
{
    return Transform.IsRotationNormalized();
}