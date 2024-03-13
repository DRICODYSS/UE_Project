// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/MovementParameters/MoveLogic_CustomJump.h"

#include "Curves/CurveVector.h"
#include "Library/Movement/TPSMovementStructLibrary.h"


FVector MoveLogic_WallRun::GetOwnerLocationByDeltaTime(const FCJumpMovementParameters &CJumpMovementParameters, const FRotator& ActorRotation, bool bIsLeftSide, float ElapsedTime)
{
	FVector TargetVector = CJumpMovementParameters.AnimMoveCurve->GetVectorValue(ElapsedTime);
	
	float Length1 = FMath::Abs(TargetVector.X);
	float Angle1  = ActorRotation.Yaw;
		
	float Length2 = -FMath::Abs(TargetVector.Y);
	float Angle2  = Angle1 + (bIsLeftSide? -90.f : 90.f); 
	
	Angle1 = FMath::DegreesToRadians(Angle1);
	Angle2 = FMath::DegreesToRadians(Angle2);
	
	FVector2D Vector1   = FVector2D( Length1 * FMath::Cos(Angle1),  Length1 * FMath::Sin(Angle1));
	FVector2D Vector2   = FVector2D(  Length2 * FMath::Cos(Angle2),  Length2 * FMath::Sin(Angle2));
	FVector2D VectorSum = Vector1 + Vector2;
		
	return  CJumpMovementParameters.TargetInitLocation + FVector(VectorSum.X, VectorSum.Y,  TargetVector.Z);
}
