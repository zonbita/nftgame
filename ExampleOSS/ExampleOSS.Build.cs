// Copyright June Rhodes. MIT Licensed.

using UnrealBuildTool;

public class ExampleOSS : ModuleRules
{
    public ExampleOSS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystemUtils", "DeveloperSettings", "VoiceChat" });

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "ExampleOSSEarlyConfig", "Http", "Json" });
    }
}