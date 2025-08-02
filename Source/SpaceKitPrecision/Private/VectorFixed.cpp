// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0

#include "SpaceKitPrecision/Public/VectorFixed.h"

FVectorFixed FVectorFixed::Identity = FVectorFixed();
FVectorFixed FVectorFixed::ZeroVector = FVectorFixed(0, 0, 0);
FVectorFixed FVectorFixed::VectorUp = FVectorFixed(0, 0, 1);
FVectorFixed FVectorFixed::VectorDown = FVectorFixed(0, 0, -1);
FVectorFixed FVectorFixed::VectorLeft = FVectorFixed(0, -1, 0);
FVectorFixed FVectorFixed::VectorRight = FVectorFixed(0, 1, 0);
FVectorFixed FVectorFixed::VectorOne = FVectorFixed(1, 1, 1);

FVectorFixed UVectorFixedMath::ConvFVectorToVectorFixed(const FVector& InVec)
{
	return FVectorFixed(InVec);
}

FVector UVectorFixedMath::ConvVectorFixedToFVector(const FVectorFixed& InVec)
{
	return InVec.ToFVector();
}

FVectorFixed UVectorFixedMath::VecPlusVec(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First + Second;
}

FVectorFixed UVectorFixedMath::VecMinusVec(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First - Second;
}

FVectorFixed UVectorFixedMath::VecMultReal(const FVectorFixed& First, const FRealFixed& Second)
{
	return First * Second;
}

FVectorFixed UVectorFixedMath::VecMultVec(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First * Second;
}

FVectorFixed UVectorFixedMath::VecDivReal(const FVectorFixed& First, const FRealFixed& Second)
{
	return First / Second;
}

FVectorFixed UVectorFixedMath::VecDivVec(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First / Second;
}

bool UVectorFixedMath::VecEqualsVec(const FVectorFixed& First, const FVectorFixed& Second, const FRealFixed& Tolerance)
{
	return First.Equals(Second, Tolerance);
}

bool UVectorFixedMath::VecNotEqualsVec(const FVectorFixed& First, const FVectorFixed& Second, const FRealFixed& Tolerance)
{
	return !UVectorFixedMath::VecEqualsVec(First, Second, Tolerance);
}

FRealFixed UVectorFixedMath::DotProduct(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First | Second;
}

FVectorFixed UVectorFixedMath::CrossProduct(const FVectorFixed& First, const FVectorFixed& Second)
{
	return First ^ Second;
}

FVectorFixed UVectorFixedMath::ProjectOnTo(const FVectorFixed& B, const FVectorFixed& A)
{
	return (B * ((A | B) / (B | B)));
}
