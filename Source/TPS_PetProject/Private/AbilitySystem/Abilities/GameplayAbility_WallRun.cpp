// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbility_WallRun.h"

#include "Character/TPS_BaseCharacter.h"
#include "Components/MovementComponents/RunnableWallDetectorComponent.h"
#include "Components/MovementComponents/TPS_CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Library/TraceUtils.h"
#include "Subsystems/DebugSubsystem.h"
#include "TPS_PetProject/GameTypes.h"

bool UGameplayAbility_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayTagContainer *SourceTags, const FGameplayTagContainer *TargetTags, FGameplayTagContainer *OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags)) return false;

	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	if(FRunnableWallDescription RunnableWallDescription; !Character->GetRunnableWallDetectorComponent()->DetectRunnableWall(RunnableWallDescription)) return false;
	Character->GetRunnableWallDetectorComponent()->ClearWallSide();
	
	switch(Character->GetMovementState())
	{
		case ETPSMovementState::Grounded : return true;
		case ETPSMovementState::Ragdoll : Character->ReplicatedRagdollEnd(); return false;
		default : return false;
	}
}

void UGameplayAbility_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData *TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	FWallRunningParameters WallRunMovementParameters;
	FRunnableWallDescription RunnableWallDescription;
	
	Character->GetRunnableWallDetectorComponent()->DetectRunnableWall(RunnableWallDescription);
	const FWallRunSettings& WallRunSettings = GetWallRunSettings(RunnableWallDescription.WallSide);
	InitWallRunMovementParameters(WallRunMovementParameters, Character, RunnableWallDescription, WallRunSettings);
	
	Character->GetMesh()->GetAnimInstance()->Montage_Play(WallRunSettings.SartMontage, 1.f, EMontagePlayReturnType::Duration, 0.f);
	Character->GetMyMovementComponent()->AttachToRunnableWall(WallRunMovementParameters);
	
	Character->SetMovementState(ETPSMovementState::WallRunning);

	
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRunnableWallDescription);
#else
	bool bIsDebugEnabled = false;
#endif
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(Character);
	GetWorld()->GetTimerManager().SetTimer(RunContinueCheckTimer, [this, QueryParams, bIsDebugEnabled, Character, WallRunMovementParameters]() {RunContinueCheck(Character, QueryParams, WallRunMovementParameters.bIsLeftSide , bIsDebugEnabled);}, CallTimeToCheckContinueRun, true);
}

void UGameplayAbility_WallRun::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo *ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	GetWorld()->GetTimerManager().ClearTimer(RunContinueCheckTimer);
	
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("WallRun: Cancel")));
	ATPS_BaseCharacter* Character = GetPossessedCharacter(ActorInfo);
	Character->GetMyMovementComponent()->DetachToRunnableWall();
	Character->GetRunnableWallDetectorComponent()->ClearWallSide();
}


void UGameplayAbility_WallRun::InitWallRunMovementParameters(FWallRunningParameters &WallRunMovementParameters, const ATPS_BaseCharacter* Character, const FRunnableWallDescription& RunnableWallDescription, const FWallRunSettings& WallRunSettings)
{
	const FVector NewInitAnimLocation = RunnableWallDescription.Location + RunnableWallDescription.WallNormal * WallRunSettings.AnimationCorrection;
	
	FRotator TargetRotation = RunnableWallDescription.Rotation;
	TargetRotation.Yaw += RunnableWallDescription.WallSide == Left ? 90.f : -90.f;
	
	WallRunMovementParameters.InitLocation        = Character->GetActorLocation();
	WallRunMovementParameters.TargetInitLocation  = NewInitAnimLocation;
	WallRunMovementParameters.TargetRotation      = TargetRotation;
	WallRunMovementParameters.Timer             = {InitTimer, StartTimer, CycleTimer, Stoptimer};
	WallRunMovementParameters.InitCurve           = WallRunSettings.InitCurve;
	WallRunMovementParameters.WallRunStartCurve   = WallRunSettings.StartCurve;
	WallRunMovementParameters.WallRunStopCurve    = WallRunSettings.StoptCurve;
	WallRunMovementParameters.bIsLeftSide         = RunnableWallDescription.WallSide == Left;
	WallRunMovementParameters.MovementState       = Init;
	WallRunMovementParameters.StopAnimMontage     = WallRunSettings.StopMontage;
}

const FWallRunSettings & UGameplayAbility_WallRun::GetWallRunSettings(EWallSide WallSide) const { return WallSide == Left ? LeftWallRunSettings : RightWallRunSettings; }

void UGameplayAbility_WallRun::RunContinueCheck(const ATPS_BaseCharacter* Character, const FCollisionQueryParams &QueryParams, bool bWallSideIsLeft, bool bDrawDebug)
{
	const float CurrentTraceDistance = bWallSideIsLeft ? -TraceDistance : TraceDistance;
	
	const FVector StartLocation = Character->GetActorLocation();
	const FVector EndLocation   = StartLocation + Character->GetActorRightVector() * CurrentTraceDistance;

	FHitResult CheckHitResult;
	if (TraceUtils::SweepLineTraceSingleByChanel
			(
				GetWorld(),
				CheckHitResult,
				StartLocation,
				EndLocation,
				ECC_WALL_RUNNING,
				QueryParams,
				FCollisionResponseParams::DefaultResponseParam,
				bDrawDebug, CallTimeToCheckContinueRun
			)) return;

	GetWorld()->GetTimerManager().ClearTimer(RunContinueCheckTimer);
	Character->TryChangeState_WallRun(false);
}
