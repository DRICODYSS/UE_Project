// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;
using System;

public class TPS_PetProjectTarget : TargetRules
{
    public TPS_PetProjectTarget(TargetInfo Target) : base(Target)
    {
        Console.WriteLine("TPS Target Game ---------------->");
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        ExtraModuleNames.AddRange(new string[] { "TPS_PetProject" });
    }
}
