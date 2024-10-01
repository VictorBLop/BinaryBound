// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using UnrealBuildTool.Rules;

public class Cyber : ModuleRules
{
	public Cyber(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"EnhancedInput",
				"UMG",
				"Cyber",
				"GameplayAbilities",
				"GameplayTags",
				"GameplayTasks",
				"OnlineSubsystem",
				"OnlineSubsystemUtils",
                "OnlineSubsystemEOS",
                "UMG",
                "Niagara",
                "Networking",
                "Sockets",
				"ApplicationCore", 
				"Slate", 
				"SlateCore", 
				"EngineSettings"
			});
	}
}
