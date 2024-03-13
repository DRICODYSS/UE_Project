// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Character/TPSCharacterEnumLibrary.h"

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/TPSCharacterStructLibrary.h"

#include "TPSMathLibrary.generated.h"

class UCapsuleComponent;

/**
 * Math library functions for ALS
 */
UCLASS()
class TPS_PETPROJECT_API UTPSMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FTransform MantleComponentLocalToWorld(const FTPSComponentAndTransform &CompAndTransform);

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FTransform TransformSub(const FTransform &T1, const FTransform &T2)
	{
		return FTransform(T1.GetRotation().Rotator() - T2.GetRotation().Rotator(), T1.GetLocation() - T2.GetLocation(),
						  T1.GetScale3D() - T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FTransform TransformAdd(const FTransform &T1, const FTransform &T2)
	{
		return FTransform(T1.GetRotation().Rotator() + T2.GetRotation().Rotator(), T1.GetLocation() + T2.GetLocation(),
						  T1.GetScale3D() + T2.GetScale3D());
	}

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FVector GetCapsuleBaseLocation(float ZOffset, UCapsuleComponent *Capsule);

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static FVector GetCapsuleLocationFromBase(FVector BaseLocation, float ZOffset, UCapsuleComponent *Capsule);

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static bool CapsuleHasRoomCheck(UCapsuleComponent *Capsule, FVector TargetLocation, float HeightOffset,
									float RadiusOffset, EDrawDebugTrace::Type DebugType = EDrawDebugTrace::Type::None,
									bool DrawDebugTrace = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static bool AngleInRange(float Angle, float MinAngle, float MaxAngle, float Buffer, bool IncreaseBuffer);

	UFUNCTION(BlueprintCallable, Category = "ALS|Math Utils")
	static ETPSMovementDirection CalculateQuadrant(ETPSMovementDirection Current, float FRThreshold, float FLThreshold,
												   float BRThreshold, float BLThreshold, float Buffer, float Angle);
};