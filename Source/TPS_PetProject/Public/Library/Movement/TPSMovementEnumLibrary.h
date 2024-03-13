// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"



UENUM(BlueprintType)
enum ECustomMovementMode : uint8
{
	CMOVE_None = 0    UMETA(DisplayName = "None"),
	CMOVE_Mantling    UMETA(DisplayName = "Mantling"),
	CMOVE_OnLadder    UMETA(DisplayName = "Mantling"),
	CMOVE_OnZipline   UMETA(DisplayName = "Mantling"),
	CMOVE_Sliding     UMETA(DisplayName = "Mantling"),
	CMOVE_WallRunning UMETA(DisplayName = "WallRunning"),
	CMOVE_CustomJump  UMETA(DisplayName = "CustomJump"),
	CMOVE_Max         UMETA(Hidden) 
};

UENUM(BlueprintType)
enum EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};

UENUM(BlueprintType)
enum EWallRunningState : uint8
{
	Init = 0,
	StartMove,
	CycleMove,
	StopMove,
};