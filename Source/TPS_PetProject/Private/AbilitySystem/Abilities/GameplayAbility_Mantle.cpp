// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_Mantle.h"

#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/LedgeDetectorComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Library/Character/TPSCharacterEnumLibrary.h"

bool UGameplayAbility_Mantle::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) 
		return false;

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	if(FLedgeDescription LedgeDescription; !Character->GetLedgeDetectorComponent()->DetectLedge(LedgeDescription))
		return false;
	
	switch(Character->GetMovementState())
	{
		case ETPSMovementState::Grounded : return true;
		case ETPSMovementState::Ragdoll : Character->ReplicatedRagdollEnd(); return false;
		default : break;
	}
	return Character->GetMyMovementComponent()->IgnoreMovementState_OnLadder();
}

void UGameplayAbility_Mantle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	switch (Character->GetStance())
	{
	case ETPSStance::Standing : Mantle(Character); break;
	case ETPSStance::Crouching : Character->UnCrouch(); Character->TryChangeState_Mantle(false); break;
	default : break;
	}
}

void UGameplayAbility_Mantle::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->SetMovementState(ETPSMovementState::Grounded);
}



const FMantlingSettings & UGameplayAbility_Mantle::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HighMantleSettings : LowMantleSettings;
}

void UGameplayAbility_Mantle::Mantle(ATPS_BaseCharacter* Character)
{
	Character->SetMovementState(ETPSMovementState::Mantling);
	Character->UnCrouch();
	FLedgeDescription LedgeDescription;
	if(Character->GetLedgeDetectorComponent()->DetectLedge(LedgeDescription))
	{
		const float MantlingHeight = (LedgeDescription.Location - Character->GetActorLocation()).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);
		
		float MinRange, MaxRange;
		MantlingSettings.MantlingCurve->GetTimeRange(MinRange,MaxRange);

		
		const FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		const FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		
		const FVector NewInitAnimLocation = LedgeDescription.Location - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNormal;
		
		FMantlingParameters MantlingParameters;

		MantlingParameters.InitLocation = Character->GetActorLocation();
		MantlingParameters.InitRotation = Character->GetActorRotation();
		MantlingParameters.InitAnimLocation = NewInitAnimLocation;
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;
		MantlingParameters.bNeedZValueChange = MantlingSettings.bNeedUseCurveVectorZForMoving;

		
		Character->GetMesh()->GetAnimInstance()->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		Character->GetMyMovementComponent()->StartMantling(MantlingParameters);
	}
}
