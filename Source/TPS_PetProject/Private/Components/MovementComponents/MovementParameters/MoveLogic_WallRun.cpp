// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/MovementParameters/MoveLogic_WallRun.h"

#include "Curves/CurveVector.h"
#include "Library/Movement/TPSMovementStructLibrary.h"


FVector MoveLogic_WallRun::GetInitOwnerLocationByElapsedTime(const FWallRunningParameters& WallRunMovementParameters, float ElapsedTime)
{
	FVector WallRunningStartCurveValue = WallRunMovementParameters.InitCurve->GetVectorValue(ElapsedTime);
	
	float XCorrectionAlpha = WallRunningStartCurveValue.X;
	float YCorrectionAlpha = WallRunningStartCurveValue.Y;
	float ZCorrectionAlpha = WallRunningStartCurveValue.Z;
		
	return FMath::Lerp(WallRunMovementParameters.InitLocation, WallRunMovementParameters.TargetInitLocation, YCorrectionAlpha);
}

FVector MoveLogic_WallRun::GetInitStartOwnerLocationByElapsedTime(const FWallRunningParameters &WallRunMovementParameters, const FRotator& ActorRotation, float ElapsedTime)
{
	FVector TargetVector = WallRunMovementParameters.WallRunStartCurve->GetVectorValue(ElapsedTime);
	
	float Length1 = FMath::Abs(TargetVector.Y);
	float Angle1  = ActorRotation.Yaw;
		
	float Length2 = FMath::Abs(TargetVector.X);
	float Angle2  = Angle1 + (WallRunMovementParameters.bIsLeftSide? -90.f : 90.f); 
	
	Angle1 = FMath::DegreesToRadians(Angle1);
	Angle2 = FMath::DegreesToRadians(Angle2);
	
	FVector2D Vector1   = FVector2D( Length1 * FMath::Cos(Angle1),  Length1 * FMath::Sin(Angle1));
	FVector2D Vector2   = FVector2D(  Length2 * FMath::Cos(Angle2),  Length2 * FMath::Sin(Angle2));
	FVector2D VectorSum = Vector1 + Vector2;
		
	return  WallRunMovementParameters.TargetInitLocation + FVector(VectorSum.X, VectorSum.Y,  TargetVector.Z);
}

FVector MoveLogic_WallRun::GetInitStopOwnerLocationByElapsedTime(const FWallRunningParameters& WallRunMovementParameters, const FRotator& ActorRotation, float ElapsedTime)
{
	FVector TargetVector = WallRunMovementParameters.WallRunStopCurve->GetVectorValue(ElapsedTime);
		
	float Length1 = FMath::Abs(TargetVector.Y);
	float Angle1 = ActorRotation.Yaw;
	
	float Length2 = -TargetVector.X;
	float Angle2 = Angle1 + (WallRunMovementParameters.bIsLeftSide? -90.f : 90.f); 
	
	Angle1 = FMath::DegreesToRadians(Angle1);
	Angle2 = FMath::DegreesToRadians(Angle2);
	
	FVector2D Vector1   = FVector2D( Length1 * FMath::Cos(Angle1),  Length1 * FMath::Sin(Angle1));
	FVector2D Vector2   = FVector2D(  Length2 * FMath::Cos(Angle2),  Length2 * FMath::Sin(Angle2));
	FVector2D VectorSum = Vector1 + Vector2;
		
	return  WallRunMovementParameters.InitLocation + FVector(VectorSum.X, VectorSum.Y,  TargetVector.Z);
}

FVector MoveLogic_WallRun::GetCurrentValueForWallRunValocity(const FRotator& ActorRotation, const float XYSpeed, const float ZSpeed)
{
	float RadYaw = FMath::DegreesToRadians(ActorRotation.Yaw);
	
	FVector DirectionVector = FVector(FMath::Cos(RadYaw), FMath::Sin(RadYaw), 0.0f);
	
	FVector NewVelocity = DirectionVector * XYSpeed;
	NewVelocity.Z = ZSpeed;
		
	return NewVelocity;
}

FVector MoveLogic_WallRun::GetCurrentValueForWallRunValocity(const FRotator &ActorRotation, const float XYZSpeed)
{
	float RadYaw = FMath::DegreesToRadians(ActorRotation.Yaw);
	FVector DirectionVector = FVector(FMath::Cos(RadYaw), FMath::Sin(RadYaw), 0.0f);
	
	return 	DirectionVector * XYZSpeed;
}
