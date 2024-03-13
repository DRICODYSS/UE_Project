// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RunnableWallDetectorComponent.generated.h"

class UCapsuleComponent;

UENUM(BlueprintType)
enum EWallSide : uint8
{
	None = 0,
	Right,
	Left
};

USTRUCT(BlueprintType)
struct FRunnableWallDescription
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "RunnableWall description")
	FVector Location{FVector::ZeroVector};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "RunnableWall description")
	FRotator Rotation{FRotator::ZeroRotator};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "RunnableWall description")
	FVector WallNormal{FVector::ZeroVector};
	
	EWallSide WallSide = EWallSide::None;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_PETPROJECT_API URunnableWallDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool DetectRunnableWall(OUT FRunnableWallDescription& RunnableWallDescription);

	EWallSide GetISWallSide() const { return CurrentWallSide; }
	EWallSide GetISPreviousWallSide() const { return PreviousWallSide; }
	void ResetWallSide() { CurrentWallSide = PreviousWallSide; }
	void ClearWallSide() { CurrentWallSide = PreviousWallSide = None; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Description setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float TraceDistance = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	FColor TraceColor = FColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	FColor HitColor = FColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category =  "Debug setting", meta=(UIMin = 0.f, ClampMin = 0.f))
	float DrawTime = 2.0f;
	
private:
	bool GeneralDetect(FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent* CapsuleComponent, const FCollisionQueryParams& QueryParams, bool bDrawDebug = false);
	bool RightDetect  (FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent* CapsuleComponent, const FCollisionQueryParams& QueryParams, bool bDrawDebug = false);
	bool LeftDetect   (FRunnableWallDescription &RunnableWallDescription, const UCapsuleComponent* CapsuleComponent, const FCollisionQueryParams& QueryParams, bool bDrawDebug = false);
	
	bool WallCheck       (const UCapsuleComponent* CapsuleComponent, const FCollisionQueryParams& QueryParams, FHitResult& CheckHitResult, bool bWallSideIsLeft, bool bDrawDebug = false) const;

	static void SetRunnableWallDescription(FRunnableWallDescription &RunnableWallDescription, const FHitResult& HitResult, const EWallSide WallSide);

	TWeakObjectPtr<ACharacter> CachedCharacterOwner {nullptr};
	EWallSide CurrentWallSide = None;
	EWallSide PreviousWallSide = None;
};
