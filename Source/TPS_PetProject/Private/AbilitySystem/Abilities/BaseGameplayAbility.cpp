// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/BaseGameplayAbility.h"

#include "Character/TPS_BaseCharacter.h"

bool UBaseGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) 
		return false;

	return ActorInfo->AvatarActor->IsA<ATPS_BaseCharacter>();
}

ATPS_BaseCharacter* UBaseGameplayAbility::GetPossessedCharacter(const FGameplayAbilityActorInfo* ActorInfo)
{
	return StaticCast<ATPS_BaseCharacter*>(ActorInfo->AvatarActor.Get());
}
