using System;
using System.IO;
using UnrealBuildTool;

public class BoostFPM : ModuleRules
{
    public BoostFPM(ReadOnlyTargetRules Target) : base(Target)
    {
		PCHUsage = ModuleRules.PCHUsageMode.UseSharedPCHs;
        PrivatePCHHeaderFile = "Public/BoostFPM.h";
        
        bUseUnity = false;

        Type = ModuleType.External;

        // Add any include paths for the plugin
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
    }
}
