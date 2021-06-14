// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralMapGenerationPlugin.h"
#include "ProceduralMapGenerationPluginEdMode.h"

#define LOCTEXT_NAMESPACE "FProceduralMapGenerationPluginModule"

void FProceduralMapGenerationPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FProceduralMapGenerationPluginEdMode>(FProceduralMapGenerationPluginEdMode::EM_ProceduralMapGenerationPluginEdModeId, LOCTEXT("ProceduralMapGenerationPluginEdModeName", "ProceduralMapGenerationPluginEdMode"), FSlateIcon(), true);
}

void FProceduralMapGenerationPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FProceduralMapGenerationPluginEdMode::EM_ProceduralMapGenerationPluginEdModeId);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralMapGenerationPluginModule, ProceduralMapGenerationPlugin)