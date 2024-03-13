// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TPS_GameInstance.generated.h"


UCLASS()
class TPS_PETPROJECT_API UTPS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	 virtual bool ProcessConsoleExec(const TCHAR *Cmd, FOutputDevice &Ar, UObject *Executor) override;

};
