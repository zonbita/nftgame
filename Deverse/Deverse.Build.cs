// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Deverse : ModuleRules
{
	public Deverse(ReadOnlyTargetRules Target) : base(Target)
	{
        PrivateDependencyModuleNames.AddRange(new string[] {"ConcertTransport"});
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bool bTargetConfig = Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test;
        if (Target.bBuildDeveloperTools || bTargetConfig)
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }

		PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay",
            "Json",
            "JsonUtilities",
            "Http",
            "RenderCore",
            "RHI",
            "UMG",
            "SlateCore",
            "DeverseMasterServer",
            "VoiceChat", 
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "RedpointEOSInterfaces",
            "ExampleOSS"
        });

        PrivateIncludePathModuleNames.AddRange(
        new string[] {
                "ImageWrapper",
                "TargetPlatform",
                "AnimatedTexture",
                "ReplicationGraph"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
        );
    }
}
