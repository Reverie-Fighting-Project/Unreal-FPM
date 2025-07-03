// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#pragma once

#include "CoreMinimal.h"
#include "SpaceKitPrecision/Public/RealFixed.h"
#include "SpaceKitPrecision/Public/VectorFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"

#include "TransformFixed.generated.h"

/**
 * A transform composed of a translation, rotation (as a rotator), and scale,
 * using deterministic fixed-point mathematics.
 *
 * The order of operations for transforming a point P is: Scale -> Rotate -> Translate.
 * P' = Translation + (Rotation * (P * Scale3D))
 */
USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FTransformFixed
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Translation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FRotatorFixed Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Scale3D;

public:

    static const FTransformFixed Identity;

    /** Default constructor initializes to identity transform. */
    FTransformFixed()
        : Translation(FVectorFixed::ZeroVector), Rotation(FRotatorFixed::Identity), Scale3D(FVectorFixed(1, 1, 1))
    {
    }

    /** Constructor with explicit components. */
    FTransformFixed(const FVectorFixed& InTranslation, const FRotatorFixed& InRotation, const FVectorFixed& InScale3D)
        : Translation(InTranslation), Rotation(InRotation), Scale3D(InScale3D)
    {
    }

    /** Constructor to convert from a standard FTransform. */
    explicit FTransformFixed(const FTransform& InTransform);
    
// Accessors
public:

    FVectorFixed GetTranslation() const { return Translation; }
    void SetTranslation(const FVectorFixed& InTranslation) { Translation = InTranslation; }

    FRotatorFixed GetRotation() const { return Rotation; }
    void SetRotation(const FRotatorFixed& InRotation) { Rotation = InRotation; }

    FVectorFixed GetScale3D() const { return Scale3D; }
    void SetScale3D(const FVectorFixed& InScale) { Scale3D = InScale; }

// Operations
public:

    /** Returns the inverse of this transform. */
    FTransformFixed Inverse() const;

    /**
     * Transforms a position from local space to world space.
     * This includes scaling, rotating, and translating the position.
     */
    FVectorFixed TransformPosition(const FVectorFixed& V) const;

    /**
     * Transforms a position from world space to local space.
     * This is the inverse of TransformPosition.
     */
    FVectorFixed InverseTransformPosition(const FVectorFixed& V) const;
    
    /**
     * Transforms a direction vector from local space to world space.
     * This applies scaling and rotation, but NOT translation.
     */
    FVectorFixed TransformVector(const FVectorFixed& V) const;

    /**
     * Transforms a direction vector from world space to local space.
     * This is the inverse of TransformVector.
     */
    FVectorFixed InverseTransformVector(const FVectorFixed& V) const;

    /** Combines this transform with another. A * B means B is applied first, then A. */
    FTransformFixed operator*(const FTransformFixed& Other) const;
    FTransformFixed& operator*=(const FTransformFixed& Other);
    
// Conversions and Utility
public:

    /** Converts this fixed-point transform to a standard FTransform. */
    FTransform ToFTransform() const;

    /** Checks for component-wise equality within a tolerance. */
    bool Equals(const FTransformFixed& Other, FRealFixed Tolerance = FRealFixed("0.000001")) const;

    FString ToString() const;
};

/**
 * Blueprints math library for TransformFixed
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UTransformFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

// Conversions & Construction
public:

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "FTransform to TransformFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransformFixed ConvFTransformToTransformFixed(const FTransform& InTransform);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "TransformFixed to FTransform", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransform ConvTransformFixedToFTransform(const FTransformFixed& InTransform);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Make TransformFixed", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FTransformFixed MakeTransformFixed(const FVectorFixed& Translation, const FRotatorFixed& Rotation, const FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Break TransformFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Translation, FRotatorFixed& Rotation, FVectorFixed& Scale);

// Operations
public:

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Inverse TransformFixed", CompactNodeTitle = "Inverse"))
    static FTransformFixed Inverse(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Combine Transforms", CompactNodeTitle = "*", Keywords = "multiply combine"))
    static FTransformFixed Multiply(const FTransformFixed& A, const FTransformFixed& B);
    
    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Transform Position"))
    static FVectorFixed TransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Position"))
    static FVectorFixed InverseTransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position);
    
    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Transform Vector (Direction)"))
    static FVectorFixed TransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Vector (Direction)"))
    static FVectorFixed InverseTransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, category = "Math|TransformFixed", meta = (DisplayName = "Equals (TransformFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool Equals(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance);
};