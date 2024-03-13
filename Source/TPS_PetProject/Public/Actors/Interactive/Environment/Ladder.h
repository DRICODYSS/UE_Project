// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Ladder.generated.h"


UCLASS(Blueprintable)
class TPS_PETPROJECT_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()

public:
	ALadder();

	float GetLadderHeight() const { return LadderHeight; }
	bool GetIsOnTop() const { return bIsOnTop; }
	UAnimMontage *GetAttachFromTopAnimMontage() const { return AttachFromTopAnimMontage; }
	FVector GetAttachFromTopAnimMontageStartingLocation() const;

protected:
	virtual void OnConstruction(const FTransform &Transform) override;
	virtual void BeginPlay() override;
	
	
	
	//float GetLadderWidth() const { return LadderWidth;}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderHeight = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float LadderWidth = 50.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float StepsInterval = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	float BottomStepOffset = 25.f;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightRailMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMesh;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TopInteractionVolume;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	FVector AttachFromAnimMontageInitialOffset {FVector::ZeroVector};
	
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	auto GetLadderInteractionBox() const -> UBoxComponent*;

	void SetLocationAndRotationForRail(UStaticMeshComponent*& RailMeshComponent, const bool SignBeforeY) const;
	void SetRelativeScale3DForSteps() const;
	void SetCurrentNumberOfSteps() const;
	
	bool bIsOnTop = false;
	
	
};
