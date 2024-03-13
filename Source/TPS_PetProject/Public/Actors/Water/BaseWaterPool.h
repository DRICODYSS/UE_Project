// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWaterPool.generated.h"


class UBoxComponent;
class UPostProcessComponent;
class AWaterPhysicsVolume;

UCLASS(Blueprintable)
class TPS_PETPROJECT_API ABaseWaterPool : public AActor
{
	GENERATED_BODY()

public:
	ABaseWaterPool();
	virtual void OnConstruction(const FTransform &Transform) override;
	
	float GetWaterSurfaceLocationZ() const;
	void ChangeWaterPhysicsVolumeParam(bool bValue) const;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* WaterSurface;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* WaterVolumeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Actor Size")
	FVector WaterVolumeSize = FVector(100.f);

	UFUNCTION()
	virtual void HandleWaterVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void HandleWaterVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY()
	AWaterPhysicsVolume* WaterPhysicsVolume;
};
