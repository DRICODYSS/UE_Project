// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/TPSPlayerCameraBehavior.h"

#include "Library/Character/TPSCharacterEnumLibrary.h"


void UTPSPlayerCameraBehavior::SetRotationMode(ETPSRotationMode RotationMode)
{
	bVelocityDirection = RotationMode == ETPSRotationMode::VelocityDirection;
	bLookingDirection = RotationMode == ETPSRotationMode::LookingDirection;
	bAiming = RotationMode == ETPSRotationMode::Aiming;
}
