// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BOTW : ModuleRules
{
	public BOTW(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "GameplayTasks", "GameplayAbilities", "GameplayTags", "Niagara", 
															"EnhancedInput", "AIModule", "UMG", "MoviePlayer", "Paper2D", "MotionWarping","Landscape", "PhysicsCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {"FMODStudio"});

		PublicIncludePaths.AddRange(new string[] { "BOTW" });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
