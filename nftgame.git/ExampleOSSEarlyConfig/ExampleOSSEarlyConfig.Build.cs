// Copyright June Rhodes. MIT Licensed.

using UnrealBuildTool;

public class ExampleOSSEarlyConfig : ModuleRules
{
    public ExampleOSSEarlyConfig(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });
    }
}