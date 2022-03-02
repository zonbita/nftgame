// Copyright June Rhodes. MIT Licensed.

using UnrealBuildTool;

public class ExampleOSSDeveloper : ModuleRules
{
    public ExampleOSSDeveloper(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystemUtils" });

        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "ExampleOSSEarlyConfig", "Http", "Json", "Gauntlet", "AutomationController", "AssetRegistry", "OnlineSubsystemRedpointEOS" });
    }
}