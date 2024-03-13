
#include "Actors/Platforms/PlatformInvocator.h"

#include "Components/BoxComponent.h"
#include "TPS_PetProject/GameTypes.h"

APlatformInvocator::APlatformInvocator()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("LadderRoot");
	
	InteractionVolume = CreateDefaultSubobject<UBoxComponent>("InteractionVolume");
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolumeName);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void APlatformInvocator::CheckIsBound()
{
	if (OnInvocatorActivated.IsBound())
	{
		OnInvocatorActivated.Broadcast(bIsOn);
	}
}

void APlatformInvocator::OnInteractionVolumeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	bIsOn = true;
	CheckIsBound();
}

void APlatformInvocator::OnInteractionVolumeOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	bIsOn = false;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &APlatformInvocator::CheckIsBound, WorkingTime, false);
}

