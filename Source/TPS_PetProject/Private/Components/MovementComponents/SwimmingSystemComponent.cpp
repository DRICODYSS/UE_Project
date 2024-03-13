// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/SwimmingSystemComponent.h"

#include "Actors/Water/BaseWaterPool.h"
#include "Character/TPS_BaseCharacter.h"


USwimmingSystemComponent::USwimmingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}



void USwimmingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedBaseCharacterOwner = CastChecked<ATPS_BaseCharacter>(GetOwner());
}

bool USwimmingSystemComponent::RegisterInteractiveActor(ABaseWaterPool *WaterVolume)
{
	if(IsValid(AvailableWaterVolume)) return false;

	AvailableWaterVolume = WaterVolume;
	return true;
}

void USwimmingSystemComponent::UnregisterInteractiveActor()
{
	AvailableWaterVolume = nullptr;
	GetWorld()->GetTimerManager().ClearTimer(GroundedToWaterTimer);
	GetWorld()->GetTimerManager().ClearTimer(InAirToWaterTimer);
}


void USwimmingSystemComponent::StartSwimmingLogic()
{
	if(!IsValid(AvailableWaterVolume)) return;

	if(CachedBaseCharacterOwner->GetMovementState() != ETPSMovementState::Grounded)
	{
		bInAir = true;
		GetWorld()->GetTimerManager().SetTimer(InAirToWaterTimer, this, &USwimmingSystemComponent::InAirToWaterLogic, LogicTickTimer, true);
	}

	GetWorld()->GetTimerManager().SetTimer(GroundedToWaterTimer, this, &USwimmingSystemComponent::GroundedToWaterLogic, LogicTickTimer, true);
	if(bInAir) GetWorld()->GetTimerManager().PauseTimer(GroundedToWaterTimer);
}

void USwimmingSystemComponent::PauseSwimAction() const
{
	GetWorld()->GetTimerManager().UnPauseTimer(GroundedToWaterTimer);
	SwimAction(false);
}


void USwimmingSystemComponent::GroundedToWaterLogic()
{
	if(CachedBaseCharacterOwner->GetMesh()->GetBoneLocation("neck_01").Z <= GetWaterSurfaceZ())
	{
		GetWorld()->GetTimerManager().PauseTimer(GroundedToWaterTimer);
		StartCheckPauseSwim();
		SwimAction(true);
		
		FixCharacterPosition();
	}
	else if(CachedBaseCharacterOwner->GetMovementState() != ETPSMovementState::Grounded)
	{
		GetWorld()->GetTimerManager().PauseTimer(GroundedToWaterTimer);
		StartCheckPauseSwim();
		SwimAction(true);
		
		bUnderWater = true;
	}
}

void USwimmingSystemComponent::InAirToWaterLogic()
{
	if(CachedBaseCharacterOwner->GetCapsuleLocation().Z + FixCharacterLocationUnderWater < GetWaterSurfaceZ())
	{
		GetWorld()->GetTimerManager().ClearTimer(InAirToWaterTimer);
		StartCheckPauseSwim();
		SwimAction(true);
		
		bUnderWater = true;
		bInAir = false;
	}
}

void USwimmingSystemComponent::StartCheckPauseSwim()
{
	GetWorld()->GetTimerManager().SetTimer(CheckPauseSwimTimer, this, &USwimmingSystemComponent::CheckPauseSwim, PauseSwimTimerTick, true);
}

void USwimmingSystemComponent::CheckPauseSwim()
{
	if(CachedBaseCharacterOwner->GetCapsuleLocation().Z > GetWaterSurfaceZ() - 10.f && !bUnderWater)
	{
		GetWorld()->GetTimerManager().ClearTimer(CheckPauseSwimTimer);
		PauseSwimAction();
	}
}

void USwimmingSystemComponent::SwimAction(bool bValue) const
{
	AvailableWaterVolume->ChangeWaterPhysicsVolumeParam(bValue);
	CachedBaseCharacterOwner->TryChangeState_Swim(bValue);
}

float USwimmingSystemComponent::GetWaterSurfaceZ() const
{
	return AvailableWaterVolume->GetWaterSurfaceLocationZ();
}

const ABaseWaterPool * USwimmingSystemComponent::GetAvailableWaterVolume() const
{
	return AvailableWaterVolume;
}

void USwimmingSystemComponent::FixCharacterPosition() 
{
	bUnderWater = false;
	FVector NewCharacterLocation = CachedBaseCharacterOwner->GetCapsuleLocation();
	NewCharacterLocation.Z = GetWaterSurfaceZ() + FixCharacterLocationOnSurface;
	CachedBaseCharacterOwner->SetCapsuleWorldLocation(NewCharacterLocation);
}