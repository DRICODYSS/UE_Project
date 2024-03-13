// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core_TPSBaseCharacter.h"
#include "TPS_BaseCharacter.h"
#include "TPS_PlayerCharacter.generated.h"


UCLASS(Blueprintable)
class TPS_PETPROJECT_API ATPS_PlayerCharacter final: public ATPS_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ATPS_PlayerCharacter(const FObjectInitializer &ObjectInitializer);
	 virtual void Tick(float DeltaSeconds) override;

	virtual void ForwardMovementAction(float Value) override;
	virtual void RightMovementAction(float Value) override;
	
	virtual void CameraUpAction(float Value) override;
	virtual void CameraRightAction(float Value) override;

	virtual void ForwardSwimmingAction(float Value) override;
	virtual void SwimmingUpAction(float Value) override;

	virtual void ClimbLadderUpAction(float Value) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	class UCameraComponent* CameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character | Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool FixCharacterSurface = false;

private:
	/* Swimming input logic*/
	void ForwardSwimmingActionLogic(float Value);
	void SwimmingUpActionLogic(float Value);
	
	FTimerHandle TraceTimer;
	float TickTimer = 0.01f;
};
