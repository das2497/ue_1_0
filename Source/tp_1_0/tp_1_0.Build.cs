// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class tp_1_0 : ModuleRules
{
	public tp_1_0(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
