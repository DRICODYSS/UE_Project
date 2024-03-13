// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Swim.h"

#include "Character/TPS_BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UGameplayAbility_Swim::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->GetCharacterMovement()->MovementMode = MOVE_Swimming;
	Character->SetMovementState(ETPSMovementState::Swimming);
}

void UGameplayAbility_Swim::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->GetCharacterMovement()->MovementMode = MOVE_Walking;
	Character->SetMovementState(ETPSMovementState::Grounded);
}
