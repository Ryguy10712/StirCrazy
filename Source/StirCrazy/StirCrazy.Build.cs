// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class StirCrazy : ModuleRules
{
	public StirCrazy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent", "EnhancedInput", "HeadMountedDisplay"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		//Include Win64 Discord SDK 
        if (Target.Platform != UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Android) return;
        PublicIncludePaths.Add("U:\\StirCrazy\\Source\\StirCrazy\\Discord");
        
        PublicAdditionalLibraries.Add("E:\\StirCrazy\\Binaries\\Win64\\discord_game_sdk.dll.lib");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
