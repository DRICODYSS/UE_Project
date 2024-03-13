#include "Actors/Water/WaterPhysicsVolume.h"

#include "Components/BoxComponent.h"


AWaterPhysicsVolume::AWaterPhysicsVolume(const FObjectInitializer &ObjectInitializer)
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(10.0f));
	BoxComponent->SetRelativeLocation(FVector::ZeroVector);
	SetRootComponent(BoxComponent);
}

void AWaterPhysicsVolume::SetWaterPhysics(const float NewTerminalVelocity, const float NewFluidFriction, const bool bNewBWaterVolume)
{
	TerminalVelocity = NewTerminalVelocity;
	FluidFriction = NewFluidFriction;
	bWaterVolume = bNewBWaterVolume;
}
