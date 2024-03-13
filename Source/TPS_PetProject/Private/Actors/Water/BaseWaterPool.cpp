// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Water/BaseWaterPool.h"

#include "Actors/Water/WaterPhysicsVolume.h"
#include "Components/BoxComponent.h"
#include "Components/MovementComponents/SwimmingSystemComponent.h"



const FName CollisionProfilePawnInteractionVolume = FName("PawnInteractionVolume");

ABaseWaterPool::ABaseWaterPool()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("WaterRoot");

	WaterSurface = CreateDefaultSubobject<UStaticMeshComponent>("WaterSurface");
	WaterSurface->SetupAttachment(RootComponent);
	
	WaterVolumeBox = CreateDefaultSubobject<UBoxComponent>("WaterVolumeBox");
	WaterVolumeBox->SetupAttachment(RootComponent);
	WaterVolumeBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	WaterVolumeBox->SetGenerateOverlapEvents(true);

	WaterPhysicsVolume = CreateDefaultSubobject<AWaterPhysicsVolume>("WaterPhysicsVolume");
}

void ABaseWaterPool::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);
	
	WaterVolumeBox->SetBoxExtent(WaterVolumeSize);
	WaterVolumeBox->SetRelativeLocation(FVector(0.f));
	
	WaterSurface->SetRelativeLocation(FVector(WaterSurface->GetRelativeLocation().X, WaterSurface->GetRelativeLocation().Y, WaterVolumeSize.Z));
	WaterSurface->SetRelativeScale3D(FVector(WaterVolumeSize.X / 50.f, WaterVolumeSize.Y / 50.f, 1.f));
}

void ABaseWaterPool::BeginPlay()
{
	Super::BeginPlay();
	
	if(IsValid(WaterVolumeBox))
	{
		WaterVolumeBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseWaterPool::HandleWaterVolumeOverlapBegin);
		WaterVolumeBox->OnComponentEndOverlap.AddDynamic(this, &ABaseWaterPool::HandleWaterVolumeOverlapEnd);
	}
	
	WaterPhysicsVolume = GetWorld()->SpawnActor<AWaterPhysicsVolume>(AWaterPhysicsVolume::StaticClass(), GetActorLocation(), FRotator::ZeroRotator);
	if(IsValid(WaterPhysicsVolume))
		WaterPhysicsVolume->GetBoxComponent()->SetBoxExtent(FVector(WaterVolumeBox->GetUnscaledBoxExtent() * WaterVolumeBox->GetComponentTransform().GetScale3D()));
}

void ABaseWaterPool::HandleWaterVolumeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	const AActor* Player = OtherActor;
	if (!IsValid(Player)) return;

	const auto ActorComponent = Player->GetComponentByClass(USwimmingSystemComponent::StaticClass());
	const auto SwimmingSystemComponent = Cast<USwimmingSystemComponent>(ActorComponent);
	if(!IsValid(SwimmingSystemComponent)) return;

	if(SwimmingSystemComponent->RegisterInteractiveActor(this))	
		SwimmingSystemComponent->StartSwimmingLogic();
}

void ABaseWaterPool::HandleWaterVolumeOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	const AActor* Player = OtherActor;
	if (!IsValid(Player)) return;

	const auto ActorComponent = Player->GetComponentByClass(USwimmingSystemComponent::StaticClass());
	const auto SwimmingSystemComponent = Cast<USwimmingSystemComponent>(ActorComponent);
	if(!IsValid(SwimmingSystemComponent)) return;
	
	SwimmingSystemComponent->UnregisterInteractiveActor();
}

float ABaseWaterPool::GetWaterSurfaceLocationZ() const
{
	return WaterSurface->GetComponentLocation().Z;
}

void ABaseWaterPool::ChangeWaterPhysicsVolumeParam(bool bValue) const
{
	if(bValue) WaterPhysicsVolume->SetWaterPhysics(271.f, 4.f, true);
	else WaterPhysicsVolume->SetWaterPhysics();
}