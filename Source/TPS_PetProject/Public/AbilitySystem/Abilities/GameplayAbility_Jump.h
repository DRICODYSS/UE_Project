// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BaseGameplayAbility.h"
#include "Library/Character/TPSCharacterEnumLibrary.h"
#include "GameplayAbility_Jump.generated.h"


struct FCJumpMovementParameters;
class UCurveVector;


USTRUCT(BlueprintType)
struct FCJumpSettings
{
	GENERATED_BODY()
	
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* JumpMontage {};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* JumpMovementCurve {};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	float AnimDuration = 0.f;
};

UCLASS()
class TPS_PETPROJECT_API UGameplayAbility_Jump : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Run Jump Settings")
	FCJumpSettings WallRunJumpRF;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Run Jump Settings")
	FCJumpSettings WallRunJumpLF;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Run Jump Settings")
	FCJumpSettings WallRunJumpR_90;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Wall Run Jump Settings")
	FCJumpSettings WallRunJumpL_90;

private:
	void SetCJumpMovementParameters(FCJumpMovementParameters& CJumpMovementParameters, const ATPS_BaseCharacter* Character, EJumpTypes JumpType) const;
	const FCJumpSettings& GetWallRunSettings(EJumpTypes MovementState, const ATPS_BaseCharacter* Character) const;
};
