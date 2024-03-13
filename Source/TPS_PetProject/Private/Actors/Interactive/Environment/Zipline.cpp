// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactive/Environment/Zipline.h"

#include "Character/TPS_BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "TPS_PetProject/GameTypes.h"
#include <cmath>


AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("ZiplineRoot");

	PillarStart = CreateDefaultSubobject<UStaticMeshComponent>("PillarStart");
	PillarStart->SetupAttachment(RootComponent);

	PillarEnd = CreateDefaultSubobject<UStaticMeshComponent>("PillarEnd");
	PillarEnd->SetupAttachment(RootComponent);

	CableMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cable");
	CableMesh->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>("InteractionVolume");
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolumeName);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	PillarStart->UpdateComponentToWorld();
	
	PillarEnd->SetRelativeLocation(EndPoint);
	PillarEnd->SetRelativeRotation(FRotator(PillarStart->GetRelativeRotation().Pitch, PillarStart->GetRelativeRotation().Yaw + 180.f, PillarStart->GetRelativeRotation().Roll));
	PillarEnd->UpdateComponentToWorld();
	
	SetLocationForInteractiveCable();
	SetRotationForInteractiveCable();
	SetScaleForInteractiveCable();
}

//* AInteractiveActor *//
void AZipline::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnInteractionVolumeOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if(!OtherActor->IsA<ATPS_BaseCharacter>()) return;
	
	ATPS_BaseCharacter* Player = StaticCast<ATPS_BaseCharacter*>(OtherActor);
	Player->TryChangeState_InteractWithZipline();
	
	
}

void AZipline::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnInteractionVolumeOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if(!OtherActor->IsA<ATPS_BaseCharacter>()) return;
	
	ATPS_BaseCharacter* Player = StaticCast<ATPS_BaseCharacter*>(OtherActor);
	Player->TryChangeState_InteractWithZipline(false);
}

auto AZipline::GetZiplineInteractionCapsule() const -> UCapsuleComponent *
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

#pragma region SecondaryFunc
void AZipline::SetLocationForInteractiveCable() const
{
	FVector NewLocationForInteractiveCable = PillarStart->GetRelativeLocation();
	NewLocationForInteractiveCable.Z = GetRelativeSocketLocation(PillarStart, SocketAttachPointName).Z;
	
	GetZiplineInteractionCapsule()->SetRelativeLocation(NewLocationForInteractiveCable + EndPoint * 0.5);
						 CableMesh->SetRelativeLocation(NewLocationForInteractiveCable + EndPoint * 0.5);
}

void AZipline::SetRotationForInteractiveCable()
{
	FRotator InteractionCableNewRotation = FindRotationBetweenPoints(PillarStart->GetSocketLocation(SocketAttachPointName), PillarEnd->GetSocketLocation(SocketAttachPointName));
	CableMesh->SetWorldRotation(InteractionCableNewRotation);
	
	InteractionCableNewRotation.Pitch -= 90;
	GetZiplineInteractionCapsule()->SetWorldRotation(InteractionCableNewRotation);
	ZiplineRotation = InteractionCableNewRotation;
}

void AZipline::SetScaleForInteractiveCable() const
{
	FVector InteractionNewSize = PillarStart->GetSocketLocation(SocketAttachPointName) - PillarEnd->GetSocketLocation(SocketAttachPointName);
	
	GetZiplineInteractionCapsule()->SetCapsuleSize(InteractionVolumeRadius, InteractionNewSize.Length() * 0.5 + InteractionVolumeDelta);
						 CableMesh->SetWorldScale3D(FVector(InteractionNewSize.Length() * 0.01f + 0.1f, 1.f, 1.f));
}

FRotator AZipline::FindRotationBetweenPoints(const FVector& Start, const FVector& End)
{
	FVector Direction = (End - Start).GetSafeNormal();
	FRotator Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	return Rotation;
}

FVector AZipline::GetRelativeSocketLocation(const UStaticMeshComponent *PillarMesh, FName SocketName)
{
	return PillarMesh->GetComponentTransform().InverseTransformPosition(PillarMesh->GetSocketLocation(SocketName));
}
#pragma endregion