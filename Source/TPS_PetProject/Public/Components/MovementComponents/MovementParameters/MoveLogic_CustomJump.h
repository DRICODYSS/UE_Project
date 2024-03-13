// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



struct FCJumpMovementParameters;

namespace MoveLogic_WallRun
{

	FVector GetOwnerLocationByDeltaTime(const FCJumpMovementParameters& CJumpMovementParameters, const FRotator& ActorRotation, bool bIsLeftSide, float ElapsedTime);
}
	
	
