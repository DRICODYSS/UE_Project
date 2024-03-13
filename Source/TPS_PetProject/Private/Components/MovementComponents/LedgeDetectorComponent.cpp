// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MovementComponents/LedgeDetectorComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Library/TraceUtils.h"
#include "Subsystems/DebugSubsystem.h"
#include "TPS_PetProject/GameTypes.h"


void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner()->IsA<ACharacter>());
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}

bool ULedgeDetectorComponent::DetectLedge(FLedgeDescription &LedgeDescription) const
{
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;
	FVector OverlapLocation;
	FHitResult ForwardCheckHitResult, DownCheckHitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnabled(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnabled = false;
#endif
	
	if(!ForwardCheck(CapsuleComponent, QueryParams, CharacterBottom, ForwardCheckHitResult, bIsDebugEnabled)) return false;
	if(!DownwardCheck(CapsuleComponent, QueryParams, CharacterBottom, ForwardCheckHitResult, DownCheckHitResult, bIsDebugEnabled)) return false;
	if(OverlapCheck(CapsuleComponent, QueryParams, DownCheckHitResult, OverlapLocation, bIsDebugEnabled)) return false;

	LedgeDescription.Location    =  OverlapLocation;
	LedgeDescription.Rotation    = (ForwardCheckHitResult.ImpactNormal * FVector(-1.f, -1.f,0.f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;
	
	return true;
}

bool ULedgeDetectorComponent::ForwardCheck(const UCapsuleComponent *CapsuleComponent,
                                           const FCollisionQueryParams &QueryParams, const FVector &CharacterBottom, FHitResult &ForwardCheckHitResult, bool
                                           bDrawDebug) const
{
	const float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5;
	
	const FVector ForwardStartLocation = CharacterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	const FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	return TraceUtils::SweepCapsuleSingleByChanel
	(
		GetWorld(),
		ForwardCheckHitResult,
		ForwardStartLocation,
		ForwardEndLocation,
		ForwardCheckCapsuleRadius,
		ForwardCheckCapsuleHalfHeight,
		FQuat::Identity,
		ECC_CLIMBING,
		QueryParams,
		FCollisionResponseParams::DefaultResponseParam,
		bDrawDebug, DrawTime
	);
}

bool ULedgeDetectorComponent::DownwardCheck(const UCapsuleComponent *CapsuleComponent,
	FCollisionQueryParams QueryParams, FVector CharacterBottom, FHitResult ForwardCheckHitResult,
	FHitResult &DownCheckHitResult, bool bDrawDebug) const
{
	const float DownwardSphereTraceRadius = CapsuleComponent->GetScaledCapsuleRadius();
	
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaxLedgeHeight + DownwardSphereTraceRadius;
	const FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);

	return TraceUtils::SweepSphereSingleByChanel
	(
		GetWorld(),
		DownCheckHitResult,
		DownwardStartLocation,
		DownwardEndLocation,
		DownwardSphereTraceRadius,
		ECC_CLIMBING,
		QueryParams,
		FCollisionResponseParams::DefaultResponseParam,
		bDrawDebug, DrawTime
	);
}

bool ULedgeDetectorComponent::OverlapCheck(const UCapsuleComponent *CapsuleComponent, FCollisionQueryParams QueryParams,
	FHitResult DownCheckHitResult, FVector &OverlapLocation, bool bDrawDebug) const
{
	const float OverlapCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const float OverlapCapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	
	OverlapLocation = DownCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) *FVector::UpVector;

	return TraceUtils::OverlapBlockingTestByProfile
	(
		GetWorld(),
		OverlapLocation,
		OverlapCapsuleRadius,
		OverlapCapsuleHalfHeight,
		FQuat::Identity,
		CollisionProfilePawnName,
		QueryParams,
		bDrawDebug, DrawTime
	);
}





