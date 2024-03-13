// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BaseGameplayAbility.h"
#include "GameplayAbility_WallRun.generated.h"

struct FWallRunningParameters;

enum EWallSide : uint8;
 struct FRunnableWallDescription;
class UCurveVector;

USTRUCT(BlueprintType)
struct FWallRunSettings
{
	GENERATED_BODY()

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* InitCurve {};
	
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* SartMontage {};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* StartCurve {};
	
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* StopMontage {};

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly)
	UCurveVector* StoptCurve {};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrection = 65.0f;
};
/**
 * 
 */
UCLASS()
class TPS_PETPROJECT_API UGameplayAbility_WallRun : public UBaseGameplayAbility
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
protected:
	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	FWallRunSettings RightWallRunSettings;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	FWallRunSettings LeftWallRunSettings;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Timers", meta=(ClampMin="0", UIMin="0"))
	float InitTimer  = 0.01f;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Timers", meta=(ClampMin="0", UIMin="0"))
	float StartTimer  = 0.67f;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Timers", meta=(ClampMin="0", UIMin="0"))
	float CycleTimer = 0.1f;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Timers", meta=(ClampMin="0", UIMin="0"))
	float Stoptimer  = 0.67f;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization settings", meta=(ClampMin="0", UIMin="0"))
	float CallTimeToCheckContinueRun  = 0.02f;

	UPROPERTY (EditDefaultsOnly, BlueprintReadOnly, Category = "Optimization settings", meta=(ClampMin="0", UIMin="0"))
	float TraceDistance  = 10.f;

private:
	FTimerHandle RunContinueCheckTimer;

	const FWallRunSettings & GetWallRunSettings(EWallSide WallSide) const;
	void InitWallRunMovementParameters(FWallRunningParameters& WallRunMovementParameters, const ATPS_BaseCharacter* Character, const FRunnableWallDescription& RunnableWallDescription, const FWallRunSettings& WallRunSettings);

	void RunContinueCheck(const ATPS_BaseCharacter* Character, const FCollisionQueryParams& QueryParams, bool bWallSideIsLeft, bool bDrawDebug = false);
};
