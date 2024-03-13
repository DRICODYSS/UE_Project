// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Jump.h"

#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/RunnableWallDetectorComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"


bool UGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) 
		return false;

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	if (Character->GetMyMovementComponent()->CustomMovementMode == CMOVE_CustomJump) return false;
	if (Character->GetMovementState() != ETPSMovementState::WallRunning)
	{
		if(FRunnableWallDescription RunnableWallDescription; Character->GetRunnableWallDetectorComponent()->DetectRunnableWall(RunnableWallDescription))
		{
			Character->GetRunnableWallDetectorComponent()->ClearWallSide();
			return false;
		}
	}
	
	switch(Character->GetMovementState())
	{
		case ETPSMovementState::Grounded    : return true;
		case ETPSMovementState::OnZipline   : return true;
		case ETPSMovementState::WallRunning : return true;
		case ETPSMovementState::Ragdoll     : Character->ReplicatedRagdollEnd(); return false;
		default                             : return false;
	}
}

void UGameplayAbility_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	
	switch(Character->GetMovementState())
	{
		case ETPSMovementState::Grounded :
		{
			switch (Character->GetStance())
			{
			case ETPSStance::Standing : Character->Jump(); break;
			case ETPSStance::Crouching : Character->UnCrouch(); break;
			default : break;
			}
			break;
		}
		case ETPSMovementState::WallRunning :
		{
			Character->GetMesh()->GetAnimInstance()->StopAllMontages(0.25);
			Character->GetMesh()->GetAnimInstance()->Montage_Play(GetWallRunSettings(WallRunJumpF, Character).JumpMontage, 1.f, EMontagePlayReturnType::Duration, 0.f);
			FCJumpMovementParameters CJumpMovementParameters;
			SetCJumpMovementParameters(CJumpMovementParameters, Character, WallRunJumpF);
			Character->GetMyMovementComponent()->ChangeTypeJump(CJumpMovementParameters);
			break;
		}
		default: break;
	}
}

void UGameplayAbility_Jump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	GetPossessedCharacter(ActorInfo)->StopJumping();
}

void UGameplayAbility_Jump::SetCJumpMovementParameters(FCJumpMovementParameters &CJumpMovementParameters, const ATPS_BaseCharacter* Character, EJumpTypes JumpType) const
{
	CJumpMovementParameters.TargetInitLocation = Character->GetActorLocation();
	CJumpMovementParameters.TargetRotation     = Character->GetActorRotation();
	CJumpMovementParameters.AnimMoveCurve      = GetWallRunSettings(JumpType, Character).JumpMovementCurve;
	CJumpMovementParameters.Duration           = GetWallRunSettings(JumpType, Character).AnimDuration;
}

const FCJumpSettings & UGameplayAbility_Jump::GetWallRunSettings(EJumpTypes JumpType, const ATPS_BaseCharacter* Character) const
{
	if (Character->GetMyMovementComponent()->GetCurrentWallRunParameters().bIsLeftSide)
		return WallRunJumpLF;
	else
		return WallRunJumpRF;
}