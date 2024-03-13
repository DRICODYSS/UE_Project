// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Aim.h"

#include "Character/TPS_BaseCharacter.h"

void UGameplayAbility_Aim::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->SetRotationMode(ETPSRotationMode::Aiming);
	Character->SetDesiredGait(ETPSGait::Walking);
}

void UGameplayAbility_Aim::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->SetRotationMode(Character->GetDesiredRotationMode());
	Character->SetDesiredGait(ETPSGait::Running);
}
