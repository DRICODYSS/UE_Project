// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/RunnableWallDetectorComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Library/TraceUtils.h"
#include "Subsystems/DebugSubsystem.h"
#include "TPS_PetProject/GameTypes.h"


void URunnableWallDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner()->IsA<ACharacter>());
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}

bool URunnableWallDetectorComponent::DetectRunnableWall(FRunnableWallDescription &RunnableWallDescription)
{
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryRunnableWallDescription);
#else
	bool bIsDebugEnabled = false;
#endif

	switch (CurrentWallSide)
	{
	case None : GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("None")));  return GeneralDetect(RunnableWallDescription, CapsuleComponent, QueryParams, bIsDebugEnabled);
	case Right:  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Right"))); return LeftDetect   (RunnableWallDescription, CapsuleComponent, QueryParams, bIsDebugEnabled);
	case Left :  GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, FString::Printf(TEXT("Left"))); return RightDetect  (RunnableWallDescription, CapsuleComponent, QueryParams, bIsDebugEnabled);
	default: return false;
	}
}

bool URunnableWallDetectorComponent::GeneralDetect(FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent *CapsuleComponent, const FCollisionQueryParams &QueryParams, bool bDrawDebug)
{
	FHitResult RightCheckHitResult, LeftCheckHitResult;
	if (!WallCheck(CapsuleComponent, QueryParams, LeftCheckHitResult,  true,  bDrawDebug) &&
		!WallCheck(CapsuleComponent, QueryParams, RightCheckHitResult, false, bDrawDebug))
	{
		PreviousWallSide = CurrentWallSide;
		CurrentWallSide = EWallSide::None;
		return false;
	}
	
	if (LeftCheckHitResult.bBlockingHit == 0)
	{
		SetRunnableWallDescription(RunnableWallDescription, RightCheckHitResult, Right);
		PreviousWallSide = CurrentWallSide;
		CurrentWallSide = EWallSide::Right;
	}
	else if (RightCheckHitResult.bBlockingHit == 0)
	{
		SetRunnableWallDescription(RunnableWallDescription, LeftCheckHitResult, Left);
		PreviousWallSide = CurrentWallSide;
		CurrentWallSide = EWallSide::Left;
	}
	else
	{
		if (LeftCheckHitResult.Distance > RightCheckHitResult.Distance)
		{
			SetRunnableWallDescription(RunnableWallDescription, RightCheckHitResult, Right);
			PreviousWallSide = CurrentWallSide;
			CurrentWallSide = EWallSide::Right;
		}
		else
		{
			SetRunnableWallDescription(RunnableWallDescription, LeftCheckHitResult, Left);
			PreviousWallSide = CurrentWallSide;
			CurrentWallSide = EWallSide::Left;
		}
		
	}
	return true;
}

bool URunnableWallDetectorComponent::RightDetect(FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent *CapsuleComponent, const FCollisionQueryParams &QueryParams, bool bDrawDebug)
{
	FHitResult RightCheckHitResult;
	if(!WallCheck(CapsuleComponent, QueryParams, RightCheckHitResult, false, bDrawDebug))
	{
		PreviousWallSide = CurrentWallSide;
		CurrentWallSide = EWallSide::None;
		return false;
	}

	SetRunnableWallDescription(RunnableWallDescription, RightCheckHitResult, Right);
	PreviousWallSide = CurrentWallSide;
	CurrentWallSide = EWallSide::Right;
	return true;
}

bool URunnableWallDetectorComponent::LeftDetect(FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent *CapsuleComponent, const FCollisionQueryParams &QueryParams, bool bDrawDebug)
{
	FHitResult LeftCheckHitResult;
	if (!WallCheck(CapsuleComponent, QueryParams, LeftCheckHitResult, true,  bDrawDebug))
	{
		PreviousWallSide = CurrentWallSide;
		CurrentWallSide = EWallSide::None;
		return false;
	}

	SetRunnableWallDescription(RunnableWallDescription, LeftCheckHitResult, Left);
	PreviousWallSide = CurrentWallSide;
	CurrentWallSide = EWallSide::Left;
	return true;
}



/* Hit Check */
bool URunnableWallDetectorComponent::WallCheck(const UCapsuleComponent *CapsuleComponent, const FCollisionQueryParams &QueryParams, FHitResult &CheckHitResult, bool bWallSideIsLeft, bool bDrawDebug) const
{
	const float ForwardCheckCapsuleRadius     = CapsuleComponent->GetScaledCapsuleRadius();
	const float ForwardCheckCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	FVector RotatedDirection = CachedCharacterOwner->GetActorRightVector().RotateAngleAxis((bWallSideIsLeft ? -135.f : -45.f), FVector(0.0f, 0.0f, 1.0f));
	
	const FVector ForwardStartLocation = CachedCharacterOwner->GetActorLocation();
	const FVector ForwardEndLocation = ForwardStartLocation + RotatedDirection * TraceDistance;

	return TraceUtils::SweepCapsuleSingleByChanel
	(
		GetWorld(),
		CheckHitResult,
		ForwardStartLocation,
		ForwardEndLocation,
		ForwardCheckCapsuleRadius,
		ForwardCheckCapsuleHalfHeight,
		FQuat::Identity,
		ECC_WALL_RUNNING,
		QueryParams,
		FCollisionResponseParams::DefaultResponseParam,
		bDrawDebug, DrawTime
	);
}

/* Setter */
void URunnableWallDetectorComponent::SetRunnableWallDescription(FRunnableWallDescription &RunnableWallDescription, const FHitResult &HitResult, const EWallSide WallSide)
{
	RunnableWallDescription.WallSide    =  WallSide;
	RunnableWallDescription.Location    =  HitResult.Location;
	RunnableWallDescription.Rotation    = (HitResult.ImpactNormal * FVector(-1.f, -1.f,0.f)).ToOrientationRotator();
	RunnableWallDescription.WallNormal  =  HitResult.ImpactNormal;
	
}
