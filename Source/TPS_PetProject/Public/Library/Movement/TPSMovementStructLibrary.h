// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSMovementEnumLibrary.h"




class UCurveVector;

struct FMantlingParameters
{
	FVector  InitLocation     {FVector::ZeroVector};
	FRotator InitRotation     {FRotator::ZeroRotator};
	FVector  InitAnimLocation {FVector::ZeroVector};

	FVector  TargetLocation   {FVector::ZeroVector};
	FRotator TargetRotation   {FRotator::ZeroRotator};

	float Duration  = 1.f;
	float StartTime = 0.f;

	UCurveVector* MantlingCurve;

	bool bNeedZValueChange = false;
};

struct FWallRunningParameters
{
	FVector  InitLocation     {FVector::ZeroVector};
	
	FVector  TargetInitLocation {FVector::ZeroVector};
	FRotator TargetRotation     {FRotator::ZeroRotator};

	struct FMultiTimer
	{
		float InitTimer  = 0.f;
		float StartTimer = 0.f;
		float CycleTimer = 0.f;
		float StopTimer  = 0.f;
	};
	
	FMultiTimer Timer {0,0,0,0};

	UCurveVector* InitCurve;
	UCurveVector* WallRunStartCurve;
	UCurveVector* WallRunStopCurve;

	bool bIsLeftSide = false;

	EWallRunningState MovementState = Init;
	UAnimMontage* StopAnimMontage;
};

struct FCJumpMovementParameters
{
	FVector  TargetInitLocation {FVector::ZeroVector};
	FRotator TargetRotation     {FRotator::ZeroRotator};

	UCurveVector* AnimMoveCurve;
	float Duration = 0.f;
};

// bool UTPS_CharacterMovementComponent::TryWallRun()
// {
// 	if (!IsFalling()) return false;
// 	if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
// 	if (Velocity.Z > -MaxVerticalWallRunSpeed) return false;
// 	const float CapR = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius();
// 	const float CapHH = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
// 	FVector Start = UpdatedComponent->GetComponentLocation();
// 	FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * CapR * 2.f; //TODO
// 	FVector RightEnd = Start + UpdatedComponent->GetRightVector() * CapR * 2.f; //TODO
// 	auto Params = GetBaseCharacterOwner()->GetIgnorCharacterParams();
// 	FHitResult FloorHit, WallHit;
//
// 	// Check Player Height
// 	if (GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH + MinWallRunHeight), "BlockAll", Params))
// 	{
// 		return false;
// 	}
//
// 	// Left Cast
// 	GetWorld()->LineTraceSingleByChannel(WallHit, Start, LeftEnd, ECC_WALL_RUNNING, Params);
// 	if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
// 	{
// 		bWallRunIsRight = false;
// 	}
// 	// Right Cast
// 	else
// 	{
// 		GetWorld()->LineTraceSingleByChannel(WallHit, Start, RightEnd, ECC_WALL_RUNNING, Params);
// 		if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
// 		{
// 			bWallRunIsRight = true;
// 		}
// 		else
// 		{
// 			return false;
// 		}
// 	}
// 	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
// 	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallRunSpeed, 2)) return false;
//
// 	// Passed all Conditions
// 	Velocity = ProjectedVelocity;
// 	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallRunSpeed);
// 	SetMovementMode(MOVE_Custom, CMOVE_WallRunning);
// 	GetBaseCharacterOwner()->SetMovementState(ETPSMovementState::WallRunning);
// 	//GetBaseCharacterOwner()->TryChangeState_WallRun();
// 	return true;
// }
//
// void UTPS_CharacterMovementComponent::PhysWallRun(const float &deltaTime, int32 Iterations)
// {
// 	if (deltaTime < MIN_TICK_TIME) return;
//
// 	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity()))
// 	{
// 		Acceleration = FVector::ZeroVector;
// 		Velocity = FVector::ZeroVector;
// 		//GetBaseCharacterOwner()->TryChangeState_WallRun(false);
// 		return;
// 	}
//
// 	bJustTeleported = false;
// 	float remainingTime = deltaTime;
//
// 	// Perform move
// 	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
// 	{
// 		++Iterations;
// 		bJustTeleported = false;
// 		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
// 		remainingTime -= timeTick;
// 		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
// 		FVector Start = UpdatedComponent->GetComponentLocation();
// 		FVector CastDelta = UpdatedComponent->GetRightVector() * GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f;
// 		FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
// 		auto Params = GetBaseCharacterOwner()->GetIgnorCharacterParams();
// 		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallRunPullAwayAngle));
// 		FHitResult WallHit;
// 		GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECC_WALL_RUNNING, Params);
// 		bool bWantsToPullAway = WallHit.IsValidBlockingHit() && !Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;
// 		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
// 		{
// 			//GetBaseCharacterOwner()->TryChangeState_WallRun(false);
// 			SetMovementMode(MOVE_Falling);
// 			GetBaseCharacterOwner()->SetMovementState(ETPSMovementState::InAir);
// 			StartNewPhysics(remainingTime, Iterations);
// 			return;
// 		}
//
// 		// Clamp Acceleration
// 		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
// 		Acceleration.Z = 0.f;
//
// 		// Apply Acceleration
// 		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
// 		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
// 		float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();
// 		bool bValUp = Velocity.Z > 0.f;
// 		Velocity.Z += GetGravityZ() * WallRunGravityScaleCurve->GetFloatValue(bValUp ? 0.f : TangentAccel) * timeTick;
// 		if (Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2) || Velocity.Z < -MaxVerticalWallRunSpeed)
// 		{
// 			SetMovementMode(MOVE_Falling);
// 			GetBaseCharacterOwner()->SetMovementState(ETPSMovementState::InAir);
// 			StartNewPhysics(remainingTime, Iterations);
// 			return;
// 		}
//
// 		// Compute move parameters
// 		const FVector Delta = timeTick * Velocity;
// 		const bool bZeroDelta = Delta.IsNearlyZero();
// 		if (bZeroDelta)
// 		{
// 			remainingTime = 0.f;
// 		}
// 		else
// 		{
// 			FHitResult Hit;
// 			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
// 			FVector WallAttractionDelta = -WallHit.Normal * WallAtractionForce * timeTick;
// 			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
// 		}
// 		if (UpdatedComponent->GetComponentLocation() == OldLocation)
// 		{
// 			remainingTime = 0.f;
// 			break;
// 		}
// 		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
// 	}
// 	FVector Start = UpdatedComponent->GetComponentLocation();
// 	FVector CastDelta = UpdatedComponent->GetRightVector() * GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2.f;
// 	FVector End = bWallRunIsRight ? Start + CastDelta : Start - CastDelta;
// 	auto Params = GetBaseCharacterOwner()->GetIgnorCharacterParams();
// 	FHitResult FloorHit, WallHit;
// 	GetWorld()->LineTraceSingleByChannel(WallHit, Start, End, ECC_WALL_RUNNING, Params);
// 	GetWorld()->LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + MinWallRunHeight * 0.5f), "BlockAll", Params);
// 	if (FloorHit.IsValidBlockingHit() || !WallHit.IsValidBlockingHit() || Velocity.SizeSquared2D() < pow(MinWallRunSpeed, 2))
// 	{
// 		SetMovementMode(MOVE_Falling);
// 		GetBaseCharacterOwner()->SetMovementState(ETPSMovementState::InAir);
// 	}
// }