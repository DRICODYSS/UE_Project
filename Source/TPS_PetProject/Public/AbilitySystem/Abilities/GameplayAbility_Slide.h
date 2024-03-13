// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BaseGameplayAbility.h"
#include "Character/TPS_BaseCharacter.h"

#include "GameplayAbility_Slide.generated.h"



UCLASS()
class TPS_PETPROJECT_API UGameplayAbility_Slide : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* SlideMontage;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	UAnimMontage* ExitSlideMontage;
	
	
};
