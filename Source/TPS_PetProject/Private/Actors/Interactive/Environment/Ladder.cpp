// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Environment/Ladder.h"

#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TPS_PetProject/GameTypes.h"

ALadder::ALadder()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("LadderRoot");
	
	LeftRailMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftRail");
	LeftRailMesh->SetupAttachment(RootComponent);

	RightRailMesh = CreateDefaultSubobject<UStaticMeshComponent>("RightRail");
	RightRailMesh->SetupAttachment(RootComponent);

	StepsMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>("Steps");
	StepsMesh->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>("InteractionVolume");
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolumeName);
	InteractionVolume->SetGenerateOverlapEvents(true);

	TopInteractionVolume = CreateDefaultSubobject<UBoxComponent>("TopInteractionVolume");
	TopInteractionVolume->SetupAttachment(RootComponent);
	TopInteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolumeName);
	TopInteractionVolume->SetGenerateOverlapEvents(true);
}

FVector ALadder::GetAttachFromTopAnimMontageStartingLocation() const
{
	FRotator OrientRotation = GetActorForwardVector().ToOrientationRotator();
	FVector Offset = OrientRotation.RotateVector(AttachFromAnimMontageInitialOffset);
	
	FVector LedderTop = GetActorLocation() + GetActorUpVector() * LadderHeight;
	return LedderTop + Offset;
}

void ALadder::OnConstruction(const FTransform &Transform)
{
	SetLocationAndRotationForRail(LeftRailMesh, true);
	SetLocationAndRotationForRail(RightRailMesh, false);

	SetRelativeScale3DForSteps();
	SetCurrentNumberOfSteps();

	const float BoxDepthExtent = GetLadderInteractionBox()->GetUnscaledBoxExtent().X;
	GetLadderInteractionBox()->SetBoxExtent(FVector(BoxDepthExtent, LadderWidth * 0.5f, LadderHeight * 0.5f));
	GetLadderInteractionBox()->SetRelativeLocation(FVector(BoxDepthExtent, 0.f, LadderHeight * 0.5f));

	const FVector TopBoxExtent = TopInteractionVolume->GetUnscaledBoxExtent();
	TopInteractionVolume->SetBoxExtent(FVector(TopBoxExtent.X, LadderWidth * 0.5f, TopBoxExtent.Z));
	TopInteractionVolume->SetRelativeLocation(FVector(-TopBoxExtent.X,0.0f,LadderHeight + TopBoxExtent.Z));
}

void ALadder::BeginPlay()
{
	Super::BeginPlay();
	if(IsValid(TopInteractionVolume))
	{
		TopInteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapBegin);
		TopInteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ALadder::OnInteractionVolumeOverlapEnd);
	}
}

auto ALadder::GetLadderInteractionBox() const -> UBoxComponent* { return StaticCast<UBoxComponent*>(InteractionVolume); }

void ALadder::SetLocationAndRotationForRail(UStaticMeshComponent *&RailMeshComponent, const bool SignBeforeY) const
{
	const float RelativeLocationY = LadderWidth * 0.5f * (SignBeforeY ? 1 : -1);
	RailMeshComponent->SetRelativeLocation(FVector(0.f, RelativeLocationY, LadderHeight * 0.5f));
	
	const UStaticMesh* CashRailMesh = RailMeshComponent->GetStaticMesh();
	if(IsValid(CashRailMesh))
	{
		const float MeshHeight = CashRailMesh->GetBoundingBox().GetSize().Z;
		if(!FMath::IsNearlyZero(MeshHeight))
			RailMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, LadderHeight / MeshHeight));
	}
}

void ALadder::SetRelativeScale3DForSteps() const
{
	const UStaticMesh* Steps = StepsMesh->GetStaticMesh();
	if(IsValid(Steps))
	{
		const float MeshWidth = Steps->GetBoundingBox().GetSize().Y;
		if(!FMath::IsNearlyZero(MeshWidth))
			StepsMesh->SetRelativeScale3D(FVector(1.f, LadderWidth / MeshWidth, 1.f));
	}
}

void ALadder::SetCurrentNumberOfSteps() const
{
	StepsMesh->ClearInstances();
	const uint32 StepsCount = FMath::FloorToInt((LadderHeight - BottomStepOffset) / StepsInterval);
	for(uint32 i = 0; i < StepsCount; ++i)
	{
		FTransform InstanceTransform(FVector(1.f, 1.f, BottomStepOffset + i * StepsInterval));
		StepsMesh->AddInstance(InstanceTransform);
	}
}

void ALadder::OnInteractionVolumeOverlapBegin(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OverlappedComponent == TopInteractionVolume) bIsOnTop = true;
}

void ALadder::OnInteractionVolumeOverlapEnd(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor,
	UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OverlappedComponent == TopInteractionVolume) bIsOnTop = false;
}
