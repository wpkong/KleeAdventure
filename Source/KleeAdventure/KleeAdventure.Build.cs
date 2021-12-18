// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KleeAdventure : ModuleRules
{
	public KleeAdventure(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "UMG", "Engine", "InputCore", "HeadMountedDisplay", "AIModule"});
	}
}
