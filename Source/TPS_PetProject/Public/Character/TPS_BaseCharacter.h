// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Core_TPSBaseCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Components/CharacterComponents/CharacterAttributesComponent.h"
#include "TPS_BaseCharacter.generated.h"


class URunnableWallDetectorComponent;
class ULedgeDetectorComponent;
class USwimmingSystemComponent;
class UTPS_AbilitySystemComponent;

UCLASS(Abstract, NotBlueprintable)
class TPS_PETPROJECT_API ATPS_BaseCharacter : public ATPSCoreBaseCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// void mantle();
	ATPS_BaseCharacter(const FObjectInitializer &ObjectInitializer);
	virtual void PossessedBy(AController *NewController) override;
	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult &Hit) override;
	
	ETPSMovementState GetMovementState() const { return MovementState; }
	const FTPSMovementStateSettings& GetMovmentData() const { return MovementData; }
	
	USwimmingSystemComponent* GetSwimmingSystemComponent() const { return SwimmingSystemComponent; }
	ULedgeDetectorComponent* GetLedgeDetectorComponent() const { return LedgeDetectorComponent; }
	URunnableWallDetectorComponent* GetRunnableWallDetectorComponent() const { return RunnableWallDetectorComponent; }

	/* InteractiveActor */
	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	float GetTurnCameraValue() const { return TurnCameraValue; }
	void SetTurnCameraValue(const float Value) { TurnCameraValue = Value; }
	float GetUpCameraValue() const { return TurnCameraValue; }
	void SetUpCameraValue(const float Value) { TurnCameraValue = Value; }

	FCollisionQueryParams GetIgnorCharacterParams() const;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character | Attributes")
	UTPS_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character | Attributes")
	UCharacterAttributesComponent* CharacterAttributesComponent;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	USwimmingSystemComponent* SwimmingSystemComponent;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	URunnableWallDetectorComponent* RunnableWallDetectorComponent;
	
	
#pragma region BindAxis
public:
	virtual void ForwardMovementAction(float Value) {}
	virtual void RightMovementAction(float Value) {}
	
	virtual void CameraUpAction(float Value) {}
	virtual void CameraRightAction(float Value) {}
	
	virtual void ForwardSwimmingAction(float Value) {}
	virtual void RightSwimmingAction(float Value) {}
	virtual void SwimmingUpAction(float Value) {}

	virtual void ClimbLadderUpAction(float Value) {}
	

	template<typename T>
	const T* GetAvalibleInteractiveActorByClass() const;

	FVector GetCapsuleLocation() const;
	void SetCapsuleWorldLocation(const FVector &NewLocation) const;
	
#pragma endregion 
	
	void TryChangeState_InteractWithZipline (bool bValue = true) const;
	void TryChangeState_InteractWithLadder  (bool bValue = true) const;
	void TryChangeState_WallRun             (bool bValue = true) const;
	void TryChangeState_Mantle			    (bool bValue = true) const;
	void TryChangeState_Crouch			    (bool bValue = true) const;
	void TryChangeState_Sprint			    (bool bValue = true) const;
	void TryChangeState_Slide               (bool bValue = true) const;
	void TryChangeState_Jump			    (bool bValue = true) const;
	void TryChangeState_Swim			    (bool bValue = true) const;
	void TryChangeState_Aim                 (bool bValue = true) const;
	

	virtual UAbilitySystemComponent *GetAbilitySystemComponent() const override;

protected:

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> Abilities;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag SprintAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag CrouchAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag JumpAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag AimAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag SwimAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag MantleAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag OnLadderAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag OnZiplineAbilityTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag WallRunAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Abilities")
	FGameplayTag SlideAbilityTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadonly, Category = "Character | Attributes")
	UCurveFloat* FallDamageCurve;
	
private:
	void InitAbilitySystemComponent(AController* NewController);
	void TryChangeState(const FGameplayTag AbilityTag, bool bIsActive = true) const;
	void OnDeath();
	
	bool bAreAbilityAdded = false;

	TArray<AInteractiveActor*> AvailableInteractiveActors;

	/* Advanced Swimming system */
	float TurnCameraValue = 0.f;
	float UpCameraValue = 0.f;

	FVector CurrentFallApex;
	
};

#pragma region Templates

template <typename T>
const T * ATPS_BaseCharacter::GetAvalibleInteractiveActorByClass() const
{
	for(const AInteractiveActor* IteractiveActor : AvailableInteractiveActors)
		if(IteractiveActor->IsA<T>()) return StaticCast<const T*>(IteractiveActor);
	
	return nullptr;
}

#pragma endregion



