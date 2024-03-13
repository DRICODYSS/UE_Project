// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TPSPlayerController.generated.h"

struct FInputActionValue;
/**
 * 
 */
UCLASS()
class TPS_PETPROJECT_API ATPSPlayerController final: public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn *InPawn) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	
	/** Main character reference */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ATPS_BaseCharacter> PossessedCharacter = nullptr;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HOLD parameters")
	float SlideHoldTime {0.5f};

private:
	/* Axis */
	void ForwardMovementAction(float Value);
	void RightMovementAction  (float Value);
	void CameraUpAction       (float Value);
	void CameraRightAction    (float Value);
	void ForwardSwimmingAction(float Value);
	void RightSwimmingAction  (float Value);
	void SwimmingUpAction     (float Value);
	void ClimbLadderUpAction  (float Value);
	
	/* Action */
	void StartAim();    void StopAim();
	void StartJump();   void StopJump();
	void StartSprint(); void StopSprint();

	void InteractWithLadder();
	void StanceAction();
	void Mantle();
	void WallRun();
	void PressedSlide();
	void ReleasedSlide();
	void Slide();

	/* Tools */
	FTimerHandle HOLD_Timer;
	inline void SetTimer_HOLD(const float& Time);
	inline void ClearTimer_HOLD();
};
