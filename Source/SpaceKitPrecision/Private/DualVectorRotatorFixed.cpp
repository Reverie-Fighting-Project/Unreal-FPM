// Steven | 27 November 2025

#include "SpaceKitPrecision/Public/DualVectorRotatorFixed.h"

FDualVectorRotatorFixed UDualVectorRotatorFixedMath::MakeDualVectorRotatorFixed(const FVectorFixed& VectorA, const FVectorFixed& VectorB, const FRotatorFixed& Rotator)
{
    return FDualVectorRotatorFixed(VectorA, VectorB, Rotator);
}

void UDualVectorRotatorFixedMath::BreakDualVectorRotatorFixed(const FDualVectorRotatorFixed& DualVectorRotator, FVectorFixed& VectorA, FVectorFixed& VectorB, FRotatorFixed& Rotator)
{
    VectorA = DualVectorRotator.VectorA;
    VectorB = DualVectorRotator.VectorB;
    Rotator = DualVectorRotator.Rotator;
}

FVectorFixed UDualVectorRotatorFixedMath::GetVectorA(const FDualVectorRotatorFixed& DualVectorRotator)
{
    return DualVectorRotator.VectorA;
}

FVectorFixed UDualVectorRotatorFixedMath::GetVectorB(const FDualVectorRotatorFixed& DualVectorRotator)
{
    return DualVectorRotator.VectorB;
}

FRotatorFixed UDualVectorRotatorFixedMath::GetRotator(const FDualVectorRotatorFixed& DualVectorRotator)
{
    return DualVectorRotator.Rotator;
}

FDualVectorRotatorFixed UDualVectorRotatorFixedMath::SetVectorA(const FDualVectorRotatorFixed& DualVectorRotator, const FVectorFixed& VectorA)
{
    FDualVectorRotatorFixed Result = DualVectorRotator;
    Result.VectorA = VectorA;
    return Result;
}

FDualVectorRotatorFixed UDualVectorRotatorFixedMath::SetVectorB(const FDualVectorRotatorFixed& DualVectorRotator, const FVectorFixed& VectorB)
{
    FDualVectorRotatorFixed Result = DualVectorRotator;
    Result.VectorB = VectorB;
    return Result;
}

FDualVectorRotatorFixed UDualVectorRotatorFixedMath::SetRotator(const FDualVectorRotatorFixed& DualVectorRotator, const FRotatorFixed& Rotator)
{
    FDualVectorRotatorFixed Result = DualVectorRotator;
    Result.Rotator = Rotator;
    return Result;
}

bool UDualVectorRotatorFixedMath::Equals(const FDualVectorRotatorFixed& A, const FDualVectorRotatorFixed& B, const FRealFixed& Tolerance)
{
    return A.Equals(B, Tolerance);
}

bool UDualVectorRotatorFixedMath::NotEqual(const FDualVectorRotatorFixed& A, const FDualVectorRotatorFixed& B, const FRealFixed& Tolerance)
{
    return !A.Equals(B, Tolerance);
}
