// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_OnLadder.h"

#include "Actors/Interactive/Environment/Ladder.h"
#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"

bool UGameplayAbility_OnLadder::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags,
	const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) 
		return false;
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	if (!IsValid(Character->GetAvalibleInteractiveActorByClass<ALadder>())) return false;
	
	switch(Character->GetMovementState())
	{
	case ETPSMovementState::Grounded : return true;
	case ETPSMovementState::Ragdoll : Character->ReplicatedRagdollEnd(); return false;
	default : return false;
	}
}

void UGameplayAbility_OnLadder::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	const ALadder* AvaliableLadder = Character->GetAvalibleInteractiveActorByClass<ALadder>();
	if(IsValid(AvaliableLadder))
	{
		FRotator TargetOrientationRotation = AvaliableLadder->GetActorForwardVector().ToOrientationRotator();
		TargetOrientationRotation.Yaw += 180.f;

		FVector LadderUpVector = AvaliableLadder->GetActorUpVector();
		FVector LadderToCharacterDistance = Character->GetMyMovementComponent()->GetActorLocation() - AvaliableLadder->GetActorLocation();
		float Projection = FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);

		FVector NewCharacterLocation = AvaliableLadder->GetActorLocation() + Projection * AvaliableLadder->GetActorUpVector() + LadderToCharacterOffset * AvaliableLadder->GetActorForwardVector();
		
		if(AvaliableLadder->GetIsOnTop())
		{
			NewCharacterLocation = AvaliableLadder->GetAttachFromTopAnimMontageStartingLocation();
			Character->PlayAnimMontage(AvaliableLadder->GetAttachFromTopAnimMontage());
		}
		Character->SetActorLocationAndRotation(NewCharacterLocation, TargetOrientationRotation);




		
		Character->GetMyMovementComponent()->AttachToLadder(AvaliableLadder);
		Character->SetMovementState(ETPSMovementState::OnLadder);
	}
}

void UGameplayAbility_OnLadder::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->GetMyMovementComponent()->DetachFromLadder();
}
