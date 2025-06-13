// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WildCard : ModuleRules
{
	public WildCard(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "AIModule", "GameplayTasks", "NavigationSystem", "NavMesh"});
	}
}
