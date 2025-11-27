// Steven | 27 November 2025

#pragma once

#include "SpaceKitPrecision/Public/VectorFixed.h"
#include "SpaceKitPrecision/Public/RotatorFixed.h"

#include "DualVectorRotatorFixed.generated.h"

USTRUCT(BlueprintType)
struct SPACEKITPRECISION_API FDualVectorRotatorFixed
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DualVectorRotatorFixed")
    FVectorFixed VectorA;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DualVectorRotatorFixed")
    FVectorFixed VectorB;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DualVectorRotatorFixed", meta=(ShowOnlyInnerProperties))
    FRotatorFixed Rotator;

    FDualVectorRotatorFixed()
        : VectorA(FRealFixed(0), FRealFixed(0), FRealFixed(0))
        , VectorB(FRealFixed(0), FRealFixed(0), FRealFixed(0))
        , Rotator(FRealFixed(0), FRealFixed(0), FRealFixed(0))
    {
    }

    FDualVectorRotatorFixed(const FVectorFixed& InVectorA, const FVectorFixed& InVectorB, const FRotatorFixed& InRotator)
        : VectorA(InVectorA)
        , VectorB(InVectorB)
        , Rotator(InRotator)
    {
    }

    bool Equals(const FDualVectorRotatorFixed& Other, const FRealFixed& Tolerance = FRealFixed("0.0001")) const
    {
        return VectorA.Equals(Other.VectorA, Tolerance)
            && VectorB.Equals(Other.VectorB, Tolerance)
            && Rotator.Equals(Other.Rotator, Tolerance);
    }

    bool operator==(const FDualVectorRotatorFixed& Other) const
    {
        return VectorA == Other.VectorA
            && VectorB == Other.VectorB
            && Rotator == Other.Rotator;
    }

    bool operator!=(const FDualVectorRotatorFixed& Other) const
    {
        return !(*this == Other);
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("VectorA: %s VectorB: %s Rotator: %s"), 
            *VectorA.ToString(), 
            *VectorB.ToString(), 
            *Rotator.ToString());
    }
};

UCLASS(BlueprintType, Abstract)
class SPACEKITPRECISION_API UDualVectorRotatorFixedMath : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
	
public:

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Make DualVectorRotatorFixed", Keywords = "construct build", CompactNodeTitle = "Make"))
    static FDualVectorRotatorFixed MakeDualVectorRotatorFixed(const FVectorFixed& VectorA, const FVectorFixed& VectorB, const FRotatorFixed& Rotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Break DualVectorRotatorFixed", Keywords = "split", CompactNodeTitle = "Break"))
    static void BreakDualVectorRotatorFixed(const FDualVectorRotatorFixed& DualVectorRotator, FVectorFixed& VectorA, FVectorFixed& VectorB, FRotatorFixed& Rotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Get Vector A", CompactNodeTitle = "VectorA"))
    static FVectorFixed GetVectorA(const FDualVectorRotatorFixed& DualVectorRotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Get Vector B", CompactNodeTitle = "VectorB"))
    static FVectorFixed GetVectorB(const FDualVectorRotatorFixed& DualVectorRotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Get Rotator", CompactNodeTitle = "Rotator"))
    static FRotatorFixed GetRotator(const FDualVectorRotatorFixed& DualVectorRotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Set Vector A"))
    static FDualVectorRotatorFixed SetVectorA(const FDualVectorRotatorFixed& DualVectorRotator, const FVectorFixed& VectorA);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Set Vector B"))
    static FDualVectorRotatorFixed SetVectorB(const FDualVectorRotatorFixed& DualVectorRotator, const FVectorFixed& VectorB);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Set Rotator"))
    static FDualVectorRotatorFixed SetRotator(const FDualVectorRotatorFixed& DualVectorRotator, const FRotatorFixed& Rotator);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Equals (DualVectorRotatorFixed)", CompactNodeTitle = "==", Keywords = "== equal"))
    static bool Equals(const FDualVectorRotatorFixed& A, const FDualVectorRotatorFixed& B, const FRealFixed& Tolerance);

    UFUNCTION(BlueprintPure, Category = "Math|DualVectorRotatorFixed", meta = (DisplayName = "Not Equal (DualVectorRotatorFixed)", CompactNodeTitle = "!=", Keywords = "!= not equal"))
    static bool NotEqual(const FDualVectorRotatorFixed& A, const FDualVectorRotatorFixed& B, const FRealFixed& Tolerance);
};
