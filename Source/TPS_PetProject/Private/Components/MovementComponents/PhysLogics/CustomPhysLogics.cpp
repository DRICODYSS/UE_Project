// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/PhysLogics/CustomPhysLogics.h"

#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "Character/TPS_BaseCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"
#include "Components/MovementComponents/MovementParameters/MoveLogic_CustomJump.h"
#include "Components/MovementComponents/MovementParameters/MoveLogic_WallRun.h"
#include "Curves/CurveVector.h"



void UPhysLogicComponent::PhysCustomJump(const UTPS_CharacterMovementComponent* MovementComponent)
{
	float ElapsedTime = MovementComponent->GetWorld()->GetTimerManager().GetTimerElapsed(MovementComponent->CustomTimerHandle);
	
	MovementComponent->GetBaseCharacterOwner()->SetActorLocation(
		MoveLogic_WallRun::GetOwnerLocationByDeltaTime(
			MovementComponent->CurrentCJumpMovementParameters,
			MovementComponent->CurrentWallRunParameters.TargetRotation,
			MovementComponent->CurrentWallRunParameters.bIsLeftSide,
			ElapsedTime
		)
	);
	
}

void UPhysLogicComponent::PhysMantling(UTPS_CharacterMovementComponent* MovementComponent)
{
	const float ElapsedTime = MovementComponent->GetWorld()->GetTimerManager().GetTimerElapsed(MovementComponent->CustomTimerHandle) + MovementComponent->CurrentMantlingParameters.StartTime; 
	FVector CurveValue = MovementComponent->CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);

	float PositionAlpha     = CurveValue.X;
	float XYCorrectionAlpha = CurveValue.Y;
	float ZCorrectionAlpha  = CurveValue.Z;

	FVector CorrectedInitialLocation   = FMath::Lerp(MovementComponent->CurrentMantlingParameters.InitLocation,   MovementComponent->CurrentMantlingParameters.InitAnimLocation,   XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(MovementComponent->CurrentMantlingParameters.InitLocation.Z, MovementComponent->CurrentMantlingParameters.InitAnimLocation.Z, ZCorrectionAlpha);
				
	FVector NewLocation    = FMath::Lerp(CorrectedInitialLocation, MovementComponent->CurrentMantlingParameters.TargetLocation, PositionAlpha);
	if (MovementComponent->CurrentMantlingParameters.bNeedZValueChange)
		NewLocation.Z  = FMath::Lerp(MovementComponent->CurrentMantlingParameters.InitLocation.Z, MovementComponent->CurrentMantlingParameters.TargetLocation.Z, ZCorrectionAlpha);
	
	FVector    Delta       = NewLocation - MovementComponent->GetOwner()->GetActorLocation();
	FRotator   NewRotation = FMath::Lerp(MovementComponent->CurrentMantlingParameters.InitRotation, MovementComponent->CurrentMantlingParameters.TargetRotation, PositionAlpha);
	FHitResult Hit;
	MovementComponent->SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UPhysLogicComponent::PhysOnLadder(UTPS_CharacterMovementComponent* MovementComponent,const float &deltaTime)
{
	MovementComponent->CalcVelocity(deltaTime, 1.f, false, MovementComponent->ClimbingOnLadderBrakingDecelartion);
	FVector    Delta = MovementComponent->Velocity * deltaTime;

	if (MovementComponent->HasAnimRootMotion())
	{
		FHitResult Hit;
		MovementComponent->SafeMoveUpdatedComponent(Delta, MovementComponent->GetOwner()->GetActorRotation(), false, Hit);
		return;
	}
	

	FVector NewPos = MovementComponent->GetActorLocation() + Delta;
	float NewPosProjection = MovementComponent->GetActorToLadderProjection(NewPos);
	if (NewPosProjection < MovementComponent->MinLadderBottomOffset)
	{
		MovementComponent->DetachFromLadderMethod = ReachingTheBottom;
		MovementComponent->GetBaseCharacterOwner()->TryChangeState_InteractWithLadder(false);
		return;
	}
	else if (NewPosProjection > MovementComponent->CurrentLadder->GetLadderHeight() - MovementComponent->MaxLadderTopOffset)
	{
		MovementComponent->DetachFromLadderMethod = ReachingTheTop;
		MovementComponent->Velocity = FVector::ZeroVector;
		MovementComponent->bIgnoreMovementState_OnLadder = true;
		MovementComponent->GetBaseCharacterOwner()->TryChangeState_Mantle(true);
		return;
	}
	FHitResult Hit;
	MovementComponent->SafeMoveUpdatedComponent(Delta, MovementComponent->GetOwner()->GetActorRotation(), true, Hit);
}

void UPhysLogicComponent::PhysOnZipline(UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime)
{
	MovementComponent->CalcVelocity(deltaTime, 1.f, false, MovementComponent->ZiplineBrakingDeceleration);
	FVector Delta = MovementComponent->Velocity * deltaTime;
	
	FHitResult Hit;
	MovementComponent->SafeMoveUpdatedComponent(Delta, MovementComponent->GetOwner()->GetActorRotation(), true, Hit);

	const FVector LadderUpVector = MovementComponent->CurrentZipline->GetEndLocation() - MovementComponent->CurrentZipline->GetStartLocation();
	MovementComponent->PawnOwner->AddMovementInput(LadderUpVector, 1);
}

void UPhysLogicComponent::PhysWallRunning(UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime)
{
	float   ElapsedTime = MovementComponent->GetWorld()->GetTimerManager().GetTimerElapsed(MovementComponent->CustomTimerHandle);

	switch (MovementComponent->CurrentWallRunParameters.MovementState)
	{
		case Init:
		{
			FHitResult Hit;
			FVector Delta  = MoveLogic_WallRun::GetInitOwnerLocationByElapsedTime(MovementComponent->CurrentWallRunParameters, ElapsedTime) - MovementComponent->GetActorLocation();
			MovementComponent->SafeMoveUpdatedComponent(Delta, MovementComponent->CurrentWallRunParameters.TargetRotation, false, Hit);
			break;
		}
		case StartMove:
		{
			MovementComponent->GetBaseCharacterOwner()->SetActorLocation(MoveLogic_WallRun::GetInitStartOwnerLocationByElapsedTime(MovementComponent->CurrentWallRunParameters, MovementComponent->GetBaseCharacterOwner()->GetActorRotation(), ElapsedTime));
			break;
		}
		case CycleMove: 
		{
			MovementComponent->Velocity =  MoveLogic_WallRun::GetCurrentValueForWallRunValocity(MovementComponent->GetBaseCharacterOwner()->GetActorRotation(), MovementComponent->WallRunningSpeed);
			MovementComponent->CalcVelocity(deltaTime, 1.f, false, MovementComponent->WallRunBrakingDeceleration);

			FHitResult Hit;
			MovementComponent->SafeMoveUpdatedComponent(MovementComponent->Velocity * deltaTime, MovementComponent->GetOwner()->GetActorRotation(), true, Hit);
			
			MovementComponent->PawnOwner->AddMovementInput(MovementComponent->PawnOwner->GetActorForwardVector(), 1);
			MovementComponent->CurrentWallRunParameters.InitLocation = MovementComponent->GetOwner()->GetActorLocation();
			break;
		}
		case StopMove:
		{
			MovementComponent->GetBaseCharacterOwner()->SetActorRotation(MovementComponent->CurrentWallRunParameters.TargetRotation);

			FVector NewLocation = MoveLogic_WallRun::GetInitStopOwnerLocationByElapsedTime(MovementComponent->CurrentWallRunParameters, MovementComponent->GetBaseCharacterOwner()->GetActorRotation(), ElapsedTime);
			MovementComponent->GetBaseCharacterOwner()->SetActorLocation(NewLocation);
			
			MovementComponent->Velocity = MoveLogic_WallRun::GetCurrentValueForWallRunValocity(MovementComponent->GetBaseCharacterOwner()->GetActorRotation(), 500.f, -1200.f);
			break;
		}
	}
}



void UPhysLogicComponent::CustomPhysRotation(UTPS_CharacterMovementComponent *MovementComponent, const float &deltaTime)
{
	if (MovementComponent->bForceRotation)
	{
		FRotator CurrentRotation = MovementComponent->UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = MovementComponent->GetDeltaRotation(deltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;
		if (!CurrentRotation.Equals(MovementComponent->ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = MovementComponent->ForceTargetRotation;

			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MovementComponent->MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
		else
		{
			MovementComponent->ForceTargetRotation = FRotator::ZeroRotator;
			MovementComponent->bForceRotation = false;
		}
		return;
	}
	if(MovementComponent->GetBaseCharacterOwner()->GetMovementState() == ETPSMovementState::Swimming)
	{
		//TODO add change bUseControllerDesiredRotation
	}
}
