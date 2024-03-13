// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "WaterPhysicsVolume.generated.h"

/**
 * 
 */
UCLASS()
class TPS_PETPROJECT_API AWaterPhysicsVolume : public APhysicsVolume
{
	GENERATED_BODY()

public:
	explicit AWaterPhysicsVolume(const FObjectInitializer& ObjectInitializer);

	UBoxComponent* GetBoxComponent() const { return BoxComponent; }
	
	void SetWaterPhysics(float NewTerminalVelocity = 4000.f, float NewFluidFriction = 0.3f, bool bNewWaterVolume = false);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent;
};
