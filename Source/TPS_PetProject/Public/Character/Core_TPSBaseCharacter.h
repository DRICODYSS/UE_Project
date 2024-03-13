// Fill out your copyright notice in the Description page of Project Settings.

#pragma once





#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Library/Character/TPSCharacterEnumLibrary.h"
#include "Library/Character/TPSCharacterStructLibrary.h"

#include "Core_TPSBaseCharacter.generated.h"

// forward declarationsggfgjg
// 
// class UALSDebugComponent;
class UAnimMontage;
class UTPSPlayerCameraBehavior;
enum class EVisibilityBasedAnimTickOption : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpPressedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRagdollStateChangedSignature, bool, bRagdollState);


UCLASS()
class TPS_PETPROJECT_API ATPSCoreBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATPSCoreBaseCharacter(const FObjectInitializer &ObjectInitializer);
	
	class UTPS_CharacterMovementComponent *GetMyMovementComponent() const { return MyCharacterMovementComponent; }

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

#pragma region Character States ALS
public:
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetMovementState(ETPSMovementState NewState, bool bForce = false);
	
	// UFUNCTION(BlueprintGetter, Category = "Character States")
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSMovementState GetPrevMovementState() const { return PrevMovementState; }
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetMovementAction(ETPSMovementAction NewAction, bool bForce = false);
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSMovementAction GetMovementAction() const { return MovementAction; }
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetStance(ETPSStance NewStance, bool bForce = false);
	
	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	ETPSStance GetStance() const { return Stance; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetOverlayOverrideState(int32 NewState);

	UFUNCTION(BlueprintGetter, Category = "Character States")
	int32 GetOverlayOverrideState() const { return OverlayOverrideState; }

	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetGait(ETPSGait NewGait, bool bForce = false);
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSGait GetGait() const { return Gait; }
	
	UFUNCTION(BlueprintGetter, Category = "CharacterStates")
	ETPSGait GetDesiredGait() const { return DesiredGait; }
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetRotationMode(ETPSRotationMode NewRotationMode, bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_SetRotationMode(ETPSRotationMode NewRotationMode, bool bForce);
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSRotationMode GetRotationMode() const { return RotationMode; }
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetViewMode(ETPSViewMode NewViewMode, bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_SetViewMode(ETPSViewMode NewViewMode, bool bForce);
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSViewMode GetViewMode() const { return ViewMode; }
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetOverlayState(ETPSOverlayState NewState, bool bForce = false);
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetGroundedEntryState(ETPSGroundedEntryState NewState);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_SetOverlayState(ETPSOverlayState NewState, bool bForce);
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSOverlayState GetOverlayState() const { return OverlayState; }
	
	UFUNCTION(BlueprintGetter, Category = "Character States")
	ETPSGroundedEntryState GetGroundedEntryState() const { return GroundedEntryState; }

	/** Landed, Jumped, Rolling, Mantling and Ragdoll*/
	/** On Landed*/
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void EventOnLanded();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character States")
	void Multicast_OnLanded();

	/** On Jumped*/
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void EventOnJumped();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character States")
	void Multicast_OnJumped();

	/** Rolling Montage Play Replication*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_PlayMontage(UAnimMontage *Montage, float PlayRate);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character States")
	void Multicast_PlayMontage(UAnimMontage *Montage, float PlayRate);

	/** Ragdolling*/
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void ReplicatedRagdollStart();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_RagdollStart();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character States")
	void Multicast_RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "Character States")
	void ReplicatedRagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_RagdollEnd(FVector CharacterLocation);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Character States")
	void Multicast_RagdollEnd(FVector CharacterLocation);

	UPROPERTY(BlueprintAssignable)
	FJumpPressedSignature JumpPressedDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnJumpedSignature OnJumpedDelegate;

	UPROPERTY(BlueprintAssignable)
	FRagdollStateChangedSignature RagdollStateChangedDelegate;

	UFUNCTION(BlueprintGetter, Category = "Input")
	ETPSStance GetDesiredStance() const { return DesiredStance; }
	
	UFUNCTION(BlueprintSetter, Category = "Input")
	void SetDesiredStance(ETPSStance NewStance);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Input")
	void Server_SetDesiredStance(ETPSStance NewStance);
	
	UFUNCTION(BlueprintCallable, Category = "Character States")
	void SetDesiredGait(ETPSGait NewGait);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_SetDesiredGait(ETPSGait NewGait);
	
	UFUNCTION(BlueprintGetter, Category = "Input")
	ETPSRotationMode GetDesiredRotationMode() const { return DesiredRotationMode; }
	
	UFUNCTION(BlueprintSetter, Category = "Input")
	void SetDesiredRotationMode(ETPSRotationMode NewRotMode);
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Character States")
	void Server_SetDesiredRotationMode(ETPSRotationMode NewRotMode);

	/** Rotation System */

	UFUNCTION(BlueprintCallable, Category = "Rotation System")
	void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);

	/** Movement System */

	UFUNCTION(BlueprintGetter, Category = "Movement System")
	bool HasMovementInput() const { return bHasMovementInput; }

	UFUNCTION(BlueprintCallable, Category = "Movement System")
	FTPSMovementSettings GetTargetMovementSettings() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement System")
	ETPSGait GetAllowedGait() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement System")
	ETPSGait GetActualGait(ETPSGait AllowedGait) const;

	UFUNCTION(BlueprintCallable, Category = "Movement System")
	bool CanSprint() const; //TODO Ability sys

	/** BP implementable function that called when Breakfall starts */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement System")
	void OnBreakfall();
	virtual void OnBreakfall_Implementation();

	/** BP implementable function that called when A Montage starts, e.g. during rolling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Movement System")
	void Replicated_PlayMontage(UAnimMontage *Montage, float PlayRate);
	virtual void Replicated_PlayMontage_Implementation(UAnimMontage *Montage, float PlayRate);

	/** Implement on BP to get required roll animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Movement System")
	UAnimMontage *GetRollAnimation();

	/** Utility */

	UFUNCTION(BlueprintCallable, Category = "Utility")
	float GetAnimCurveValue(FName CurveName) const;

	UFUNCTION(BlueprintCallable, Category = "Utility")
	void SetVisibleMesh(USkeletalMesh *NewSkeletalMesh);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Utility")
	void Server_SetVisibleMesh(USkeletalMesh *NewSkeletalMesh);

	/** Essential Information Getters/Setters */

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	FVector GetAcceleration() const { return Acceleration; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Essential Information")
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetMovementInputAmount() const { return MovementInputAmount; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Essential Information")
	FRotator GetAimingRotation() const { return AimingRotation; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetAimYawRate() const { return AimYawRate; }

	/** Input */
	
	//void JumpAction(bool bValue);
	//void SprintAction(bool bValue);
	//void AimAction(bool bValue);
	//void StanceAction();
	//void WalkAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void RagdollAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void VelocityDirectionAction();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void LookingDirectionAction();

protected:
	/** Ragdoll System */

	void RagdollUpdate(float DeltaTime);

	void SetActorLocationDuringRagdoll(float DeltaTime);

	/** State Changes */

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnMovementStateChanged(ETPSMovementState PreviousState);
	
	virtual void OnMovementActionChanged(ETPSMovementAction PreviousAction);
	
	virtual void OnStanceChanged(ETPSStance PreviousStance);
	
	virtual void OnRotationModeChanged(ETPSRotationMode PreviousRotationMode);
	
	// virtual void OnGaitChanged(ETPSGait PreviousGait);
	
	virtual void OnViewModeChanged(ETPSViewMode PreviousViewMode);
	
	virtual void OnOverlayStateChanged(ETPSOverlayState PreviousState);

	virtual void OnVisibleMeshChanged(const USkeletalMesh *PreviousSkeletalMesh);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnJumped_Implementation() override;

	virtual void Landed(const FHitResult &Hit) override;

	void OnLandFrictionReset();

	void SetEssentialValues(float DeltaTime);

	void UpdateCharacterMovement();

	void UpdateGroundedRotation(float DeltaTime);

	void UpdateInAirRotation(float DeltaTime);

	/** Utils */

	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);

	float CalculateGroundedRotationRate() const;

	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	void SetMovementModel();
	
	void ForceUpdateCharacterState();
	
	/** Replication */
	UFUNCTION(Category = "ALS|Replication")
	void OnRep_RotationMode(ETPSRotationMode PrevRotMode);
	
	UFUNCTION(Category = "ALS|Replication")
	void OnRep_ViewMode(ETPSViewMode PrevViewMode);
	
	UFUNCTION(Category = "ALS|Replication")
	void OnRep_OverlayState(ETPSOverlayState PrevOverlayState);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_VisibleMesh(const USkeletalMesh *PreviousSkeletalMesh);

protected:
	/* Custom movement component*/
	UPROPERTY()
	TObjectPtr<UTPS_CharacterMovementComponent> MyCharacterMovementComponent;

	/** Input */

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input")
	ETPSRotationMode DesiredRotationMode = ETPSRotationMode::LookingDirection;
	
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input")
	ETPSGait DesiredGait = ETPSGait::Running;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "ALS|Input")
	ETPSStance DesiredStance = ETPSStance::Standing;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookUpDownRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookLeftRightRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float RollDoubleTapTimeout = 0.3f;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bBreakFall = false;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bSprintHeld = false;

	/** Movement System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Movement System")
	FDataTableRowHandle MovementModel;

	/** Essential Information */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float MovementInputAmount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float AimYawRate = 0.0f;

	/** Replicated Essential Information*/

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float EasedMaxAcceleration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FVector ReplicatedCurrentAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;

	/** Replicated Skeletal Mesh Information*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Skeletal Mesh", ReplicatedUsing = OnRep_VisibleMesh)
	TObjectPtr<USkeletalMesh> VisibleMesh = nullptr;

	/** State Values */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_OverlayState)
	ETPSOverlayState OverlayState = ETPSOverlayState::Default;
	
	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	ETPSGroundedEntryState GroundedEntryState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	ETPSMovementState MovementState = ETPSMovementState::None;
	
	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	ETPSMovementState PrevMovementState = ETPSMovementState::None;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	ETPSMovementAction MovementAction = ETPSMovementAction::None;
	
	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_RotationMode)
	ETPSRotationMode RotationMode = ETPSRotationMode::LookingDirection;
	
	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	ETPSGait Gait = ETPSGait::Walking;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values")
	ETPSStance Stance = ETPSStance::Standing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_ViewMode)
	ETPSViewMode ViewMode = ETPSViewMode::ThirdPerson;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	int32 OverlayOverrideState = 0;

	/** Movement System */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Movement System")
	FTPSMovementStateSettings MovementData;

	/** Rotation System */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator InAirRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	float YawOffset = 0.0f;

	/** Breakfall System */

	/** If player hits to the ground with a specified amount of velocity, switch to breakfall state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System")
	bool bBreakfallOnLand = true;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to breakfall state
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System",
			  meta = (EditCondition = "bBreakfallOnLand"))
	float BreakfallOnLandVelocity = 700.0f;

	/** Ragdoll System */

	/** If the skeleton uses a reversed pelvis bone, flip the calculation operator */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bReversedPelvis = false;

	/** If player hits to the ground with a specified amount of velocity, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnLand = false;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System",
			  meta = (EditCondition = "bRagdollOnLand"))
	float RagdollOnLandVelocity = 1000.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnGround = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollFaceUp = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	FVector LastRagdollVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Ragdoll System")
	FVector TargetRagdollLocation = FVector::ZeroVector;

	/* Server ragdoll pull force storage*/
	float ServerRagdollPull = 0.0f;

	/* Dedicated server mesh default visibility based anim tick option*/
	EVisibilityBasedAnimTickOption DefVisBasedTickOp;

	bool bPreRagdollURO = false;

	/** Cached Variables */

	FVector PreviousVelocity = FVector::ZeroVector;

	float PreviousAimYaw = 0.0f;

	/** Last time the 'first' crouch/roll button is pressed */
	float LastStanceInputTime = 0.0f;

	/* Timer to manage reset of braking friction factor after on landed event */
	FTimerHandle OnLandedFrictionResetTimer;

	/* Smooth out aiming by interping control rotation*/
	FRotator AimingRotation = FRotator::ZeroRotator;

	/** We won't use curve based movement and a few other features on networked games */
	bool bEnableNetworkOptimizations = false;
	
	/** Ragdoll System */
public:
	/** Implement on BP to get required get up animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Ragdoll System")
	UAnimMontage *GetGetUpAnimation(bool bRagdollFaceUpState);

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "ALS|Ragdoll System")
	void Server_SetMeshLocationDuringRagdoll(FVector MeshLocation);

#pragma endregion
};
