// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Platforms/BasePlatform.h"

#include "Actors/Platforms/PlatformInvocator.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>("PlatformRoot");
	RootComponent = DefaultPlatformRoot;
	
	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlatformMesh");
	PlatformMesh->SetupAttachment(DefaultPlatformRoot);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = PlatformMesh->GetRelativeLocation();

	if (!IsValid(TimelineCurve)) return;

	FOnTimelineFloatStatic PlatformMovementTimelineUpdate;
	PlatformMovementTimelineUpdate.BindUObject(this, &ABasePlatform::PlatformTimelineUpdate);
	PlatformTimeline.AddInterpFloat(TimelineCurve, PlatformMovementTimelineUpdate);

	if (IsValid(PlatformInvocator))
	{
		bIsOnTrigger = false;
		PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
	}
	else  MovePlatformTimer();
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::PlatformTimelineUpdate(float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation,EndLocation,Alpha);
	PlatformMesh->SetRelativeLocation(PlatformTargetLocation);
}


void ABasePlatform::OnPlatformInvoked(bool bIsOn) 
{
	bIsOnTrigger = bIsOn;
	MovePlatformTimer();
}

void ABasePlatform::MovePlatformTimer()
{
	switch (PlatformBehavior)
	{
		case EPlatformBehavior::OnDemand:
		{
			GetWorldTimerManager().SetTimer(TimerHandle,this,&ABasePlatform::MovePlatform_OnDemand,TimerCooldown,true);
			return;
		}
		case EPlatformBehavior::Loop:
		{
			GetWorldTimerManager().SetTimer(TimerHandle,this,&ABasePlatform::MovePlatform_Loop,TimerCooldown,true);
			return;
		}
		default: return;
	}
}


void ABasePlatform::MovePlatform_OnDemand()
{
	if (bIsOnTrigger)
	{
		PlatformTimeline.Play();
	}
	else
	{
		PlatformTimeline.Reverse();
	}
}

void ABasePlatform::MovePlatform_Loop()
{
	if (!bIsOnTrigger)
	{
		PlatformTimeline.Reverse();
		return;
	}
	
	if (PlatformTimeline.GetPlaybackPosition() == 0.f)
	{
		PlatformTimeline.Play();
	}
	else
	{
		PlatformTimeline.Reverse();
	}
}