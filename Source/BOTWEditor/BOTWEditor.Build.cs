// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BOTWEditor : ModuleRules
{
	public BOTWEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { "BOTWEditor/Public" });
        PrivateIncludePaths.AddRange(new string[] { "BOTWEditor/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "BOTW", "Core", "CoreUObject", "Engine", "InputCore", "BlueprintGraph", "UnrealEd", "KismetCompiler" });
		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });

		PublicIncludePaths.AddRange(new string[] { "BOTWEditor", "BOTW" });
    }
}
