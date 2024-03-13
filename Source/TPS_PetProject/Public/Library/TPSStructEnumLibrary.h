// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/TPSCharacterEnumLibrary.h"

#include "TPSStructEnumLibrary.generated.h"


USTRUCT(BlueprintType)
struct FTPSMovementState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	ETPSMovementState State = ETPSMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Grounded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool InAir_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Mantling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Ragdoll_ = false;

public:
	FTPSMovementState() {}

	FTPSMovementState(const ETPSMovementState InitialState) { *this = InitialState; }

	const bool &None() const { return None_; }
	const bool &Grounded() const { return Grounded_; }
	const bool &InAir() const { return InAir_; }
	const bool &Mantling() const { return Mantling_; }
	const bool &Ragdoll() const { return Ragdoll_; }

	operator ETPSMovementState() const { return State; }

	void operator=(const ETPSMovementState NewState)
	{
		State = NewState;
		None_ = State == ETPSMovementState::None;
		Grounded_ = State == ETPSMovementState::Grounded;
		InAir_ = State == ETPSMovementState::InAir;
		Mantling_ = State == ETPSMovementState::Mantling;
		Ragdoll_ = State == ETPSMovementState::Ragdoll;
	}
};

USTRUCT(BlueprintType)
struct FTPSStance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	ETPSStance Stance = ETPSStance::Standing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Standing_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Crouching_ = false;

public:
	FTPSStance() {}

	FTPSStance(const ETPSStance InitialStance) { *this = InitialStance; }

	const bool &Standing() const { return Standing_; }
	const bool &Crouching() const { return Crouching_; }

	operator ETPSStance() const { return Stance; }

	void operator=(const ETPSStance NewStance)
	{
		Stance = NewStance;
		Standing_ = Stance == ETPSStance::Standing;
		Crouching_ = Stance == ETPSStance::Crouching;
	}
};

USTRUCT(BlueprintType)
struct FTPSRotationMode
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Rotation System")
	ETPSRotationMode RotationMode = ETPSRotationMode::VelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Rotation System")
	bool VelocityDirection_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Rotation System")
	bool LookingDirection_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Rotation System")
	bool Aiming_ = false;

public:
	FTPSRotationMode() {}

	FTPSRotationMode(const ETPSRotationMode InitialRotationMode) { *this = InitialRotationMode; }

	const bool &VelocityDirection() const { return VelocityDirection_; }
	const bool &LookingDirection() const { return LookingDirection_; }
	const bool &Aiming() const { return Aiming_; }

	operator ETPSRotationMode() const { return RotationMode; }

	void operator=(const ETPSRotationMode NewRotationMode)
	{
		RotationMode = NewRotationMode;
		VelocityDirection_ = RotationMode == ETPSRotationMode::VelocityDirection;
		LookingDirection_ = RotationMode == ETPSRotationMode::LookingDirection;
		Aiming_ = RotationMode == ETPSRotationMode::Aiming;
	}
};

USTRUCT(BlueprintType)
struct FTPSMovementDirection
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	ETPSMovementDirection MovementDirection = ETPSMovementDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Forward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Right_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Left_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Backward_ = false;

public:
	FTPSMovementDirection() {}

	FTPSMovementDirection(const ETPSMovementDirection InitialMovementDirection) { *this = InitialMovementDirection; }

	const bool &Forward() const { return Forward_; }
	const bool &Right() const { return Right_; }
	const bool &Left() const { return Left_; }
	const bool &Backward() const { return Backward_; }

	operator ETPSMovementDirection() const { return MovementDirection; }

	void operator=(const ETPSMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;
		Forward_ = MovementDirection == ETPSMovementDirection::Forward;
		Right_ = MovementDirection == ETPSMovementDirection::Right;
		Left_ = MovementDirection == ETPSMovementDirection::Left;
		Backward_ = MovementDirection == ETPSMovementDirection::Backward;
	}
};

USTRUCT(BlueprintType)
struct FTPSMovementAction
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	ETPSMovementAction Action = ETPSMovementAction::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool LowMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool HighMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Rolling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool GettingUp_ = false;

public:
	FTPSMovementAction() {}

	FTPSMovementAction(const ETPSMovementAction InitialAction) { *this = InitialAction; }

	const bool &None() const { return None_; }
	const bool &LowMantle() const { return LowMantle_; }
	const bool &HighMantle() const { return HighMantle_; }
	const bool &Rolling() const { return Rolling_; }
	const bool &GettingUp() const { return GettingUp_; }

	operator ETPSMovementAction() const { return Action; }

	void operator=(const ETPSMovementAction NewAction)
	{
		Action = NewAction;
		None_ = Action == ETPSMovementAction::None;
		LowMantle_ = Action == ETPSMovementAction::LowMantle;
		HighMantle_ = Action == ETPSMovementAction::HighMantle;
		Rolling_ = Action == ETPSMovementAction::Rolling;
		GettingUp_ = Action == ETPSMovementAction::GettingUp;
	}
};

USTRUCT(BlueprintType)
struct FTPSGait
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	ETPSGait Gait = ETPSGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Walking_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Running_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Movement System")
	bool Sprinting_ = false;

public:
	FTPSGait() {}

	FTPSGait(const ETPSGait InitialGait) { *this = InitialGait; }

	const bool &Walking() const { return Walking_; }
	const bool &Running() const { return Running_; }
	const bool &Sprinting() const { return Sprinting_; }

	operator ETPSGait() const { return Gait; }

	void operator=(const ETPSGait NewGait)
	{
		Gait = NewGait;
		Walking_ = Gait == ETPSGait::Walking;
		Running_ = Gait == ETPSGait::Running;
		Sprinting_ = Gait == ETPSGait::Sprinting;
	}
};

USTRUCT(BlueprintType)
struct FTPSOverlayState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	ETPSOverlayState State = ETPSOverlayState::Default;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Default_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Masculine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Feminine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Injured_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool HandsTied_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Rifle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool PistolOneHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool PistolTwoHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Bow_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Torch_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Binoculars_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Box_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Character States")
	bool Barrel_ = false;

public:
	FTPSOverlayState() {}

	FTPSOverlayState(const ETPSOverlayState InitialState) { *this = InitialState; }

	const bool &Default() const { return Default_; }
	const bool &Masculine() const { return Masculine_; }
	const bool &Feminine() const { return Feminine_; }
	const bool &Injured() const { return Injured_; }
	const bool &HandsTied() const { return HandsTied_; }
	const bool &Rifle() const { return Rifle_; }
	const bool &PistolOneHanded() const { return PistolOneHanded_; }
	const bool &PistolTwoHanded() const { return PistolTwoHanded_; }
	const bool &Bow() const { return Bow_; }
	const bool &Torch() const { return Torch_; }
	const bool &Binoculars() const { return Binoculars_; }
	const bool &Box() const { return Box_; }
	const bool &Barrel() const { return Barrel_; }

	operator ETPSOverlayState() const { return State; }

	void operator=(const ETPSOverlayState NewAction)
	{
		State = NewAction;
		Default_ = State == ETPSOverlayState::Default;
		Masculine_ = State == ETPSOverlayState::Masculine;
		Feminine_ = State == ETPSOverlayState::Feminine;
		Injured_ = State == ETPSOverlayState::Injured;
		HandsTied_ = State == ETPSOverlayState::HandsTied;
		Rifle_ = State == ETPSOverlayState::Rifle;
		PistolOneHanded_ = State == ETPSOverlayState::PistolOneHanded;
		PistolTwoHanded_ = State == ETPSOverlayState::PistolTwoHanded;
		Bow_ = State == ETPSOverlayState::Bow;
		Torch_ = State == ETPSOverlayState::Torch;
		Binoculars_ = State == ETPSOverlayState::Binoculars;
		Box_ = State == ETPSOverlayState::Box;
		Barrel_ = State == ETPSOverlayState::Barrel;
	}
};

USTRUCT(BlueprintType)
struct FTPSGroundedEntryState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Breakfall System")
	ETPSGroundedEntryState State = ETPSGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Breakfall System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true),
			  Category = "ALS|Breakfall System")
	bool Roll_ = false;

public:
	FTPSGroundedEntryState() {}

	FTPSGroundedEntryState(const ETPSGroundedEntryState InitialState) { *this = InitialState; }

	const bool &None() const { return None_; }
	const bool &Roll() const { return Roll_; }

	operator ETPSGroundedEntryState() const { return State; }

	void operator=(const ETPSGroundedEntryState NewState)
	{
		State = NewState;
		None_ = State == ETPSGroundedEntryState::None;
		Roll_ = State == ETPSGroundedEntryState::Roll;
	}
};
