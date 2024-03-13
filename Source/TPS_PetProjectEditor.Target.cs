// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;
using System;

public class TPS_PetProjectEditorTarget : TargetRules
{
    public TPS_PetProjectEditorTarget(TargetInfo Target) : base(Target)
    {
        Console.WriteLine("TPS Target Editor ---------------->");
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V4;
        ExtraModuleNames.AddRange(new string[] { "TPS_PetProject" });
    }
}
