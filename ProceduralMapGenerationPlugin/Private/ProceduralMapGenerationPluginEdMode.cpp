// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralMapGenerationPluginEdMode.h"
#include "ProceduralMapGenerationPluginEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FProceduralMapGenerationPluginEdMode::EM_ProceduralMapGenerationPluginEdModeId = TEXT("EM_ProceduralMapGenerationPluginEdMode");

FProceduralMapGenerationPluginEdMode::FProceduralMapGenerationPluginEdMode()
{

}

FProceduralMapGenerationPluginEdMode::~FProceduralMapGenerationPluginEdMode()
{

}

void FProceduralMapGenerationPluginEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FProceduralMapGenerationPluginEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FProceduralMapGenerationPluginEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

bool FProceduralMapGenerationPluginEdMode::UsesToolkits() const
{
	return true;
}




