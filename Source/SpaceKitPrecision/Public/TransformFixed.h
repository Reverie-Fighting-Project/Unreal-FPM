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
    FVectorFixed Location;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed", meta=(ShowOnlyInnerProperties))
    FRotatorFixed Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TransformFixed")
    FVectorFixed Scale;

    static FTransformFixed Identity;

    FTransformFixed()
        : Location(FRealFixed(0), FRealFixed(0), FRealFixed(0))
        , Rotation(FRealFixed(0), FRealFixed(0), FRealFixed(0))
        , Scale(FRealFixed(1), FRealFixed(1), FRealFixed(1))
    {
    }

    FTransformFixed(const FRotatorFixed& InRotation, const FVectorFixed& InLocation, const FVectorFixed& InScale)
        : Location(InLocation)
        , Rotation(InRotation)
        , Scale(InScale)
    {
    }

    FTransformFixed(const FRotatorFixed& InRotation, const FVectorFixed& InLocation)
        : Location(InLocation)
        , Rotation(InRotation)
        , Scale(FRealFixed(1), FRealFixed(1), FRealFixed(1))
    {
    }

    explicit FTransformFixed(const FVectorFixed& InLocation)
        : Location(InLocation)
        , Rotation(FRealFixed(0), FRealFixed(0), FRealFixed(0))
        , Scale(FRealFixed(1), FRealFixed(1), FRealFixed(1))
    {
    }

    explicit FTransformFixed(const FTransform& Transform)
        : Location(Transform.GetTranslation())
        , Rotation(Transform.GetRotation().Rotator())
        , Scale(Transform.GetScale3D())
    {
    }

    FTransform ToFTransform() const
    {
        return FTransform(Rotation.ToFRotator(), Location.ToFVector(), Scale.ToFVector());
    }

    FVectorFixed TransformPosition(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V * Scale) + Location;
    }

    FVectorFixed TransformPositionNoScale(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V) + Location;
    }

    FVectorFixed TransformVector(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V * Scale);
    }

    FVectorFixed TransformVectorNoScale(const FVectorFixed& V) const
    {
        return Rotation.RotateVector(V);
    }

    FVectorFixed InverseTransformPosition(const FVectorFixed& V) const
    {
        const FVectorFixed Translated = V - Location;
        const FVectorFixed Rotated = Rotation.UnrotateVector(Translated);
        return Rotated / Scale;
    }

    FVectorFixed InverseTransformPositionNoScale(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V - Location);
    }

    FVectorFixed InverseTransformVector(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V) / Scale;
    }

    FVectorFixed InverseTransformVectorNoScale(const FVectorFixed& V) const
    {
        return Rotation.UnrotateVector(V);
    }

    FTransformFixed operator*(const FTransformFixed& Other) const
    {
        FTransformFixed Result;
        
        Result.Rotation = FRotatorFixed(FQuatFixed(Rotation) * FQuatFixed(Other.Rotation));
        
        Result.Scale = Scale * Other.Scale;
        
        Result.Location = TransformPosition(Other.Location);
        
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
        
        const FQuatFixed QuatRot(Rotation);
        Result.Rotation = FRotatorFixed(QuatRot.Inverse());
        
        Result.Scale = FVectorFixed(
            FRealFixed(1) / Scale.X,
            FRealFixed(1) / Scale.Y,
            FRealFixed(1) / Scale.Z
        );
        
        Result.Location = FQuatFixed(Result.Rotation).RotateVector(-Location * Result.Scale);
        
        return Result;
    }

    static FTransformFixed Lerp(const FTransformFixed& A, const FTransformFixed& B, const FRealFixed& Alpha)
    {
        FTransformFixed Result;
        
        Result.Location = A.Location + (B.Location - A.Location) * Alpha;
        
        Result.Rotation = FRotatorFixed(UQuatFixedMath::Slerp(FQuatFixed(A.Rotation), FQuatFixed(B.Rotation), Alpha));
        
        Result.Scale = A.Scale + (B.Scale - A.Scale) * Alpha;
        
        return Result;
    }

    bool Equals(const FTransformFixed& Other, const FRealFixed& Tolerance = FRealFixed("0.0001")) const
    {
        return Location.Equals(Other.Location, Tolerance)
            && Rotation.Equals(Other.Rotation, Tolerance)
            && Scale.Equals(Other.Scale, Tolerance);
    }

    bool operator==(const FTransformFixed& Other) const
    {
        return Location == Other.Location
            && Rotation == Other.Rotation
            && Scale == Other.Scale;
    }

    bool operator!=(const FTransformFixed& Other) const
    {
        return !(*this == Other);
    }

    FRotatorFixed Rotator() const
    {
        return Rotation;
    }

    void SetRotation(const FRotatorFixed& InRotator)
    {
        Rotation = InRotator;
    }

    void SetRotation(const FQuatFixed& InRotation)
    {
        Rotation = FRotatorFixed(InRotation);
    }

    FQuatFixed GetRotation() const
    {
        return FQuatFixed(Rotation);
    }

    FRotatorFixed GetRotator() const
    {
        return Rotation;
    }

    FVectorFixed GetLocation() const
    {
        return Location;
    }

    void SetLocation(const FVectorFixed& InLocation)
    {
        Location = InLocation;
    }

    FVectorFixed GetScale() const
    {
        return Scale;
    }

    void SetScale(const FVectorFixed& InScale)
    {
        Scale = InScale;
    }

    void Accumulate(const FTransformFixed& Other)
    {
        *this = *this * Other;
    }

    void NormalizeRotation()
    {
        FQuatFixed Quat(Rotation);
        Quat.Normalize();
        Rotation = FRotatorFixed(Quat);
    }

    bool IsRotationNormalized() const
    {
        const FQuatFixed Quat(Rotation);
        const FRealFixed SquareSum = Quat.X * Quat.X + Quat.Y * Quat.Y + 
                                      Quat.Z * Quat.Z + Quat.W * Quat.W;
        const FRealFixed One(1);
        const FRealFixed Tolerance("0.01");
        return URealFixedMath::Abs(SquareSum - One) < Tolerance;
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("Location: %s Rotation: %s Scale: %s"), 
            *Location.ToString(), 
            *Rotation.ToString(), 
            *Scale.ToString());
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
    static FTransformFixed MakeTransformFixed(const FVectorFixed& Location, const FRotatorFixed& Rotation, const FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Make TransformFixed from Location", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FTransformFixed MakeTransformFromLocation(const FVectorFixed& Location);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Break TransformFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakTransformFixed(const FTransformFixed& Transform, FVectorFixed& Location, FRotatorFixed& Rotation, FVectorFixed& Scale);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Location", CompactNodeTitle = "Location"))
    static FVectorFixed GetLocation(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Rotation (Quat)", CompactNodeTitle = "Rotation"))
    static FQuatFixed GetRotation(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Rotator", CompactNodeTitle = "Rotator"))
    static FRotatorFixed GetRotator(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Get Scale", CompactNodeTitle = "Scale"))
    static FVectorFixed GetScale(const FTransformFixed& Transform);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Location"))
    static FTransformFixed SetLocation(const FTransformFixed& Transform, const FVectorFixed& Location);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Rotation (Quat)"))
    static FTransformFixed SetRotation(const FTransformFixed& Transform, const FQuatFixed& Rotation);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Rotation (Rotator)"))
    static FTransformFixed SetRotationFromRotator(const FTransformFixed& Transform, const FRotatorFixed& Rotation);

    UFUNCTION(BlueprintPure, Category = "Math|TransformFixed", meta = (DisplayName = "Set Scale"))
    static FTransformFixed SetScale(const FTransformFixed& Transform, const FVectorFixed& Scale);

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
