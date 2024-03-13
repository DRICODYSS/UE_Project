// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable All
#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Components/MovementComponents/PhysLogics/CustomPhysLogics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Library/Character/TPSCharacterEnumLibrary.h"
#include "Library/Character/TPSCharacterStructLibrary.h"
#include "Library/Movement/TPSMovementStructLibrary.h"
#include "MovementTypes/FMovementParams_Slide.h"

#include "TPS_CharacterMovementComponent.generated.h"

class ATPS_BaseCharacter;


UCLASS()
class TPS_PETPROJECT_API UTPS_CharacterMovementComponent final: public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaTime, const FVector &OldLocation, const FVector &OldVelocity) override;

	// Movement Settings Override
 	virtual void  PhysWalking(float deltaTime, int32 Iterations) override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	// Movement Settings Variables
	UPROPERTY()
	uint8 bRequestMovementSettingsChange = 1;

	UPROPERTY()
	ETPSGait AllowedGait = ETPSGait::Walking;

	
 public:
	// Set Movement Curve (Called in every instance)
	float GetMappedSpeed() const;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Movement System")
	FTPSMovementSettings CurrentMovementSettings;

	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	void SetMovementSettings(FTPSMovementSettings NewMovementSettings);

	// Set Max Walking Speed (Called from the owning client)
	UFUNCTION(BlueprintCallable, Category = "Movement Settings")
	void SetAllowedGait(ETPSGait NewAllowedGait);

public:
	virtual float GetMaxSpeed() const override;
	virtual void  PhysicsRotation(float DeltaTime) override;

	void ChangeTypeJump      (const FCJumpMovementParameters &CJumpMovementParameters);   void StopCustomJump();
	void StartMantling       (const FMantlingParameters      &MantlingParameters);        void StopMantling();
	void AttachToRunnableWall(const FWallRunningParameters   &WallRunMovementParameters); void DetachToRunnableWall();
	void EnterSlide(); void ExitSlide();
	
	void AttachToLadder (const class ALadder*  Ladder);  void DetachFromLadder();
	void AttachToZipline(const class AZipline* Zipline); void DetachFromZipline();
	
	
	const ALadder* GetCurrentLadder() const { return CurrentLadder; }
	
	float GetLadderSpeedRation() const;
	float GetActorToLadderProjection(const FVector& Location) const;
	bool  IgnoreMovementState_OnLadder() const { return bIgnoreMovementState_OnLadder; }

	const FWallRunningParameters& GetCurrentWallRunParameters() const { return CurrentWallRunParameters; }
	
	
protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	
	
	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Ladder")
	float ClimbingOnLadderSpeed = 200.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Ladder")
	float ClimbingOnLadderBrakingDecelartion = 2048.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Ladder")
	float MaxLadderTopOffset = 90.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Ladder")
	float MinLadderBottomOffset = 90.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Ladder")
	float JumpOffFromLadderSpeed = 500.0f;

	
	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Zipline")
	float SlideOnZiplineSpeed = 500.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Zipline")
	float ZiplineBrakingDeceleration = 2048.f;
	

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: WallRun")
	float WallRunningSpeed = 650.f;

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: WallRun")
	float WallRunBrakingDeceleration = 2048.f;
	

	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Slide")
	float Slide_MinSpeed {350.f};
	
	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Slide")
	float Slide_EnterImpulse {500.f};
	
	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Slide")
	float Slide_GravityForce {5000.f};
	
	UPROPERTY (EditAnywhere, BlueprintReadwrite, meta=(ClampMin="0", UIMin="0"), Category="Character Movement: Slide")
	float Slide_Friction {1.3f};
	
	inline ATPS_BaseCharacter* GetBaseCharacterOwner() const { return StaticCast<ATPS_BaseCharacter*>(CharacterOwner); }
	
private:
	friend void UPhysLogicComponent::CustomPhysRotation(UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime);
	
	friend void UPhysLogicComponent::PhysCustomJump    (const UTPS_CharacterMovementComponent* MovementComponent); // TODO REWORK
	friend void UPhysLogicComponent::PhysMantling      (UTPS_CharacterMovementComponent* MovementComponent);
	friend void UPhysLogicComponent::PhysOnLadder      (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime); // TODO REWORK
	friend void UPhysLogicComponent::PhysOnZipline     (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime); // TODO REWORK
	friend void UPhysLogicComponent::PhysWallRunning   (UTPS_CharacterMovementComponent* MovementComponent, const float &deltaTime); // TODO REWORK
	void PhysSliding( const float &deltaTime, int32 Iterations);
	

	void ChangeTimerAndState_WallRun();

	void UpdateActorTransformForSliding(float Alpha);
	void UpdateActorRotationAfterSliding(float Alpha);
	
	bool GetSlideSurface(FHitResult& Hit);

	FCJumpMovementParameters CurrentCJumpMovementParameters;
	FMantlingParameters      CurrentMantlingParameters;
	FWallRunningParameters   CurrentWallRunParameters;
	FMovementParams_Slide CurrentSlideParams;

	const ALadder*  CurrentLadder  {nullptr};
	const AZipline* CurrentZipline {nullptr};
	

	EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::JumpOff;
	FRotator ForceTargetRotation {FRotator::ZeroRotator};

	FTimerHandle CustomTimerHandle;
	
	
	bool bIgnoreMovementState_OnLadder = false;
	bool bForceRotation = false;
	

};