// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_OnZipline.h"

#include "Actors/Interactive/Environment/Zipline.h"
#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UGameplayAbility_OnZipline::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags,
                                                    const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGameplayAbility_OnZipline::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	const AZipline* AvaliableZipline = Character->GetAvalibleInteractiveActorByClass<AZipline>();
	if(!IsValid(AvaliableZipline)) return; 


	FRotator TargetOrientationRotation = (AvaliableZipline->GetStartLocation() - AvaliableZipline->GetEndLocation()).ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.f;
	TargetOrientationRotation.Pitch = AvaliableZipline->GetZiplineRotation().Pitch; //TODO add offset orientation - 10;
	Character->SetActorRotation(TargetOrientationRotation);

	FVector Actor = FVector(Character->GetActorLocation().X, Character->GetActorLocation().Y, 0.f);
	FVector StartPoint = FVector(AvaliableZipline->GetStartLocation().X, AvaliableZipline->GetStartLocation().Y, 0.f);

	FVector CashVector = Actor - StartPoint;
	
	FVector NewCharacterLocation = UKismetMathLibrary::GetDirectionUnitVector(AvaliableZipline->GetStartLocation(), AvaliableZipline->GetEndLocation()) * CashVector.Length() + AvaliableZipline->GetStartLocation();
	NewCharacterLocation.Z -= 125.f;
	

	Character->SetActorLocation(NewCharacterLocation);
	Character->GetMyMovementComponent()->AttachToZipline(AvaliableZipline);
	Character->SetMovementState(ETPSMovementState::OnZipline);
}

void UGameplayAbility_OnZipline::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);

	Character->GetMyMovementComponent()->DetachFromZipline();
	Character->SetMovementState(ETPSMovementState::InAir);
}
