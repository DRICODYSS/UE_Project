// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Slide.h"

#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"


bool UGameplayAbility_Slide::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	if (Character->GetMyMovementComponent()->Velocity.Length() < Character->GetMovmentData().VelocityDirection.Standing.RunSpeed) return false;

	return !Character->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && Character->GetMovementState() != ETPSMovementState::InAir;
}

void UGameplayAbility_Slide::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	
	Character->GetMesh()->GetAnimInstance()->Montage_Play(SlideMontage, 1.f, EMontagePlayReturnType::Duration, 0.f);
	Character->GetMyMovementComponent()->EnterSlide();
	Character->SetMovementState(ETPSMovementState::Sliding);
}

void UGameplayAbility_Slide::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->GetMesh()->GetAnimInstance()->Montage_Stop(0.25f, SlideMontage);
	Character->GetMesh()->GetAnimInstance()->Montage_Play(ExitSlideMontage, 1.f, EMontagePlayReturnType::Duration, 0.f);
	Character->SetMovementState(ETPSMovementState::Grounded);

}

