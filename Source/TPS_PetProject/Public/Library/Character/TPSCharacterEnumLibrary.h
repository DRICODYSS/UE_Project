// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TPSCharacterEnumLibrary.generated.h"

/* Returns the enumeration index. */
template <typename Enumeration>
static FORCEINLINE int32 GetEnumerationIndex(const Enumeration InValue)
{
	return StaticEnum<Enumeration>()->GetIndexByValue(static_cast<int64>(InValue));
}

/* Returns the enumeration value as string. */
template <typename Enumeration>
static FORCEINLINE FString GetEnumerationToString(const Enumeration InValue)
{
	return StaticEnum<Enumeration>()->GetNameStringByValue(static_cast<int64>(InValue));
}

/**
 * Character gait state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_Gait"))
enum class ETPSGait : uint8
{
	Walking,
	Running,
	Sprinting
};

/**
 * Character movement action state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_MovementAction"))
enum class ETPSMovementAction : uint8
{
	None,
	LowMantle,
	HighMantle,
	Rolling,
	GettingUp
};

/**
 * Character movement state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_MovementState"))
enum class ETPSMovementState : uint8
{
	None,
	Grounded,
	InAir,
	Mantling,
	OnLadder,
	OnZipline,
	Ragdoll,
	Swimming,
	WallRunning,
	CustomJump,
	Sliding
};

UENUM(BlueprintType)
enum EJumpTypes : uint8
{
	Common,
	WallRunJumpF,
	WallRunJump_90
};

/**
 * Character overlay state. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_OverlayState"))
enum class ETPSOverlayState : uint8
{
	Default,
	Masculine,
	Feminine,
	Injured,
	HandsTied,
	Rifle,
	PistolOneHanded,
	PistolTwoHanded,
	Bow,
	Torch,
	Binoculars,
	Box,
	Barrel
};

/**
 * Character rotation mode. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_RotationMode"))
enum class ETPSRotationMode : uint8
{
	VelocityDirection,
	LookingDirection,
	Aiming
};

/**
 * Character stance. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_Stance"))
enum class ETPSStance : uint8
{
	Standing,
	Crouching
};

/**
 * Character view mode. Note: Also edit related struct in ALSStructEnumLibrary if you add new enums
 */
UENUM(BlueprintType, meta = (ScriptName = "ALS_ViewMode"))
enum class ETPSViewMode : uint8
{
	ThirdPerson,
	FirstPerson
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_AnimFeatureExample"))
enum class ETPSAnimFeatureExample : uint8
{
	StrideBlending,
	AdditiveBlending,
	SprintImpulse
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_FootstepType"))
enum class ETPSFootstepType : uint8
{
	Step,
	WalkRun,
	Jump,
	Land
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_GroundedEntryState"))
enum class ETPSGroundedEntryState : uint8
{
	None,
	Roll
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_HipsDirection"))
enum class ETPSHipsDirection : uint8
{
	F,
	B,
	RF,
	RB,
	LF,
	LB
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_MantleType"))
enum class ETPSMantleType : uint8
{
	HighMantle,
	LowMantle,
	FallingCatch
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_MovementDirection"))
enum class ETPSMovementDirection : uint8
{
	Forward,
	Right,
	Left,
	Backward
};

UENUM(BlueprintType, meta = (ScriptName = "ALS_SpawnType"))
enum class ETPSSpawnType : uint8
{
	Location,
	Attached
};
