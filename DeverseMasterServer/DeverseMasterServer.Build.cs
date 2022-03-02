// Copyright Cosugames, LLC 2021

using UnrealBuildTool;

public class DeverseMasterServer : ModuleRules
{
    public DeverseMasterServer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Settings"
        });
    }
}