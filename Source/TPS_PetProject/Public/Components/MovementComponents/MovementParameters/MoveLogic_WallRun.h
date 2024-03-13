// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FWallRunningParameters;
class UCurveVector;

namespace MoveLogic_WallRun
{
	
	FVector GetInitOwnerLocationByElapsedTime(const FWallRunningParameters& WallRunMovementParameters, float ElapsedTime);
	
	FVector GetInitStartOwnerLocationByElapsedTime(const FWallRunningParameters& WallRunMovementParameters, const FRotator& ActorRotation, float ElapsedTime);
	FVector GetInitStopOwnerLocationByElapsedTime (const FWallRunningParameters& WallRunMovementParameters, const FRotator& ActorRotation, float ElapsedTime);

	FVector GetCurrentValueForWallRunValocity(const FRotator& ActorRotation, const float XYSpeed, const float ZSpeed);
	FVector GetCurrentValueForWallRunValocity(const FRotator& ActorRotation, const float XYZSpeed);
}
