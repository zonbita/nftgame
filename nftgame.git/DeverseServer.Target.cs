// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class DeverseServerTarget : TargetRules
{
	public DeverseServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
        bUseLoggingInShipping = true;
        ExtraModuleNames.Add("Deverse");
	}
}