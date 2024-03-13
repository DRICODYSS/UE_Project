// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Core_TPSBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Character/Animation/TPSPlayerCameraBehavior.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"


const FName NAME_FP_Camera(TEXT("FP_Camera"));
const FName NAME_Pelvis(TEXT("Pelvis"));
const FName NAME_RagdollPose(TEXT("RagdollPose"));
const FName NAME_RotationAmount(TEXT("RotationAmount"));
const FName NAME_YawOffset(TEXT("YawOffset"));
const FName NAME_pelvis(TEXT("pelvis"));
const FName NAME_root(TEXT("root"));
const FName NAME_spine_03(TEXT("spine_03"));


ATPSCoreBaseCharacter::ATPSCoreBaseCharacter(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTPS_CharacterMovementComponent>(CharacterMovementComponentName))
{
	MyCharacterMovementComponent = StaticCast<UTPS_CharacterMovementComponent*>(GetCharacterMovement());
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	bReplicates = true;
	SetReplicatingMovement(true);
}

void ATPSCoreBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATPSCoreBaseCharacter, TargetRagdollLocation);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, ReplicatedCurrentAcceleration, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, ReplicatedControlRotation, COND_SkipOwner);
	
	DOREPLIFETIME(ATPSCoreBaseCharacter, DesiredGait);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, DesiredStance, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, DesiredRotationMode, COND_SkipOwner);
	
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, RotationMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, OverlayState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, ViewMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ATPSCoreBaseCharacter, VisibleMesh, COND_SkipOwner);
}

void ATPSCoreBaseCharacter::OnBreakfall_Implementation() { Replicated_PlayMontage(GetRollAnimation(), 1.35); }

void ATPSCoreBaseCharacter::Replicated_PlayMontage_Implementation(UAnimMontage *Montage, float PlayRate)
{
	// Roll: Simply play a Root Motion Montage.
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	Server_PlayMontage(Montage, PlayRate);
}

void ATPSCoreBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// If we're in networked game, disable curved movement
	bEnableNetworkOptimizations = !IsNetMode(NM_Standalone);

	// Make sure the mesh and animbp update after the CharacterBP to ensure it gets the most recent values.
	GetMesh()->AddTickPrerequisiteActor(this);

	// Set the Movement Model
	SetMovementModel();

	// Force update states to use the initial desired values.
	ForceUpdateCharacterState();

	if (Stance == ETPSStance::Standing)
	{
		UnCrouch();
	}
	else if (Stance == ETPSStance::Crouching)
	{
		Crouch();
	}

	// Set default rotation values.
	TargetRotation = GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;

	if (GetMesh()->GetAnimInstance() && GetLocalRole() == ROLE_SimulatedProxy)
	{
		GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void ATPSCoreBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Set required values
	SetEssentialValues(DeltaTime);

	if (MovementState == ETPSMovementState::Grounded || MovementState == ETPSMovementState::Swimming)
	{
		UpdateCharacterMovement();
		UpdateGroundedRotation(DeltaTime);
	}
	else if (MovementState == ETPSMovementState::InAir)
	{
		UpdateInAirRotation(DeltaTime);
	}
	else if (MovementState == ETPSMovementState::Ragdoll)
	{
		RagdollUpdate(DeltaTime);
	}

	// Cache values
	PreviousVelocity = GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

void ATPSCoreBaseCharacter::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}

	/** When Networked, disables replicate movement reset TargetRagdollLocation and ServerRagdollPull variable
	and if the host is a dedicated server, change character mesh optimisation option to avoid z-location bug*/
	MyCharacterMovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = 1;

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		DefVisBasedTickOp = GetMesh()->VisibilityBasedAnimTickOption;
		GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
	ServerRagdollPull = 0;

	// Disable URO
	bPreRagdollURO = GetMesh()->bEnableUpdateRateOptimizations;
	GetMesh()->bEnableUpdateRateOptimizations = false;

	// Step 1: Clear the Character Movement Mode and set the Movement State to Ragdoll
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(ETPSMovementState::Ragdoll);

	// Step 2: Disable capsule collision and enable mesh physics simulation starting from the pelvis.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(NAME_Pelvis, true, true);

	// Step 3: Stop any active montages.
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
	}

	// Fixes character mesh is showing default A pose for a split-second just before ragdoll ends in listen server games
	GetMesh()->bOnlyAllowAutonomousTickPose = true;

	SetReplicateMovement(false);
}

void ATPSCoreBaseCharacter::RagdollEnd()
{
	/** Re-enable Replicate Movement and if the host is a dedicated server set mesh visibility based anim
	tick option back to default*/

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		GetMesh()->VisibilityBasedAnimTickOption = DefVisBasedTickOp;
	}

	GetMesh()->bEnableUpdateRateOptimizations = bPreRagdollURO;

	// Revert back to default settings
	MyCharacterMovementComponent->bIgnoreClientMovementErrorChecksAndCorrection = 0;
	GetMesh()->bOnlyAllowAutonomousTickPose = false;
	SetReplicateMovement(true);

	// Step 1: Save a snapshot of the current Ragdoll Pose for use in AnimGraph to blend out of the ragdoll
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->SavePoseSnapshot(NAME_RagdollPose);
	}

	// Step 2: If the ragdoll is on the ground, set the movement mode to walking and play a Get Up animation.
	// If not, set the movement mode to falling and update the character movement velocity to match the last ragdoll
	// velocity.
	if (bRagdollOnGround)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if (GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(GetGetUpAnimation(bRagdollFaceUp), 1.0f,
													   EMontagePlayReturnType::MontageLength, 0.0f, true);
		}
	}
	else
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	// Step 3: Re-Enable capsule collision, and disable physics simulation on the mesh.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetAllBodiesSimulatePhysics(false);

	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void ATPSCoreBaseCharacter::Server_SetMeshLocationDuringRagdoll_Implementation(FVector MeshLocation)
{
	TargetRagdollLocation = MeshLocation;
}

void ATPSCoreBaseCharacter::SetMovementState(const ETPSMovementState NewState, bool bForce)
{
	if (bForce || MovementState != NewState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;
		OnMovementStateChanged(PrevMovementState);
	}
}

void ATPSCoreBaseCharacter::SetMovementAction(const ETPSMovementAction NewAction, bool bForce)
{
	if (bForce || MovementAction != NewAction)
	{
		const ETPSMovementAction Prev = MovementAction;
		MovementAction = NewAction;
		OnMovementActionChanged(Prev);
	}
}

void ATPSCoreBaseCharacter::SetStance(const ETPSStance NewStance, bool bForce)
{
	if (bForce || Stance != NewStance)
	{
		const ETPSStance Prev = Stance;
		Stance = NewStance;
		OnStanceChanged(Prev);
	}
}

void ATPSCoreBaseCharacter::SetOverlayOverrideState(int32 NewState) { OverlayOverrideState = NewState; }

void ATPSCoreBaseCharacter::SetGait(const ETPSGait NewGait, bool bForce)
{
	if (bForce || Gait != NewGait)
	{
		const ETPSGait Prev = Gait;
		Gait = NewGait;
		//OnGaitChanged(Prev);
	}
}


void ATPSCoreBaseCharacter::SetDesiredStance(ETPSStance NewStance)
{
	DesiredStance = NewStance;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredStance(NewStance);
	}
}

void ATPSCoreBaseCharacter::Server_SetDesiredStance_Implementation(ETPSStance NewStance) { SetDesiredStance(NewStance); }

void ATPSCoreBaseCharacter::SetDesiredGait(const ETPSGait NewGait)
{
	DesiredGait = NewGait;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredGait(NewGait);
	}
}

void ATPSCoreBaseCharacter::Server_SetDesiredGait_Implementation(ETPSGait NewGait) { SetDesiredGait(NewGait); }

void ATPSCoreBaseCharacter::SetDesiredRotationMode(ETPSRotationMode NewRotMode)
{
	DesiredRotationMode = NewRotMode;
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredRotationMode(NewRotMode);
	}
}

void ATPSCoreBaseCharacter::Server_SetDesiredRotationMode_Implementation(ETPSRotationMode NewRotMode)
{
	SetDesiredRotationMode(NewRotMode);
}

void ATPSCoreBaseCharacter::SetRotationMode(const ETPSRotationMode NewRotationMode, bool bForce)
{
	if (bForce || RotationMode != NewRotationMode)
	{
		const ETPSRotationMode Prev = RotationMode;
		RotationMode = NewRotationMode;
		OnRotationModeChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetRotationMode(NewRotationMode, bForce);
		}
	}
}


void ATPSCoreBaseCharacter::Server_SetRotationMode_Implementation(ETPSRotationMode NewRotationMode, bool bForce)
{
	SetRotationMode(NewRotationMode, bForce);
}

void ATPSCoreBaseCharacter::SetViewMode(const ETPSViewMode NewViewMode, bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
	{
		const ETPSViewMode Prev = ViewMode;
		ViewMode = NewViewMode;
		OnViewModeChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetViewMode(NewViewMode, bForce);
		}
	}
}

void ATPSCoreBaseCharacter::Server_SetViewMode_Implementation(ETPSViewMode NewViewMode, bool bForce)
{
	SetViewMode(NewViewMode, bForce);
}

void ATPSCoreBaseCharacter::SetOverlayState(const ETPSOverlayState NewState, bool bForce)
{
	if (bForce || OverlayState != NewState)
	{
		const ETPSOverlayState Prev = OverlayState;
		OverlayState = NewState;
		OnOverlayStateChanged(Prev);

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_SetOverlayState(NewState, bForce);
		}
	}
}

void ATPSCoreBaseCharacter::SetGroundedEntryState(ETPSGroundedEntryState NewState) { GroundedEntryState = NewState; }


void ATPSCoreBaseCharacter::Server_SetOverlayState_Implementation(ETPSOverlayState NewState, bool bForce)
{
	SetOverlayState(NewState, bForce);
}

void ATPSCoreBaseCharacter::EventOnLanded()
{
	const float VelZ = FMath::Abs(GetCharacterMovement()->Velocity.Z);

	if (bRagdollOnLand && VelZ > RagdollOnLandVelocity)
	{
		ReplicatedRagdollStart();
	}
	else if (bBreakfallOnLand && bHasMovementInput && VelZ >= BreakfallOnLandVelocity)
	{
		OnBreakfall();
	}
	else
	{
		GetCharacterMovement()->BrakingFrictionFactor = bHasMovementInput ? 0.5f : 3.0f;

		// After 0.5 secs, reset braking friction factor to zero
		GetWorldTimerManager().SetTimer(OnLandedFrictionResetTimer, this, &ATPSCoreBaseCharacter::OnLandFrictionReset, 0.5f,false);
	}
}

void ATPSCoreBaseCharacter::Multicast_OnLanded_Implementation()
{
	if (!IsLocallyControlled())
	{
		EventOnLanded();
	}
}

void ATPSCoreBaseCharacter::EventOnJumped()
{
	// Set the new In Air Rotation to the velocity rotation if speed is greater than 100.
	InAirRotation = Speed > 100.0f ? LastVelocityRotation : GetActorRotation();

	OnJumpedDelegate.Broadcast();
}

void ATPSCoreBaseCharacter::Server_PlayMontage_Implementation(UAnimMontage *Montage, float PlayRate)
{
	if (GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	ForceNetUpdate();
	Multicast_PlayMontage(Montage, PlayRate);
}

void ATPSCoreBaseCharacter::Multicast_PlayMontage_Implementation(UAnimMontage *Montage, float PlayRate)
{
	if (GetMesh()->GetAnimInstance() && !IsLocallyControlled())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}
}

void ATPSCoreBaseCharacter::Multicast_OnJumped_Implementation()
{
	if (!IsLocallyControlled())
	{
		EventOnJumped();
	}
}

void ATPSCoreBaseCharacter::Server_RagdollStart_Implementation() { Multicast_RagdollStart(); }

void ATPSCoreBaseCharacter::Multicast_RagdollStart_Implementation() { RagdollStart(); }

void ATPSCoreBaseCharacter::Server_RagdollEnd_Implementation(FVector CharacterLocation)
{
	Multicast_RagdollEnd(CharacterLocation);
}

void ATPSCoreBaseCharacter::Multicast_RagdollEnd_Implementation(FVector CharacterLocation) { RagdollEnd(); }

void ATPSCoreBaseCharacter::SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation)
{
	SetActorLocationAndRotation(NewLocation, NewRotation);
	TargetRotation = NewRotation;
}

void ATPSCoreBaseCharacter::SetMovementModel()
{
	const FString ContextString = GetFullName();
	FTPSMovementStateSettings *OutRow =
		MovementModel.DataTable->FindRow<FTPSMovementStateSettings>(MovementModel.RowName, ContextString);
	check(OutRow);
	MovementData = *OutRow;
}

void ATPSCoreBaseCharacter::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}

FTPSMovementSettings ATPSCoreBaseCharacter::GetTargetMovementSettings() const
{
	if (RotationMode == ETPSRotationMode::VelocityDirection)
	{
		if (Stance == ETPSStance::Standing)
		{
			return MovementData.VelocityDirection.Standing;
		}
		if (Stance == ETPSStance::Crouching)
		{
			return MovementData.VelocityDirection.Crouching;
		}
	}
	else if (RotationMode == ETPSRotationMode::LookingDirection)
	{
		if (Stance == ETPSStance::Standing)
		{
			return MovementData.LookingDirection.Standing;
		}
		if (Stance == ETPSStance::Crouching)
		{
			return MovementData.LookingDirection.Crouching;
		}
	}
	else if (RotationMode == ETPSRotationMode::Aiming)
	{
		if (Stance == ETPSStance::Standing)
		{
			return MovementData.Aiming.Standing;
		}
		if (Stance == ETPSStance::Crouching)
		{
			return MovementData.Aiming.Crouching;
		}
	}

	// Default to velocity dir standing
	return MovementData.VelocityDirection.Standing;
}

bool ATPSCoreBaseCharacter::CanSprint() const
{
	// Determine if the character is currently able to sprint based on the Rotation mode and current acceleration
	// (input) rotation. If the character is in the Looking Rotation mode, only allow sprinting if there is full
	// movement input and it is faced forward relative to the camera + or - 50 degrees.

	if (!bHasMovementInput || RotationMode == ETPSRotationMode::Aiming)
	{
		return false;
	}
	
	const bool bValidInputAmount = MovementInputAmount > 0.9f;
	
	if (RotationMode == ETPSRotationMode::VelocityDirection)
	{
		return bValidInputAmount;
	}
	
	if (RotationMode == ETPSRotationMode::LookingDirection)
	{
		const FRotator AccRot = ReplicatedCurrentAcceleration.ToOrientationRotator();
		FRotator Delta = AccRot - AimingRotation;
		Delta.Normalize();
	
		return bValidInputAmount && FMath::Abs(Delta.Yaw) < 50.0f;
	}

	return false;
}

FVector ATPSCoreBaseCharacter::GetMovementInput() const { return ReplicatedCurrentAcceleration; }

float ATPSCoreBaseCharacter::GetAnimCurveValue(FName CurveName) const
{
	if (GetMesh()->GetAnimInstance())
	{
		return GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	}

	return 0.0f;
}

void ATPSCoreBaseCharacter::SetVisibleMesh(USkeletalMesh *NewVisibleMesh)
{
	if (VisibleMesh != NewVisibleMesh)
	{
		const USkeletalMesh *Prev = VisibleMesh;
		VisibleMesh = NewVisibleMesh;
		OnVisibleMeshChanged(Prev);

		if (GetLocalRole() != ROLE_Authority)
		{
			Server_SetVisibleMesh(NewVisibleMesh);
		}
	}
}

void ATPSCoreBaseCharacter::Server_SetVisibleMesh_Implementation(USkeletalMesh *NewVisibleMesh)
{
	SetVisibleMesh(NewVisibleMesh);
}

void ATPSCoreBaseCharacter::RagdollUpdate(float DeltaTime)
{
	GetMesh()->bOnlyAllowAutonomousTickPose = false;

	// Set the Last Ragdoll Velocity.
	const FVector NewRagdollVel = GetMesh()->GetPhysicsLinearVelocity(NAME_root);
	LastRagdollVelocity =
		(NewRagdollVel != FVector::ZeroVector || IsLocallyControlled()) ? NewRagdollVel : LastRagdollVelocity / 2;

	// Use the Ragdoll Velocity to scale the ragdoll's joint strength for physical animation.
	const float SpringValue =
		FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f}, LastRagdollVelocity.Size());
	GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual acceleration.
	// This also prevents the ragdoll from going through the floor.
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	GetMesh()->SetEnableGravity(bEnableGrav);

	// Update the Actor location to follow the ragdoll.
	SetActorLocationDuringRagdoll(DeltaTime);
}

void ATPSCoreBaseCharacter::SetActorLocationDuringRagdoll(float DeltaTime)
{
	if (IsLocallyControlled())
	{
		// Set the pelvis as the target location.
		TargetRagdollLocation = GetMesh()->GetSocketLocation(NAME_Pelvis);
		if (!HasAuthority())
		{
			Server_SetMeshLocationDuringRagdoll(TargetRagdollLocation);
		}
	}

	// Determine whether the ragdoll is facing up or down and set the target rotation accordingly.
	const FRotator PelvisRot = GetMesh()->GetSocketRotation(NAME_Pelvis);

	if (bReversedPelvis)
	{
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	}
	else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}


	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);

	// Trace downward from the target location to offset the target location,
	// preventing the lower half of the capsule from going through the floor when the ragdoll is laying on the ground.
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
							TargetRagdollLocation.Z - GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	UWorld *World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult HitResult;
	const bool bHit =
		World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect, ECC_Visibility, Params);

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	if (!IsLocallyControlled())
	{
		ServerRagdollPull = FMath::FInterpTo(ServerRagdollPull, 750.0f, DeltaTime, 0.6f);
		float RagdollSpeed = FVector(LastRagdollVelocity.X, LastRagdollVelocity.Y, 0).Size();
		FName RagdollSocketPullName = RagdollSpeed > 300 ? NAME_spine_03 : NAME_pelvis;
		GetMesh()->AddForce((TargetRagdollLocation - GetMesh()->GetSocketLocation(RagdollSocketPullName)) *
								ServerRagdollPull,
							RagdollSocketPullName, true);
	}
	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void ATPSCoreBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	// Use the Character Movement Mode changes to set the Movement States to the right values. This allows you to have
	// a custom set of movement states but still use the functionality of the default character movement component.

	if (GetCharacterMovement()->MovementMode == MOVE_Walking || GetCharacterMovement()->MovementMode == MOVE_NavWalking)
	{
		SetMovementState(ETPSMovementState::Grounded);
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		SetMovementState(ETPSMovementState::InAir);
	}
	// else if(GetCharacterMovement()->MovementMode == MOVE_Swimming)
	// {
	// 	SetMovementState(ETPSMovementState::Swimming);
	// }
}

void ATPSCoreBaseCharacter::OnMovementStateChanged(const ETPSMovementState PreviousState)
{
	if (MovementState == ETPSMovementState::InAir)
	{
		if (MovementAction == ETPSMovementAction::None)
		{
			// If the character enters the air, set the In Air Rotation and uncrouch if crouched.
			InAirRotation = GetActorRotation();
			if (Stance == ETPSStance::Crouching)
			{
				UnCrouch();
			}
		}
		else if (MovementAction == ETPSMovementAction::Rolling)
		{
			// If the character is currently rolling, enable the ragdoll.
			ReplicatedRagdollStart();
		}
	}
}

void ATPSCoreBaseCharacter::OnMovementActionChanged(const ETPSMovementAction PreviousAction)
{
	// Make the character crouch if performing a roll.
	if (MovementAction == ETPSMovementAction::Rolling)
	{
		Crouch();
	}

	if (PreviousAction == ETPSMovementAction::Rolling)
	{
		if (DesiredStance == ETPSStance::Standing)
		{
			UnCrouch();
		}
		else if (DesiredStance == ETPSStance::Crouching)
		{
			Crouch();
		}
	}
}

void ATPSCoreBaseCharacter::OnStanceChanged(const ETPSStance PreviousStance)
{
	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void ATPSCoreBaseCharacter::OnRotationModeChanged(ETPSRotationMode PreviousRotationMode)
{
	if (RotationMode == ETPSRotationMode::VelocityDirection && ViewMode == ETPSViewMode::FirstPerson)
	{
		// If the new rotation mode is Velocity Direction and the character is in First Person,
		// set the viewmode to Third Person.
		SetViewMode(ETPSViewMode::ThirdPerson);
	}
	MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
}

void ATPSCoreBaseCharacter::OnViewModeChanged(const ETPSViewMode PreviousViewMode)
{
	if (ViewMode == ETPSViewMode::ThirdPerson)
	{
		if (RotationMode == ETPSRotationMode::VelocityDirection || RotationMode == ETPSRotationMode::LookingDirection)
		{
			// If Third Person, set the rotation mode back to the desired mode.
			SetRotationMode(DesiredRotationMode);
		}
	}
	else if (ViewMode == ETPSViewMode::FirstPerson && RotationMode == ETPSRotationMode::VelocityDirection)
	{
		// If First Person, set the rotation mode to looking direction if currently in the velocity direction mode.
		SetRotationMode(ETPSRotationMode::LookingDirection);
	}
}

void ATPSCoreBaseCharacter::OnOverlayStateChanged(const ETPSOverlayState PreviousState) {}

void ATPSCoreBaseCharacter::OnVisibleMeshChanged(const USkeletalMesh *PrevVisibleMesh)
{
	// Update the Skeletal Mesh before we update materials and anim bp variables
	GetMesh()->SetSkeletalMesh(VisibleMesh);

	// Reset materials to their new mesh defaults
	if (GetMesh() != nullptr)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < GetMesh()->GetNumMaterials(); ++MaterialIndex)
		{
			GetMesh()->SetMaterial(MaterialIndex, nullptr);
		}
	}

	// Force set variables. This ensures anim instance & character stay synchronized on mesh changes
	ForceUpdateCharacterState();
}

void ATPSCoreBaseCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(ETPSStance::Crouching);
}

void ATPSCoreBaseCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(ETPSStance::Standing);
}

void ATPSCoreBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (IsLocallyControlled())
	{
		EventOnJumped();
	}
	if (HasAuthority())
	{
		Multicast_OnJumped();
	}
}

void ATPSCoreBaseCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);

	// if (IsLocallyControlled())
	// {
	// 	EventOnLanded();
	// }
	// if (HasAuthority())
	// {
	// 	Multicast_OnLanded();
	// }
}

void ATPSCoreBaseCharacter::OnLandFrictionReset()
{
	// Reset the braking friction
	GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}

void ATPSCoreBaseCharacter::SetEssentialValues(float DeltaTime)
{
	if (GetLocalRole() != ROLE_SimulatedProxy)
	{
		ReplicatedCurrentAcceleration = GetCharacterMovement()->GetCurrentAcceleration();
		ReplicatedControlRotation = GetControlRotation();
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		EasedMaxAcceleration = GetCharacterMovement()->GetMaxAcceleration() != 0
			? GetCharacterMovement()->GetMaxAcceleration()
			: EasedMaxAcceleration / 2;
	}

	// Interp AimingRotation to current control rotation for smooth character rotation movement. Decrease InterpSpeed
	// for slower but smoother movement.
	AimingRotation = FMath::RInterpTo(AimingRotation, ReplicatedControlRotation, DeltaTime, 30);

	// These values represent how the capsule is moving as well as how it wants to move, and therefore are essential
	// for any data driven animation system. They are also used throughout the system for various functions,
	// so I found it is easiest to manage them all in one place.

	const FVector CurrentVel = GetVelocity();

	// Set the amount of Acceleration.
	const FVector NewAcceleration = (CurrentVel - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || IsLocallyControlled() ? NewAcceleration : Acceleration / 2;

	// Determine if the character is moving by getting it's speed. The Speed equals the length of the horizontal (x y)
	// velocity, so it does not take vertical movement into account. If the character is moving, update the last
	// velocity rotation. This value is saved because it might be useful to know the last orientation of movement
	// even after the character has stopped.
	Speed = CurrentVel.Size2D();
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}

	// Determine if the character has movement input by getting its movement input amount.
	// The Movement Input Amount is equal to the current acceleration divided by the max acceleration so that
	// it has a range of 0-1, 1 being the maximum possible amount of input, and 0 being none.
	// If the character has movement input, update the Last Movement Input Rotation.
	MovementInputAmount = ReplicatedCurrentAcceleration.Size() / EasedMaxAcceleration;
	bHasMovementInput = MovementInputAmount > 0.0f;
	if (bHasMovementInput)
	{
		LastMovementInputRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
	}

	// Set the Aim Yaw rate by comparing the current and previous Aim Yaw value, divided by Delta Seconds.
	// This represents the speed the camera is rotating left to right.
	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
}

void ATPSCoreBaseCharacter::UpdateCharacterMovement()
{
	// Set the Allowed Gait
	const ETPSGait AllowedGait = GetAllowedGait();
	
	// Determine the Actual Gait. If it is different from the current Gait, Set the new Gait Event.
	const ETPSGait ActualGait = GetActualGait(AllowedGait);
	
	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}
	
	// // Update the Character Max Walk Speed to the configured speeds based on the currently Allowed Gait.
	MyCharacterMovementComponent->SetAllowedGait(AllowedGait);
}

void ATPSCoreBaseCharacter::UpdateGroundedRotation(float DeltaTime)
{
	if (MovementAction == ETPSMovementAction::None)
	{
		const bool bCanUpdateMovingRot = ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !HasAnyRootMotion();
		if (bCanUpdateMovingRot)
		{
			const float GroundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == ETPSRotationMode::VelocityDirection)
			{
				// Velocity Direction Rotation
				SmoothCharacterRotation({0.0f, LastVelocityRotation.Yaw, 0.0f}, 800.0f, GroundedRotationRate,
										DeltaTime);
			}
			else if (RotationMode == ETPSRotationMode::LookingDirection)
			{
				// Looking Direction Rotation
				float YawValue;
				if (Gait == ETPSGait::Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					// Walking or Running..
					const float YawOffsetCurveVal = GetAnimCurveValue(NAME_YawOffset);
					YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				}
				SmoothCharacterRotation({0.0f, YawValue, 0.0f}, 500.0f, GroundedRotationRate, DeltaTime);
			}
			else if (RotationMode == ETPSRotationMode::Aiming)
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({0.0f, ControlYaw, 0.0f}, 1000.0f, 20.0f, DeltaTime);
			}
		}
		else
		{
			// Not Moving
	
			if ((ViewMode == ETPSViewMode::ThirdPerson && RotationMode == ETPSRotationMode::Aiming) ||
				ViewMode == ETPSViewMode::FirstPerson)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
			}
	
			// Apply the RotationAmount curve from Turn In Place Animations.
			// The Rotation Amount curve defines how much rotation should be applied each frame,
			// and is calculated for animations that are animated at 30fps.
	
			const float RotAmountCurve = GetAnimCurveValue(NAME_RotationAmount);
	
			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetLocalRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					SetActorRotation(TargetRotation);
				}
				else
				{
					AddActorWorldRotation({0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0});
				}
				TargetRotation = GetActorRotation();
			}
		}
	}
	else if (MovementAction == ETPSMovementAction::Rolling)
	{
		// Rolling Rotation (Not allowed on networked games)
		if (!bEnableNetworkOptimizations && bHasMovementInput)
		{
			SmoothCharacterRotation({0.0f, LastMovementInputRotation.Yaw, 0.0f}, 0.0f, 2.0f, DeltaTime);
		}
	}

	// Other actions are ignored...
}

void ATPSCoreBaseCharacter::UpdateInAirRotation(float DeltaTime)
{
	if (RotationMode == ETPSRotationMode::VelocityDirection || RotationMode == ETPSRotationMode::LookingDirection)
	{
		// Velocity / Looking Direction Rotation
		SmoothCharacterRotation({0.0f, InAirRotation.Yaw, 0.0f}, 0.0f, 5.0f, DeltaTime);
	}
	else if (RotationMode == ETPSRotationMode::Aiming)
	{
		// Aiming Rotation
		SmoothCharacterRotation({0.0f, AimingRotation.Yaw, 0.0f}, 0.0f, 15.0f, DeltaTime);
		InAirRotation = GetActorRotation();
	}
}

ETPSGait ATPSCoreBaseCharacter::GetAllowedGait() const
{
	// Calculate the Allowed Gait. This represents the maximum Gait the character is currently allowed to be in,
	// and can be determined by the desired gait, the rotation mode, the stance, etc. For example,
	// if you wanted to force the character into a walking state while indoors, this could be done here.

	if (Stance == ETPSStance::Standing)
	{
		if (RotationMode != ETPSRotationMode::Aiming)
		{
			if (DesiredGait == ETPSGait::Sprinting)
			{
				return CanSprint() ? ETPSGait::Sprinting : ETPSGait::Running;
			}
			return DesiredGait;
		}
	}

	// Crouching stance & Aiming rot mode has same behaviour

	if (DesiredGait == ETPSGait::Sprinting)
	{
		return ETPSGait::Running;
	}

	return DesiredGait;
}

ETPSGait ATPSCoreBaseCharacter::GetActualGait(ETPSGait AllowedGait) const
{
	// Get the Actual Gait. This is calculated by the actual movement of the character,  and so it can be different
	// from the desired gait or allowed gait. For instance, if the Allowed Gait becomes walking,
	// the Actual gait will still be running until the character decelerates to the walking speed.

	const float LocWalkSpeed = MyCharacterMovementComponent->CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = MyCharacterMovementComponent->CurrentMovementSettings.RunSpeed;

	if (Speed > LocRunSpeed + 10.0f)
	{
		if (AllowedGait == ETPSGait::Sprinting)
		{
			return ETPSGait::Sprinting;
		}
		return ETPSGait::Running;
	}
	
	if (Speed >= LocWalkSpeed + 10.0f)
	{
		return ETPSGait::Running;
	}

	return ETPSGait::Walking;
}

void ATPSCoreBaseCharacter::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed,
												float DeltaTime)
{
	// Interpolate the Target Rotation for extra smooth rotation behavior
	TargetRotation = FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}

float ATPSCoreBaseCharacter::CalculateGroundedRotationRate() const
{
	// Calculate the rotation rate by using the current Rotation Rate Curve in the Movement Settings.
	// Using the curve in conjunction with the mapped speed gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	const float MappedSpeedVal = MyCharacterMovementComponent->GetMappedSpeed();
	const float CurveVal = MyCharacterMovementComponent->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 300.0f}, {1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

void ATPSCoreBaseCharacter::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime)
{
	// Prevent the character from rotating past a certain angle.
	FRotator Delta = AimingRotation - GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;

	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({0.0f, TargetYaw, 0.0f}, 0.0f, InterpSpeed, DeltaTime);
	}
}

void ATPSCoreBaseCharacter::RagdollAction_Implementation()
{
	// Ragdoll Action: Press "Ragdoll Action" to toggle the ragdoll state on or off.

	if (MovementState == ETPSMovementState::Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
	else
	{
		ReplicatedRagdollStart();
	}
}

void ATPSCoreBaseCharacter::VelocityDirectionAction_Implementation()
{
	// Select Rotation Mode: Switch the desired (default) rotation mode to Velocity or Looking Direction.
	// This will be the mode the character reverts back to when un-aiming
	SetDesiredRotationMode(ETPSRotationMode::VelocityDirection);
	SetRotationMode(ETPSRotationMode::VelocityDirection);
}

void ATPSCoreBaseCharacter::LookingDirectionAction_Implementation()
{
	SetDesiredRotationMode(ETPSRotationMode::LookingDirection);
	SetRotationMode(ETPSRotationMode::LookingDirection);
}

void ATPSCoreBaseCharacter::ReplicatedRagdollStart()
{
	if (HasAuthority())
	{
		Multicast_RagdollStart();
	}
	else
	{
		Server_RagdollStart();
	}
}

void ATPSCoreBaseCharacter::ReplicatedRagdollEnd()
{
	if (HasAuthority())
	{
		Multicast_RagdollEnd(GetActorLocation());
	}
	else
	{
		Server_RagdollEnd(GetActorLocation());
	}
}

void ATPSCoreBaseCharacter::OnRep_RotationMode(ETPSRotationMode PrevRotMode) { OnRotationModeChanged(PrevRotMode); }

void ATPSCoreBaseCharacter::OnRep_ViewMode(ETPSViewMode PrevViewMode) { OnViewModeChanged(PrevViewMode); }

void ATPSCoreBaseCharacter::OnRep_OverlayState(ETPSOverlayState PrevOverlayState)
{
	OnOverlayStateChanged(PrevOverlayState);
}

void ATPSCoreBaseCharacter::OnRep_VisibleMesh(const USkeletalMesh *PreviousSkeletalMesh)
{
	OnVisibleMeshChanged(PreviousSkeletalMesh);
}
