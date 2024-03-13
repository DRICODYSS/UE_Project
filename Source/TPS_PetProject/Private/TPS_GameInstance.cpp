// Fill out your copyright notice in the Description page of Project Settings.


#include "TPS_GameInstance.h"


bool UTPS_GameInstance::ProcessConsoleExec(const TCHAR *Cmd, FOutputDevice &Ar, UObject *Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);

	if(!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for(auto Subsystem : Subsystems)
		{
			bResult |= Subsystem->ProcessConsoleExec(Cmd, Ar, Executor); 
		}
	}
	return bResult;
}
