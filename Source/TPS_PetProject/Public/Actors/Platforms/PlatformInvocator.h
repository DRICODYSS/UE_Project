// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "PlatformInvocator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInvocatorActivated, bool, IsOn);

UCLASS()
class TPS_PETPROJECT_API APlatformInvocator :  public AInteractiveActor
{
	GENERATED_BODY()
public:
	APlatformInvocator();

	UPROPERTY(BlueprintAssignable) FOnInvocatorActivated OnInvocatorActivated;
	
protected:
	void CheckIsBound();
	
	UPROPERTY(EditAnywhere, Category = "Timer setting")
	float WorkingTime = 20.f;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOn = false;
	FTimerHandle TimerHandle;

};
