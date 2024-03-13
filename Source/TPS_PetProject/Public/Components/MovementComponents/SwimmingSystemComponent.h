// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwimmingSystemComponent.generated.h"


class ATPS_BaseCharacter;
class ABaseWaterPool;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_PETPROJECT_API USwimmingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USwimmingSystemComponent();

	bool RegisterInteractiveActor(ABaseWaterPool* WaterVolume);
	void UnregisterInteractiveActor();
	const ABaseWaterPool* GetAvailableWaterVolume() const;

	void StartSwimmingLogic();
	void PauseSwimAction() const;
	void FixCharacterPosition();

	float GetWaterSurfaceZ() const;
	bool IsUnderWater() const { return bUnderWater; }
	void SetIsUnderWater(bool bValue) { bUnderWater = bValue; }
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Tick Timer parameters")
	float LogicTickTimer = 0.01f;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Tick Timer parameters")
	float PauseSwimTimerTick = 0.01f;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Fix Location")
	float FixCharacterLocationOnSurface = -15.f;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Fix Location")
	float FixCharacterLocationUnderWater = 95.f;

private:
	void SwimAction(bool bValue) const;
	void GroundedToWaterLogic();
	void InAirToWaterLogic();
	void StartCheckPauseSwim();
	void CheckPauseSwim();
	
	UPROPERTY()
	ABaseWaterPool* AvailableWaterVolume = nullptr;
	
	TWeakObjectPtr<ATPS_BaseCharacter> CachedBaseCharacterOwner;

	bool bUnderWater = false;
	bool bInAir = false;
	
	FTimerHandle GroundedToWaterTimer;
	FTimerHandle InAirToWaterTimer;
	FTimerHandle CheckPauseSwimTimer;
};
