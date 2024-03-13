// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LedgeDetectorComponent.generated.h"

class UCapsuleComponent;

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Ledge description")
	FVector Location{FVector::ZeroVector};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Ledge description")
	FRotator Rotation{FRotator::ZeroRotator};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Ledge description")
	FVector LedgeNormal{FVector::ZeroVector};
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_PETPROJECT_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription) const;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float MinLedgeHeight = 40.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float MaxLedgeHeight = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float ForwardCheckDistance = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float BottomZOffset = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float DownwardCheckDepthOffset = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float OverlapCapsuleFloorOffset = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	FColor TraceColor = FColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	FColor HitColor = FColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float DrawTime = 2.0f;

private:
	TWeakObjectPtr<ACharacter> CachedCharacterOwner;

	bool ForwardCheck(const UCapsuleComponent* CapsuleComponent, const FCollisionQueryParams& QueryParams, const FVector& CharacterBottom, FHitResult& ForwardCheckHitResult, bool bDrawDebug = false) const;
	bool DownwardCheck(const UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QueryParams, FVector CharacterBottom, FHitResult ForwardCheckHitResult, FHitResult& DownCheckHitResult, bool bDrawDebug = false) const;
	bool OverlapCheck(const UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QueryParams, FHitResult DownCheckHitResult, FVector& OverlapLocation, bool bDrawDebug = false) const;
};
