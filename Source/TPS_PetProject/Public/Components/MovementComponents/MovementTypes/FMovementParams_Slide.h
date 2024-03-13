#pragma once

#include "CoreMinimal.h"


struct FMovementParams_Slide
{
	
	UCurveFloat* TimeLineCurve {};
	float BaseCapsuleHalfHeight {};
	
	float SlideBaseCapsuleHalfHeight {};
	float BaseMeshLocationZ {};
	
	float SlideMeshLocationZ {};
};

