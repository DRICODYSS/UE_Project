// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BasePlatform.generated.h"

UENUM()
enum class EPlatformBehavior : uint8
{
	OnDemand = 0,
	Loop
};

UCLASS()
class TPS_PETPROJECT_API ABasePlatform : public AActor
{
	GENERATED_BODY()
	
public:
	ABasePlatform();
	
	void PlatformTimelineUpdate(float Alpha);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PlatformMesh;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform setting", meta = (MakeEditWidget))
	FVector EndLocation {FVector::ZeroVector};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform setting")
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform setting")
	UCurveFloat* TimelineCurve;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Platform setting")
	class APlatformInvocator* PlatformInvocator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform setting")
	float TimerCooldown = 0.1f;
	
	UPROPERTY(BlueprintReadWrite)
	bool bIsOnTrigger = true;

	UFUNCTION() void OnPlatformInvoked(bool bIsOn);

	void MovePlatformTimer();
	
	void MovePlatform_OnDemand();
	void MovePlatform_Loop();

private:
	FVector StartLocation {FVector::ZeroVector};
	
	FTimerHandle TimerHandle;
	FTimeline PlatformTimeline;
};
