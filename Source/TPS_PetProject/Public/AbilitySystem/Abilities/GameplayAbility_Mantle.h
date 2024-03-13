// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BaseGameplayAbility.h"
#include "GameplayAbility_Mantle.generated.h"

class UCurveVector;

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* MantlingMontage {};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* MantlingCurve {};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 220.0f;
	
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeight {220.f};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeight {100.f};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MaxHeightStartTime {0.f};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMin = 0.f))
	float MinHeightStartTime {0.f};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	bool bNeedUseCurveVectorZForMoving = false;
};
/**
 * 
 */
UCLASS()
class TPS_PETPROJECT_API UGameplayAbility_Mantle : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	FMantlingSettings HighMantleSettings;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	FMantlingSettings LowMantleSettings;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	float LowMantleMaxHeight = 125.f;

private:
	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;
	void Mantle(ATPS_BaseCharacter* Character);
};
