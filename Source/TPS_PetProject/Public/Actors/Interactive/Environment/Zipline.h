// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Zipline.generated.h"

class UCapsuleComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class TPS_PETPROJECT_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()

public:
	AZipline();
	virtual void OnConstruction(const FTransform& Transform) override;

	FVector GetStartLocation() const { return  PillarStart->GetSocketLocation(SocketAttachPointName); }
	FVector GetEndLocation() const { return PillarEnd->GetSocketLocation(SocketAttachPointName); }
	FRotator GetZiplineRotation() const { return ZiplineRotation; }
	
protected:
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters", meta = (MakeEditWidget))
	FVector EndPoint = FVector(500.f, 0.f, 0.f);

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float InteractionVolumeRadius = 22.f;

	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	float InteractionVolumeDelta = 0.f;
	
	UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Zipline parameters")
	FName SocketAttachPointName;
	
	UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarStart;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PillarEnd;

	UPROPERTY (VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* CableMesh;

	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
private:
	auto GetZiplineInteractionCapsule() const -> UCapsuleComponent*;
	
	FRotator ZiplineRotation = FRotator::ZeroRotator;

	static FRotator FindRotationBetweenPoints(const FVector& Start, const FVector& Target);
	static FVector GetRelativeSocketLocation(const UStaticMeshComponent* PillarMesh, FName SocketName);

	void SetLocationForInteractiveCable() const;
	void SetRotationForInteractiveCable();
	void SetScaleForInteractiveCable() const;
	
};
