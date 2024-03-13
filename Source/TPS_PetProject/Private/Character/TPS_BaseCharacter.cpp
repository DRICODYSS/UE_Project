// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TPS_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/TPS_AbilitySystemComponent.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/MovementComponents/LedgeDetectorComponent.h"
#include "Components/MovementComponents/RunnableWallDetectorComponent.h"
#include "Components/MovementComponents/SwimmingSystemComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"


ATPS_BaseCharacter::ATPS_BaseCharacter(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
	//MyCharacterMovementComponent = StaticCast<UTPSCharacterMovementComponent*>(GetCharacterMovement());

	AbilitySystemComponent = CreateDefaultSubobject<UTPS_AbilitySystemComponent>("AbilitySystemComponent");
	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>("AttributesComponent");
	SwimmingSystemComponent = CreateDefaultSubobject<USwimmingSystemComponent>("SwimmingSystemComponent");
	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>("LedgeDetectorComponent");
	RunnableWallDetectorComponent = CreateDefaultSubobject<URunnableWallDetectorComponent>("RunnableWallDetectorComponent");
}

void ATPS_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributesComponent->OnDeathEvent.AddUObject(this, &ATPS_BaseCharacter::OnDeath);
}

void ATPS_BaseCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
	InitAbilitySystemComponent(NewController);
}

//Interactive actor
void ATPS_BaseCharacter::  RegisterInteractiveActor(AInteractiveActor* InteractiveActor){ AvailableInteractiveActors.AddUnique(InteractiveActor); }
void ATPS_BaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor){ AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);}

FCollisionQueryParams ATPS_BaseCharacter::GetIgnorCharacterParams() const
{
	FCollisionQueryParams Params;

	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);
	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

FVector ATPS_BaseCharacter::GetCapsuleLocation() const { return GetCapsuleComponent()->GetComponentLocation(); }
void ATPS_BaseCharacter::SetCapsuleWorldLocation(const FVector &NewLocation) const { GetCapsuleComponent()->SetWorldLocation(NewLocation); }

#pragma region GameplayAbilities

void ATPS_BaseCharacter::TryChangeState_InteractWithZipline(bool bValue) const { TryChangeState(OnZiplineAbilityTag, bValue); }
void ATPS_BaseCharacter::TryChangeState_InteractWithLadder (bool bValue) const { TryChangeState(OnLadderAbilityTag,  bValue); }
void ATPS_BaseCharacter::TryChangeState_WallRun            (bool bValue) const { TryChangeState(WallRunAbilityTag,   bValue); }
void ATPS_BaseCharacter::TryChangeState_Mantle             (bool bValue) const { TryChangeState(MantleAbilityTag,    bValue); }
void ATPS_BaseCharacter::TryChangeState_Crouch             (bool bValue) const { TryChangeState(CrouchAbilityTag,    bValue); }
void ATPS_BaseCharacter::TryChangeState_Sprint             (bool bValue) const { TryChangeState(SprintAbilityTag,    bValue); }
void ATPS_BaseCharacter::TryChangeState_Slide              (bool bValue) const { TryChangeState(SlideAbilityTag,     bValue); }
void ATPS_BaseCharacter::TryChangeState_Jump               (bool bValue) const { TryChangeState(JumpAbilityTag,      bValue); }
void ATPS_BaseCharacter::TryChangeState_Swim               (bool bValue) const { TryChangeState(SwimAbilityTag,      bValue); }
void ATPS_BaseCharacter::TryChangeState_Aim                (bool bValue) const { TryChangeState(AimAbilityTag,       bValue); }

UAbilitySystemComponent * ATPS_BaseCharacter::GetAbilitySystemComponent() const { return AbilitySystemComponent; }

void ATPS_BaseCharacter::InitAbilitySystemComponent(AController *NewController)
{
	AbilitySystemComponent->InitAbilityActorInfo(NewController, this); if(bAreAbilityAdded) return;
	
	for(const auto& AbilityClass : Abilities)
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass));
	
	bAreAbilityAdded = true;
}

void ATPS_BaseCharacter::TryChangeState(const FGameplayTag AbilityTag, bool bIsActive) const
{
	!AbilitySystemComponent->IsAbilityActive(AbilityTag) && bIsActive ?
		AbilitySystemComponent->TryActivateAbilityWithTag(AbilityTag) : AbilitySystemComponent->TryCancelAbilityWithTag(AbilityTag);
}

void ATPS_BaseCharacter::OnDeath()
{
	Multicast_RagdollStart();
}

void ATPS_BaseCharacter::Falling()
{
	Super::Falling();
	GetMyMovementComponent()->bNotifyApex = true;
}

void ATPS_BaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void ATPS_BaseCharacter::Landed(const FHitResult &Hit)
{
	Super::Landed(Hit);
	if (!IsValid(FallDamageCurve)) return;
	
	const float FallHeight = (CurrentFallApex - GetActorLocation()).Z;
	const float DamgeAmount = FallDamageCurve->GetFloatValue(FallHeight);
	TakeDamage(DamgeAmount, FDamageEvent{}, GetController(), Hit.GetActor());
}

#pragma endregion 
