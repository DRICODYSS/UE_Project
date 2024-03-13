// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System;

public class TPS_PetProject : ModuleRules
{
    public TPS_PetProject(ReadOnlyTargetRules Target) : base(Target)
    {
        Console.WriteLine("TPS module rules ---------------->");
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core", 
            "CoreUObject", 
            "Engine", 
            "InputCore", 
            "NavigationSystem", 
            "AIModule", 
            "GameplayTasks", 
            "PhysicsCore", 
            "Niagara", 
            "EnhancedInput",
            "GameplayAbilities",
            "GameplayTags",
            "CableComponent", 
            "Niagara", 
            "PhysicsCore"
        });
    }
}
