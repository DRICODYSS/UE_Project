// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "TPS_AbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class TPS_PETPROJECT_API UTPS_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool TryActivateAbilityWithTag(FGameplayTag GameplayTag, bool bAllowRemoteActivation = true);

	bool TryCancelAbilityWithTag(FGameplayTag GameplayTag) const;

	bool IsAbilityActive(FGameplayTag GameplayTag) const;
};
