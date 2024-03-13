// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPS_PlayerCharacter.h"

#include "Actors/Interactive/Environment/Ladder.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/MovementComponents/SwimmingSystemComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"


ATPS_PlayerCharacter::ATPS_PlayerCharacter(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;
	
	SpringArmComponent->bEnableCameraLag = 1;
	SpringArmComponent->CameraLagSpeed = 5.f;
	SpringArmComponent->CameraLagMaxDistance = 50.f;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	
}

void ATPS_PlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(11, 20.0f, FColor::Yellow, FString::Printf(TEXT("X %.f Y %.f Z %.f"), GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z));
	GEngine->AddOnScreenDebugMessage(10, 20.0f, FColor::Yellow, FString::Printf(TEXT("Pitch %.f Yaw %.f Roll %.f"), GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll));
}

void ATPS_PlayerCharacter::ForwardMovementAction(const float Value)
{
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("Speed: %.f"), GetSpeed()));
	if (!GetMovementComponent()->IsSwimming() && (MovementState == ETPSMovementState::Grounded || MovementState == ETPSMovementState::InAir) && MovementState != ETPSMovementState::OnLadder)
	{
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void ATPS_PlayerCharacter::RightMovementAction(const float Value)
{
	GEngine->AddOnScreenDebugMessage(2, 1.0f, FColor::Blue, FString::Printf(TEXT("Speed: %.f"), GetSpeed()));
	if (!GetMovementComponent()->IsSwimming() && (MovementState == ETPSMovementState::Grounded || MovementState == ETPSMovementState::InAir) && MovementState != ETPSMovementState::OnLadder)
	{
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
	}
}

void ATPS_PlayerCharacter::CameraUpAction(const float Value)
{
	SetUpCameraValue(Value);
	AddControllerPitchInput(LookUpDownRate * Value);
}

void ATPS_PlayerCharacter::CameraRightAction(const float Value)
{
	SetTurnCameraValue(Value);
	AddControllerYawInput(LookLeftRightRate * Value);
}

void ATPS_PlayerCharacter::ForwardSwimmingAction(const float Value)
{
	if(!GetMovementComponent()->IsSwimming() || Value <= 0.f)
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		return;
	}
	
	ForwardSwimmingActionLogic(Value);
}

void ATPS_PlayerCharacter::SwimmingUpAction(const float Value)
{
	if(!GetMovementComponent()->IsSwimming()) return;
	SwimmingUpActionLogic(Value);
}

void ATPS_PlayerCharacter::ClimbLadderUpAction(float Value)
{
	if(!FMath::IsNearlyZero(Value) && MovementState == ETPSMovementState::OnLadder)
	{
		FVector LadderUpVector = GetMyMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void ATPS_PlayerCharacter::ForwardSwimmingActionLogic(float Value)
{
	if(!SwimmingSystemComponent->IsUnderWater())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
	else
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		const FRotator DirRotator(GetControlRotation().Pitch, AimingRotation.Yaw, 0.0f);
		AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void ATPS_PlayerCharacter::SwimmingUpActionLogic(float Value)
{
	if(SwimmingSystemComponent->IsUnderWater())
	{
		AddMovementInput(FVector::UpVector, Value);
	}
	else if(Value < 0.f && !SwimmingSystemComponent->IsUnderWater())
	{
		AddMovementInput(FVector::UpVector, Value);
		SwimmingSystemComponent->SetIsUnderWater(true);
	}
	if(GetCapsuleComponent()->GetComponentLocation().Z + 15.f >= SwimmingSystemComponent->GetWaterSurfaceZ())
	{
		SwimmingSystemComponent->FixCharacterPosition(); 
	}
}
