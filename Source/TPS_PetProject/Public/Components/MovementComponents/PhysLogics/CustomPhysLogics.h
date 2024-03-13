// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct FMantlingParameters;
class UTPS_CharacterMovementComponent;

class UPhysLogicComponent
{
	friend class UTPS_CharacterMovementComponent;

	
	
	static void PhysCustomJump    (const UTPS_CharacterMovementComponent* MovementComponent);
	static void PhysMantling      (UTPS_CharacterMovementComponent* MovementComponent);
	static void PhysOnLadder      (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime);
	static void PhysOnZipline     (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime);
	static void PhysWallRunning   (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime);
	
	static void CustomPhysRotation(UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime);
};
