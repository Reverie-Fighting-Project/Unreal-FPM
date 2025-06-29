// Copyright 2020 Baptiste Hutteau Licensed under the Apache License, Version 2.0
// Updated by AI Studio for deterministic fixed-point calculations.

#pragma once

#include "SpaceKitPrecision/Public/VectorFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"
#include "SpaceKitPrecision/Public/QuatFixed.h" // Required for correct transform combination

#include "TransformFixed.generated.h"

/**
 * A transform composed of a translation, a rotation (as a fixed-point rotator), and a 3D scale.
 * This class is designed for deterministic calculations using fixed-point arithmetic.
 */
USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FTransformFixed
{
    GENERATED_BODY()

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FRotatorFixed Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Translation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Scale3D;

public:

    static FTransformFixed Identity;

    /** Default constructor initializes to Identity. */
    FTransformFixed()
        : Rotation(FRotatorFixed::Identity)
        , Translation(FVectorFixed::ZeroVector)
        , Scale3D(FVectorFixed(1, 1, 1))
    {
    }

    /** Constructor with explicit components. */
    FTransformFixed(const FRotatorFixed& InRotation, const FVectorFixed& InTranslation, const FVectorFixed& InScale3D)
        : Rotation(InRotation)
        , Translation(InTranslation)
        , Scale3D(InScale3D)
    {
    }

    /** Constructor with only rotation, translation is zero, scale is 1. */
    explicit FTransformFixed(const FRotatorFixed& InRotation)
        : Rotation(InRotation)
        , Translation(FVectorFixed::ZeroVector)
        , Scale3D(FVectorFixed(1, 1, 1))
    {
    }

    /** Constructor with only translation, rotation is zero, scale is 1. */
    explicit FTransformFixed(const FVectorFixed& InTranslation)
        : Rotation(FRotatorFixed::Identity)
        , Translation(InTranslation)
        , Scale3D(FVectorFixed(1, 1, 1))
    {
    }

    /** Conversion constructor from an FTransform. */
    explicit FTransformFixed(const FTransform& InTransform)
        : Rotation(FRotatorFixed(InTransform.GetRotation().Rotator()))
        , Translation(FVectorFixed(InTransform.GetTranslation()))
        , Scale3D(FVectorFixed(InTransform.GetScale3D()))
    {
    }

    /** Returns the rotation as a fixed-point Quaternion. This is needed for correct combination of transforms. */
    FQuatFixed GetQuat() const
    {
        // Assumes a FQuatFixed(const FRotatorFixed&) constructor exists or will be created.
        return FQuatFixed(Rotation);
    }
    
    /** Returns the inverse of this transform. */
    FTransformFixed Inverse() const
    {
        // The correct way to inverse a transform:
        // 1. Inverse Scale
        // 2. Inverse Rotation
        // 3. The new translation is the original translation, but inverse-rotated and inverse-scaled.
        
        FQuatFixed InvRotationQuat = GetQuat().Inverse();
        FVectorFixed InvScale3D = Scale3D.Reciprocal(); // Assumes a Reciprocal() method (1/v) exists on FVectorFixed
        
        // Un-rotate and un-scale the translation
        FVectorFixed InvTranslation = InvRotationQuat.RotateVector(-Translation) * InvScale3D;

        // Assumes a FRotatorFixed(const FQuatFixed&) constructor exists or will be created.
        return FTransformFixed(FRotatorFixed(InvRotationQuat), InvTranslation, InvScale3D);
    }

    /** Combine two transforms. The result is as if you applied Other, then *this. */
    FTransformFixed operator*(const FTransformFixed& Other) const
    {
        // Note: Combining rotations is best done with Quaternions to avoid gimbal lock and other issues.
        // T_new = T1 * T2
        // R_new = R1 * R2
        // S_new = S1 * S2
        // P_new = T1.Rotate(T2.P * T1.S) + T1.P
        
        FQuatFixed NewRotationQuat = this->GetQuat() * Other.GetQuat();
        FVectorFixed NewScale3D = this->Scale3D * Other.Scale3D;
        FVectorFixed NewTranslation = this->GetQuat().RotateVector(Other.Translation * this->Scale3D) + this->Translation;

        return FTransformFixed(FRotatorFixed(NewRotationQuat), NewTranslation, NewScale3D);
    }

    FTransformFixed& operator*=(const FTransformFixed& Other)
    {
        *this = *this * Other;
        return *this;
    }

    /** Transforms a position. The order is: Scale -> Rotate -> Translate. */
    FVectorFixed TransformPosition(const FVectorFixed& V) const
    {
        return GetQuat().RotateVector(V * Scale3D) + Translation;
    }

    /** Transforms a position, ignoring the scale component. */
    FVectorFixed TransformPositionNoScale(const FVectorFixed& V) const
    {
        return GetQuat().RotateVector(V) + Translation;
    }

    /** Inverse-transforms a position. */
    FVectorFixed InverseTransformPosition(const FVectorFixed& V) const
    {
        return Inverse().TransformPosition(V);
    }

    /** Transforms a direction vector. The order is: Scale -> Rotate. Translation is ignored. */
    FVectorFixed TransformVector(const FVectorFixed& V) const
    {
        return GetQuat().RotateVector(V * Scale3D);
    }

    /** Inverse-transforms a direction vector. */
    FVectorFixed InverseTransformVector(const FVectorFixed& V) const
    {
        return Inverse().TransformVector(V);
    }

    /** Convert back to a standard FTransform. */
    FTransform ToFTransform() const
    {
        return FTransform(Rotation.ToFRotator(), Translation.ToFVector(), Scale3D.ToFVector());
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Translation: %s\nRotation: %s\nScale3D: %s"), *Translation.ToString(), *Rotation.ToString(), *Scale3D.ToString());
    }
    
    // Getters and Setters
    void SetTranslation(const FVectorFixed& NewTranslation) { Translation = NewTranslation; }
    const FVectorFixed& GetTranslation() const { return Translation; }

    void SetRotation(const FRotatorFixed& NewRotation) { Rotation = NewRotation; }
    const FRotatorFixed& GetRotation() const { return Rotation; }

    void SetScale3D(const FVectorFixed& NewScale3D) { Scale3D = NewScale3D; }
    const FVectorFixed& GetScale3D() const { return Scale3D; }

    void SetComponents(const FRotatorFixed& InRotation, const FVectorFixed& InTranslation, const FVectorFixed& InScale3D)
    {
        Rotation = InRotation;
        Translation = InTranslation;
        Scale3D = InScale3D;
    }
};


/**
 * Blueprint function library for FTransformFixed.
 */
UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UTransformFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Make TransformFixed", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FTransformFixed MakeTransformFixed(const FVectorFixed& Translation, const FRotatorFixed& Rotation, const FVectorFixed& Scale3D);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Break TransformFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Translation, FRotatorFixed& Rotation, FVectorFixed& Scale3D);
    
    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "FTransform to TransformFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransformFixed ConvFTransformToTransformFixed(const FTransform& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "TransformFixed to FTransform", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransform ConvTransformFixedToFTransform(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Combine Transforms", CompactNodeTitle = "*", Keywords = "multiply combine"))
    static FTransformFixed Multiply_TransformTransform(const FTransformFixed& A, const FTransformFixed& B);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform", CompactNodeTitle = "Inverse"))
    static FTransformFixed Inverse(const FTransformFixed& Transform);
    
    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Location", Keywords = "transform position"))
    static FVectorFixed TransformLocation(const FTransformFixed& Transform, const FVectorFixed& Location);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Location", Keywords = "inverse untransform position"))
    static FVectorFixed InverseTransformLocation(const FTransformFixed& Transform, const FVectorFixed& Location);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Direction", Keywords = "transform vector"))
    static FVectorFixed TransformDirection(const FTransformFixed& Transform, const FVectorFixed& Direction);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Direction", Keywords = "inverse untransform vector"))
    static FVectorFixed InverseTransformDirection(const FTransformFixed& Transform, const FVectorFixed& Direction);
};