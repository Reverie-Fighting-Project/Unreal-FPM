// Steven | 27 November 2025

#pragma once

#include "SpaceKitPrecision/Public/VectorFixed.h"
#include "SpaceKitPrecision/Public/QuatFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"

#include "TransformFixed.generated.h"

USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FTransformFixed
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FQuatFixed Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Translation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Scale3D;

    static FTransformFixed Identity;

    FTransformFixed()
        : Rotation(FQuatFixed::Identity)
        , Translation(FVectorFixed::ZeroVector)
        , Scale3D(FVectorFixed::VectorOne)
    {
    }

    FTransformFixed(const FQuatFixed& InRotation, const FVectorFixed& InTranslation, const FVectorFixed& InScale3D)
        : Rotation(InRotation)
        , Translation(InTranslation)
        , Scale3D(InScale3D)
    {
    }

    FTransformFixed(const FQuatFixed& InRotation, const FVectorFixed& InTranslation)
        : Rotation(InRotation)
        , Translation(InTranslation)
        , Scale3D(FVectorFixed::VectorOne)
    {
    }

    explicit FTransformFixed(const FVectorFixed& InTranslation)
        : Rotation(FQuatFixed::Identity)
        , Translation(InTranslation)
        , Scale3D(FVectorFixed::VectorOne)
    {
    }

    explicit FTransformFixed(const FTransform& Transform)
        : Rotation(Transform.GetRotation())
        , Translation(Transform.GetTranslation())
        , Scale3D(Transform.GetScale3D())
    {
    }

    FTransform ToFTransform() const
    {
        return FTransform(Rotation.ToFQuat(), Translation.ToFVector(), Scale3D.ToFVector());
    }

    FVectorFixed TransformPosition(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V * Scale3D) + Translation;
    }

    FVectorFixed TransformPositionNoScale(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V) + Translation;
    }

    FVectorFixed TransformVector(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V * Scale3D);
    }

    FVectorFixed TransformVectorNoScale(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V);
    }

    FVectorFixed InverseTransformPosition(const FVectorFixed& V) const
    {
        const FVectorFixed Translated = V - Translation;
        const FVectorFixed Rotated = Rotation.UnrotateVector(Translated);
        return Rotated / Scale3D;
    }

    FVectorFixed InverseTransformPositionNoScale(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V - Translation);
    }

    FVectorFixed InverseTransformVector(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V) / Scale3D;
    }

    FVectorFixed InverseTransformVectorNoScale(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V);
    }

    FTransformFixed operator*(const FTransformFixed& Other) const
    {
        FTransformFixed Result;
        
        Result.Rotation = Rotation * Other.Rotation;
        
        Result.Scale3D = Scale3D * Other.Scale3D;
        
        Result.Translation = TransformPosition(Other.Translation);
        
        return Result;
    }

    FTransformFixed& operator*=(const FTransformFixed& Other)
    {
        *this = *this * Other;
        return *this;
    }

    FTransformFixed Inverse() const
    {
        FTransformFixed Result;
        
        Result.Rotation = Rotation.Inverse();
        
        Result.Scale3D = FVectorFixed(
            FRealFixed(1) / Scale3D.X,
            FRealFixed(1) / Scale3D.Y,
            FRealFixed(1) / Scale3D.Z
        );
        
        Result.Translation = Result.Rotation.RotateVector(-Translation * Result.Scale3D);
        
        return Result;
    }

    static FTransformFixed Lerp(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Alpha)
    {
        FTransformFixed Result;
        
        Result.Translation = A.Translation + (B.Translation - A.Translation) * Alpha;
        
        Result.Rotation = UQuatFixedMath::Slerp(A.Rotation, B.Rotation, Alpha);
        
        Result.Scale3D = A.Scale3D + (B.Scale3D - A.Scale3D) * Alpha;
        
        return Result;
    }

    bool Equals(const FTransformFixed& Other, const FRealFixed& Tolerance = FRealFixed("0.0001")) const
    {
        return Translation.Equals(Other.Translation, Tolerance)
            && Rotation.Equals(Other.Rotation, Tolerance)
            && Scale3D.Equals(Other.Scale3D, Tolerance);
    }

    bool operator==(const FTransformFixed& Other) const
    {
        return Translation == Other.Translation
            && Rotation == Other.Rotation
            && Scale3D == Other.Scale3D;
    }

    bool operator!=(const FTransformFixed& Other) const
    {
        return !(*this == Other);
    }

    FRotatorFixed Rotator() const
    {
        return FRotatorFixed(Rotation);
    }

    void SetRotation(const FRotatorFixed& InRotator)
    {
        Rotation = FQuatFixed(InRotator);
    }

    void SetRotation(const FQuatFixed& InRotation)
    {
        Rotation = InRotation;
    }

    FQuatFixed GetRotation() const
    {
        return Rotation;
    }

    FVectorFixed GetTranslation() const
    {
        return Translation;
    }

    void SetTranslation(const FVectorFixed& InTranslation)
    {
        Translation = InTranslation;
    }

    FVectorFixed GetScale3D() const
    {
        return Scale3D;
    }

    void SetScale3D(const FVectorFixed& InScale3D)
    {
        Scale3D = InScale3D;
    }

    void Accumulate(const FTransformFixed& Other)
    {
        *this = *this * Other;
    }

    void NormalizeRotation()
    {
        Rotation.Normalize();
    }

    bool IsRotationNormalized() const
    {
        const FRealFixed SquareSum = Rotation.X * Rotation.X + Rotation.Y * Rotation.Y + 
                                      Rotation.Z * Rotation.Z + Rotation.W * Rotation.W;
        const FRealFixed One(1);
        const FRealFixed Tolerance("0.01");
        return URealFixedMath::Abs(SquareSum - One) < Tolerance;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Translation: %s Rotation: %s Scale: %s"), 
            *Translation.ToString(), 
            *Rotation.ToString(), 
            *Scale3D.ToString());
    }
};

UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UTransformFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "FTransform to TransformFixed", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransformFixed ConvFTransformToTransformFixed(const FTransform& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "TransformFixed to FTransform", CompactNodeTitle = "->", BlueprintAutocast))
    static FTransform ConvTransformFixedToFTransform(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Make TransformFixed (TRS)", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FTransformFixed MakeTransformFixed(const FVectorFixed& Translation, const FRotatorFixed& Rotation, const FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Make TransformFixed from Location", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FTransformFixed MakeTransformFromLocation(const FVectorFixed& Location);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Break TransformFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Translation, FRotatorFixed& Rotation, FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Translation", CompactNodeTitle = "Translation"))
    static FVectorFixed GetTranslation(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Rotation (Quat)", CompactNodeTitle = "Rotation"))
    static FQuatFixed GetRotation(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Rotator", CompactNodeTitle = "Rotator"))
    static FRotatorFixed GetRotator(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Scale", CompactNodeTitle = "Scale"))
    static FVectorFixed GetScale3D(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Translation"))
    static FTransformFixed SetTranslation(const FTransformFixed& Transform, const FVectorFixed& Translation);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Rotation (Quat)"))
    static FTransformFixed SetRotation(const FTransformFixed& Transform, const FQuatFixed& Rotation);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Rotation (Rotator)"))
    static FTransformFixed SetRotationFromRotator(const FTransformFixed& Transform, const FRotatorFixed& Rotation);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Scale"))
    static FTransformFixed SetScale3D(const FTransformFixed& Transform, const FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Location", CompactNodeTitle = "TransformLoc"))
    static FVectorFixed TransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Location (No Scale)", CompactNodeTitle = "TransformLocNoScale"))
    static FVectorFixed TransformPositionNoScale(const FTransformFixed& Transform, const FVectorFixed& Position);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Direction", CompactNodeTitle = "TransformDir"))
    static FVectorFixed TransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Transform Direction (No Scale)", CompactNodeTitle = "TransformDirNoScale"))
    static FVectorFixed TransformVectorNoScale(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Location", CompactNodeTitle = "InvTransformLoc"))
    static FVectorFixed InverseTransformPosition(const FTransformFixed& Transform, const FVectorFixed& Position);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Location (No Scale)", CompactNodeTitle = "InvTransformLocNoScale"))
    static FVectorFixed InverseTransformPositionNoScale(const FTransformFixed& Transform, const FVectorFixed& Position);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Direction", CompactNodeTitle = "InvTransformDir"))
    static FVectorFixed InverseTransformVector(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform Direction (No Scale)", CompactNodeTitle = "InvTransformDirNoScale"))
    static FVectorFixed InverseTransformVectorNoScale(const FTransformFixed& Transform, const FVectorFixed& Vector);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Compose Transforms", CompactNodeTitle = "*", Keywords = "multiply combine"))
    static FTransformFixed Multiply(const FTransformFixed& A, const FTransformFixed& B);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Inverse Transform", CompactNodeTitle = "Inverse"))
    static FTransformFixed Inverse(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Lerp (Transform)", CompactNodeTitle = "Lerp", Keywords = "interpolate blend"))
    static FTransformFixed Lerp(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Alpha);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Equals (TransformFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool Equals(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Not Equal (TransformFixed)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
    static bool NotEqual(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Tolerance);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Normalize Rotation"))
    static FTransformFixed NormalizeRotation(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Is Rotation Normalized"))
    static bool IsRotationNormalized(const FTransformFixed& Transform);
};
