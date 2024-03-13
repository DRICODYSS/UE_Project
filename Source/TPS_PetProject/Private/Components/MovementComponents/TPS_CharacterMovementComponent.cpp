// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"

#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "Character/TPS_BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/MovementComponents/MovementParameters/MoveLogic_WallRun.h"
#include "Curves/CurveVector.h"
#include "Library/Movement/TPSMovementEnumLibrary.h"


void UTPS_CharacterMovementComponent::OnMovementUpdated(float DeltaTime, const FVector &OldLocation, const FVector &OldVelocity)
{
	Super::OnMovementUpdated(DeltaTime, OldLocation, OldVelocity);

	if (!CharacterOwner) return;

	// Set Movement Settings
	if (bRequestMovementSettingsChange)
	{
		const float UpdateMaxWalkSpeed = CurrentMovementSettings.GetSpeedForGait(AllowedGait);
		MaxWalkSpeed = UpdateMaxWalkSpeed;
		MaxWalkSpeedCrouched = UpdateMaxWalkSpeed;

		bRequestMovementSettingsChange = false;
	}
}

void UTPS_CharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bRequestMovementSettingsChange = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}


float UTPS_CharacterMovementComponent::GetMaxSpeed() const
{
	if(MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_OnLadder)    return ClimbingOnLadderSpeed; 
	if(MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_OnZipline)   return SlideOnZiplineSpeed; 
	if(MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_WallRunning) return WallRunningSpeed;
	
	return Super::GetMaxSpeed();
}

float UTPS_CharacterMovementComponent::GetMaxAcceleration() const
{
	if (!IsMovingOnGround() || !CurrentMovementSettings.MovementCurve) return Super::GetMaxAcceleration();
	
	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).X;
}

float UTPS_CharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (!IsMovingOnGround() || !CurrentMovementSettings.MovementCurve) return Super::GetMaxBrakingDeceleration();
	
	return CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).Y;
}

float UTPS_CharacterMovementComponent::GetMappedSpeed() const
{
	// Map the character's current speed to the configured movement speeds with a range of 0-3,
	// with 0 = stopped, 1 = the Walk Speed, 2 = the Run Speed, and 3 = the Sprint Speed.
	// This allows us to vary the movement speeds but still use the mapped range in calculations for consistent results

	const float Speed = Velocity.Size2D();
	const float LocWalkSpeed = CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = CurrentMovementSettings.RunSpeed;
	const float LocSprintSpeed = CurrentMovementSettings.SprintSpeed;

	if (Speed > LocRunSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({LocRunSpeed, LocSprintSpeed}, {2.0f, 3.0f}, Speed);
	}

	if (Speed > LocWalkSpeed)
	{
		return FMath::GetMappedRangeValueClamped<float, float>({LocWalkSpeed, LocRunSpeed}, {1.0f, 2.0f}, Speed);
	}

	return FMath::GetMappedRangeValueClamped<float, float>({0.0f, LocWalkSpeed}, {0.0f, 1.0f}, Speed);
}

void UTPS_CharacterMovementComponent::SetMovementSettings(FTPSMovementSettings NewMovementSettings)
{
	// Set the current movement settings from the owner
	CurrentMovementSettings = NewMovementSettings;
	bRequestMovementSettingsChange = true;
}

void UTPS_CharacterMovementComponent::SetAllowedGait(ETPSGait NewAllowedGait)
{
	if (AllowedGait != NewAllowedGait)
	{
		if (PawnOwner->IsLocallyControlled()) //TODO it might ruin everything 
		{
			AllowedGait = NewAllowedGait;
			if (GetCharacterOwner()->GetLocalRole() == ROLE_AutonomousProxy)
			{
				//Server_SetAllowedGait(NewAllowedGait);
			}
			bRequestMovementSettingsChange = true;
			return;
		}
		if (!PawnOwner->HasAuthority())
		{
			const float UpdateMaxWalkSpeed = CurrentMovementSettings.GetSpeedForGait(AllowedGait);
			MaxWalkSpeed = UpdateMaxWalkSpeed;
			MaxWalkSpeedCrouched = UpdateMaxWalkSpeed;
		}
	}
}

void UTPS_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		
	}
	if(PreviousCustomMode == MOVE_Custom && PreviousCustomMode == CMOVE_OnLadder)  CurrentLadder  = nullptr;
	if(PreviousCustomMode == MOVE_Custom && PreviousCustomMode == CMOVE_OnZipline) CurrentZipline = nullptr;
	
	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
		case CMOVE_Mantling:
			{ GetWorld()->GetTimerManager().SetTimer(CustomTimerHandle, this, &UTPS_CharacterMovementComponent::StopMantling, CurrentMantlingParameters.Duration, false); break; }
		case CMOVE_CustomJump:
			{ GetWorld()->GetTimerManager().SetTimer(CustomTimerHandle, this, &UTPS_CharacterMovementComponent::StopCustomJump, CurrentCJumpMovementParameters.Duration * 0.5, false); break; }
		case CMOVE_WallRunning:
			{ GetWorld()->GetTimerManager().SetTimer(CustomTimerHandle, this, &UTPS_CharacterMovementComponent::ChangeTimerAndState_WallRun, CurrentWallRunParameters.Timer.InitTimer, false); break; }
		default: break;
		}
	}
}

bool UTPS_CharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || CustomMovementMode == CMOVE_Sliding;
}

bool UTPS_CharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}


/* Custom Jump */
void UTPS_CharacterMovementComponent::ChangeTypeJump(const FCJumpMovementParameters &CJumpMovementParameters)
{
	GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
	
	CurrentCJumpMovementParameters = CJumpMovementParameters;
	Velocity = MoveLogic_WallRun::GetCurrentValueForWallRunValocity(GetBaseCharacterOwner()->GetActorRotation(), WallRunningSpeed);
	
	SetMovementMode(MOVE_Custom, CMOVE_CustomJump);
}

void UTPS_CharacterMovementComponent::StopCustomJump()
{
	GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
	
	SetMovementMode(MOVE_Falling);
	GetBaseCharacterOwner()->TryChangeState_WallRun(false);
}


/* Mantling */
void UTPS_CharacterMovementComponent::StartMantling(const FMantlingParameters &MantlingParameters)
{
	GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
	
	CurrentMantlingParameters = MantlingParameters;
	Velocity = FVector::ZeroVector;
	
	SetMovementMode(MOVE_Custom, CMOVE_Mantling);
}

void UTPS_CharacterMovementComponent::StopMantling()
{
	GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
	
	SetMovementMode(MOVE_Walking);
	GetBaseCharacterOwner()->TryChangeState_Mantle(false);
}


/* Ladder */
void UTPS_CharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	Velocity = FVector::ZeroVector;
	
	SetMovementMode(MOVE_Custom, CMOVE_OnLadder);
}

void UTPS_CharacterMovementComponent::DetachFromLadder()
{
	bIgnoreMovementState_OnLadder = false;
	switch (DetachFromLadderMethod)
	{
		case Fall:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
		case EDetachFromLadderMethod::JumpOff:
		{
			FVector JumpDirection = CurrentLadder->GetActorForwardVector();
			SetMovementMode(MOVE_Falling);
			FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
			Launch(JumpVelocity);
			break;
		}
		default:
			SetMovementMode(MOVE_Walking);break;
	}
	DetachFromLadderMethod = EDetachFromLadderMethod::JumpOff;
}

float UTPS_CharacterMovementComponent::GetActorToLadderProjection(const FVector& Location) const
{
	check(IsValid(CurrentLadder));
	
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

float UTPS_CharacterMovementComponent::GetLadderSpeedRation() const
{
	if(!IsValid(CurrentLadder)) return 0.f;
	
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderSpeed;
}


/* Zipline */
void UTPS_CharacterMovementComponent::AttachToZipline(const AZipline *Zipline)
{
	CurrentZipline = Zipline;
	Velocity = FVector::ZeroVector;
	
	SetMovementMode(MOVE_Custom, CMOVE_OnZipline);
}

void UTPS_CharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}


/* WallRun */
void UTPS_CharacterMovementComponent::AttachToRunnableWall(const FWallRunningParameters  &WallRunMovementParameters)
{
	CurrentWallRunParameters = WallRunMovementParameters;
	Velocity = MoveLogic_WallRun::GetCurrentValueForWallRunValocity(GetBaseCharacterOwner()->GetActorRotation(), WallRunningSpeed, -1200.f);
	
	SetMovementMode(MOVE_Custom, CMOVE_WallRunning);
}

void UTPS_CharacterMovementComponent::DetachToRunnableWall()
{
	GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
	SetMovementMode(MOVE_Walking);
}

void UTPS_CharacterMovementComponent::ChangeTimerAndState_WallRun()
{
	switch (CurrentWallRunParameters.MovementState)
	{
	case StopMove  :
	{
		GetBaseCharacterOwner()->TryChangeState_WallRun(false);
		break;
	}
	case CycleMove :
	{
		GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
		GetWorld()->GetTimerManager().SetTimer  (CustomTimerHandle, this, &UTPS_CharacterMovementComponent::ChangeTimerAndState_WallRun, CurrentWallRunParameters.Timer.StopTimer, false);
		GetBaseCharacterOwner()->GetMesh()->GetAnimInstance()->Montage_Play(CurrentWallRunParameters.StopAnimMontage, 1.f, EMontagePlayReturnType::Duration, 0.f);
		CurrentWallRunParameters.MovementState = StopMove;
		break;
	}
	case StartMove :
	{
		GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
		GetWorld()->GetTimerManager().SetTimer  (CustomTimerHandle, this, &UTPS_CharacterMovementComponent::ChangeTimerAndState_WallRun, CurrentWallRunParameters.Timer.CycleTimer, false);
		CurrentWallRunParameters.MovementState = CycleMove;
		break;
	}
	case Init      :
	{
		GetWorld()->GetTimerManager().ClearTimer(CustomTimerHandle);
		GetWorld()->GetTimerManager().SetTimer  (CustomTimerHandle, this, &UTPS_CharacterMovementComponent::ChangeTimerAndState_WallRun, CurrentWallRunParameters.Timer.StartTimer, false);
		CurrentWallRunParameters.MovementState = StartMove;
		break;
	}
	}
}

/* Slide */
void UTPS_CharacterMovementComponent::EnterSlide()
{
	bWantsToCrouch = true;
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	SetMovementMode(MOVE_Custom, CMOVE_Sliding);
}

void UTPS_CharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UTPS_CharacterMovementComponent::PhysSliding( const float &deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME) return;
	
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		GetBaseCharacterOwner()->TryChangeState_Slide(false);
		ExitSlide();
		return;
	}
	
	// Surface Gravity
	Velocity += Slide_GravityForce * FVector::DownVector * deltaTime;

	// Strafe
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > 0.5f)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// Calc Velocity
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, Slide_Friction, true, GetMaxBrakingDeceleration());
	}

	// Perform Move
	++Iterations;
	bJustTeleported = false; //TODO need for averyseing PhysFunc!!!

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.f);
	FVector Delta = Velocity * deltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Delta, NewRotation, true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Delta);
		SlideAlongSurface(Delta, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}

	// if (Velocity.SizeSquared() < pow(Slide_MinSpeed + 50.f, 2))
	// {
	// 	GetBaseCharacterOwner()->TryChangeState_Slide(false);
	// }
	
	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
	{
		GetBaseCharacterOwner()->TryChangeState_Slide(false);
		ExitSlide();
	}

	//Update Outgoing Velocity & Acceleration
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

bool UTPS_CharacterMovementComponent::GetSlideSurface(FHitResult &Hit)
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, GetBaseCharacterOwner()->GetIgnorCharacterParams());
}



/* Physics */
void UTPS_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case CMOVE_CustomJump:  UPhysLogicComponent::PhysCustomJump (this); break;
	case CMOVE_Mantling:    UPhysLogicComponent::PhysMantling   (this); break;
	case CMOVE_OnLadder:    UPhysLogicComponent::PhysOnLadder   (this, deltaTime); break;
	case CMOVE_OnZipline:   UPhysLogicComponent::PhysOnZipline  (this, deltaTime); break;
	case CMOVE_WallRunning: UPhysLogicComponent::PhysWallRunning(this, deltaTime); break;
	case CMOVE_Sliding:     PhysSliding    (deltaTime, Iterations); break;
	default: break;
	}
	Super::PhysCustom(deltaTime, Iterations);
}

void UTPS_CharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	if (CurrentMovementSettings.MovementCurve)
		GroundFriction = CurrentMovementSettings.MovementCurve->GetVectorValue(GetMappedSpeed()).Z;
	
	Super::PhysWalking(deltaTime, Iterations);
}

void UTPS_CharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	UPhysLogicComponent::CustomPhysRotation(this, DeltaTime);
	Super::PhysicsRotation(DeltaTime);
}