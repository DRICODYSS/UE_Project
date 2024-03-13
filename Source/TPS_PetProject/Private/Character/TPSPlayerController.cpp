// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPSPlayerController.h"


#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Character/Core_TPSBaseCharacter.h"
#include "Character/TPS_BaseCharacter.h"
#include "GameFramework/PlayerInput.h"


void ATPSPlayerController::SetPawn(APawn *InPawn)
{
	Super::SetPawn(InPawn);
	PossessedCharacter = Cast<ATPS_BaseCharacter>(InPawn);
}

void ATPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(PossessedCharacter);
}

void ATPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ATPSPlayerController::ForwardMovementAction);
	InputComponent->BindAxis("MoveRight",   this, &ATPSPlayerController::RightMovementAction);
	
	InputComponent->BindAxis("Turn",   this, &ATPSPlayerController::CameraRightAction);
	InputComponent->BindAxis("LookUp", this, &ATPSPlayerController::CameraUpAction);

	InputComponent->BindAxis("SwimForward", this, &ATPSPlayerController::ForwardSwimmingAction);
	InputComponent->BindAxis("SwimUp",      this, &ATPSPlayerController::SwimmingUpAction);

	InputComponent->BindAxis("ClimbLadderUp", this, &ATPSPlayerController::ClimbLadderUpAction);
	

	InputComponent->BindAction("Aim",  IE_Pressed, this, &ATPSPlayerController::StartAim);
	InputComponent->BindAction("Aim", IE_Released, this, &ATPSPlayerController::StopAim);

	InputComponent->BindAction("Slide",  IE_Pressed, this, &ATPSPlayerController::PressedSlide);
	InputComponent->BindAction("Slide", IE_Released, this, &ATPSPlayerController::ReleasedSlide);
	
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ATPSPlayerController::StanceAction);
	
	InputComponent->BindAction("Sprint",  IE_Pressed, this, &ATPSPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ATPSPlayerController::StopSprint);

	
	
	InputComponent->BindAction("InteractWithLadder", IE_Pressed, this, &ATPSPlayerController::InteractWithLadder);
	
	InputComponent->BindAction("Jump",  IE_Pressed, this, &ATPSPlayerController::StartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ATPSPlayerController::StopJump);

	InputComponent->BindAction("Mantle", IE_Pressed, this, &ATPSPlayerController::Mantle);

	InputComponent->BindAction("WallRun", IE_Pressed, this, &ATPSPlayerController::WallRun);
}

/* Axis */
void ATPSPlayerController::ForwardMovementAction(float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->ForwardMovementAction(Value); }
void ATPSPlayerController::RightMovementAction  (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->RightMovementAction(Value);   }

void ATPSPlayerController::CameraRightAction    (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->CameraRightAction(Value);     }
void ATPSPlayerController::CameraUpAction       (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->CameraUpAction(Value);        }

void ATPSPlayerController::ForwardSwimmingAction(float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->ForwardSwimmingAction(Value); }
void ATPSPlayerController::RightSwimmingAction  (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->RightSwimmingAction(Value);   }

void ATPSPlayerController::SwimmingUpAction     (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->SwimmingUpAction(Value);      }

void ATPSPlayerController::ClimbLadderUpAction  (float Value) { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->ClimbLadderUpAction(Value);   }
/* Axis_end */

/* Action */
void ATPSPlayerController::StartAim(){ if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Aim(true);  }
void ATPSPlayerController::StopAim() { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Aim(false); }

void ATPSPlayerController::StanceAction() { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Crouch(); }

void ATPSPlayerController::StartSprint() { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Sprint(true);  }
void ATPSPlayerController::StopSprint()  { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Sprint(false); }

void ATPSPlayerController::PressedSlide()  { SetTimer_HOLD(SlideHoldTime); }
void ATPSPlayerController::ReleasedSlide() { ClearTimer_HOLD(); }
void ATPSPlayerController::Slide()
{
	if (!IsValid(PossessedCharacter)) return;
	
	ClearTimer_HOLD();
	PossessedCharacter->TryChangeState_Slide(true);
}

void ATPSPlayerController::InteractWithLadder() { if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_InteractWithLadder(true); }

void ATPSPlayerController::StartJump() {  if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Jump(true);  }
void ATPSPlayerController::StopJump()  {  if (!IsValid(PossessedCharacter)) return; PossessedCharacter->TryChangeState_Jump(false); }

void ATPSPlayerController::Mantle()
{
	if (!IsValid(PossessedCharacter)) return;
	
	if (PossessedCharacter->GetMovementState() != ETPSMovementState::Mantling)
		PossessedCharacter->TryChangeState_Mantle(true);
}

void ATPSPlayerController::WallRun()
{
	if (!IsValid(PossessedCharacter)) return;
	
	if (PossessedCharacter->GetMovementState() != ETPSMovementState::WallRunning)
		PossessedCharacter->TryChangeState_WallRun(true);
}

/* Action_end */

/* Tools */
void ATPSPlayerController::SetTimer_HOLD(const float& Time)
{
	GetWorld()->GetTimerManager().SetTimer(HOLD_Timer, this, &ATPSPlayerController::Slide, Time, false);
}

void ATPSPlayerController::ClearTimer_HOLD()
{
	GetWorld()->GetTimerManager().ClearTimer(HOLD_Timer);
}