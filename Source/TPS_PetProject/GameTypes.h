#pragma once

#define ECC_CLIMBING           ECC_GameTraceChannel1
#define ECC_INTERACTION_VOLUME ECC_GameTraceChannel2
#define ECC_WALL_RUNNING       ECC_GameTraceChannel3

const FName CollisionProfilePawnName				  = "Pawn";
const FName CollisionProfilePawnInteractionVolumeName = "PawnInteractionVolume";

const FName DebugCategoryLedgeDetection          = "LedgeDetection";
const FName DebugCategoryRunnableWallDescription = "RunnableWallDescription";
const FName DebugCategorySlideDescription        = "SlideDescription";
const FName DebugCategoryCharacterAttributes     = "CharacterAttributes";

//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("_ %.f"), _));
//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, FString::Printf(TEXT("_")));