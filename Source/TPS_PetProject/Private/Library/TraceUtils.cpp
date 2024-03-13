#include "../../Public/Library/TraceUtils.h"

bool TraceUtils::SweepLineTraceSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
	const FVector& End,ECollisionChannel TraceChannel, const FCollisionQueryParams& Params,
	const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	
	const bool bResult = World->LineTraceSingleByChannel(OutHit, Start, End, TraceChannel, Params, ResponseParam);
	
#if ENABLE_DRAW_DEBUG
	if(bDrawDebug)
	{
		DrawDebugLine(World, Start, End, TraceColor, false, DrawTime, 0, 1.f); 

		if(bResult)
			DrawDebugPoint(World, OutHit.ImpactPoint, 60.f, HitColor, false, DrawTime);
	}
#endif
	
	return bResult;
}

bool TraceUtils::SweepCapsuleSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
	const FVector& End, float CapsuleRadius, float CapsuleHalfHeight, const FQuat& Rot,
	ECollisionChannel TraceChannel, const FCollisionQueryParams& Params,
	const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool bResult = World->SweepSingleByChannel(OutHit, Start, End, Rot, TraceChannel, CollisionShape, Params, ResponseParam);
	
#if ENABLE_DRAW_DEBUG
	if(bDrawDebug)
	{
		DrawDebugCapsule(World, Start, CapsuleHalfHeight, CapsuleRadius,FQuat::Identity, TraceColor, false, 2.f);
		DrawDebugCapsule(World, End, CapsuleHalfHeight, CapsuleRadius,FQuat::Identity, TraceColor, false, 2.f);
		DrawDebugLine(World, Start, End, FColor::Green, false, 2.f); 

		if(bResult)
		{
			DrawDebugCapsule(World, OutHit.Location, CapsuleHalfHeight, CapsuleRadius,FQuat::Identity, HitColor, false, 2.f);
			DrawDebugPoint(World, OutHit.ImpactPoint, 30.f, HitColor, false, 2.f);
		}
	}
#endif
	
	return bResult;
}

bool TraceUtils::SweepSphereSingleByChanel(const UWorld* World, FHitResult& OutHit, const FVector& Start,
	const FVector& End, float Radius, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params,
	const FCollisionResponseParams& ResponseParam, bool bDrawDebug, float DrawTime, FColor TraceColor, FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(Radius);
	const bool bResult = World->SweepSingleByChannel(OutHit, Start, End, FQuat::Identity, TraceChannel, CollisionShape, Params, ResponseParam);

#if ENABLE_DRAW_DEBUG
	if(bDrawDebug)
	{
		const FVector DebugDrawCapsuleLocation = (Start + End) * 0.5f;
		const FVector TraceVector = End - Start;
		const float  DebugDrawCapsuleHalfHeight = TraceVector.Size() * 0.5f;
		const FQuat DebugCapsuleRotation = FRotationMatrix::MakeFromZ(TraceVector).ToQuat();
		
		DrawDebugCapsule(World, DebugDrawCapsuleLocation, DebugDrawCapsuleHalfHeight, Radius, DebugCapsuleRotation, TraceColor, false, DrawTime);
	
		if(bResult)
		{
			DrawDebugSphere(World, OutHit.Location, Radius, 16, HitColor, false, DrawTime);
			DrawDebugPoint(World, OutHit.ImpactPoint, 10.0f, HitColor, false, DrawTime);
		}
	}
#endif
	
	return bResult;
}

bool TraceUtils::OverlapCapsuleAnyByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius,
	float CapsuleHalfHeight, const FQuat& Rotation, const FName& ProfileName, const FCollisionQueryParams& QueryParams,
	bool bDrawDebug, float DrawTime, FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool bResult = World->OverlapAnyTestByProfile(Pos, Rotation, ProfileName, CollisionShape, QueryParams);

#if ENABLE_DRAW_DEBUG
	if(bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Pos, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor, false, DrawTime);
	}
#endif
	
	return bResult;
}

bool TraceUtils::OverlapBlockingTestByProfile(const UWorld* World, const FVector& Pos, float CapsuleRadius,
	float CapsuleHalfHeight, const FQuat& Rotation, const FName& ProfileName, const FCollisionQueryParams& QueryParams,
	bool bDrawDebug, float DrawTime, FColor HitColor)
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	const bool bResult = World->OverlapBlockingTestByProfile(Pos, Rotation, ProfileName, CollisionShape, QueryParams);

#if ENABLE_DRAW_DEBUG
	if(bDrawDebug && bResult)
	{
		DrawDebugCapsule(World, Pos, CapsuleHalfHeight, CapsuleRadius, Rotation, HitColor, false, DrawTime);
	}
#endif
	
	return bResult;
}