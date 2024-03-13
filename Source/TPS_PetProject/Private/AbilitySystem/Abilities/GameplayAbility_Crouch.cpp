// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Crouch.h"

#include "Character/TPS_BaseCharacter.h"





bool UGameplayAbility_Crouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;
	
	return GetPossessedCharacter(ActorInfo)->GetMovementState() == ETPSMovementState::Grounded;
}

void UGameplayAbility_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->SetDesiredStance(ETPSStance::Crouching);
	Character->Crouch();
}

void UGameplayAbility_Crouch::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->SetDesiredStance(ETPSStance::Standing);
	Character->UnCrouch();
}