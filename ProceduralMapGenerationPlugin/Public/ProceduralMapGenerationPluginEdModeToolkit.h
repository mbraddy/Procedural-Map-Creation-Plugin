// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"

class FProceduralMapGenerationPluginEdModeToolkit : public FModeToolkit
{
public:

	FProceduralMapGenerationPluginEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }
	FReply OnButtonClick();

private:
	void GenerateTerrain();
	void SetHeightMapFile(const FText&, ETextCommit::Type TextType);
	void SetBitMapFile(const FText&, ETextCommit::Type TextType);


	//	TODO :: Need to attach this to an enum later for different resolutions
	size_t m_ResolutionSize = 1024;	

	//	TODO :: Allow Height declaration from the tool and an offset for negatives
	float m_TerrainMaxHeight = 100000.0f;	
	float m_XScale = 100.0f;
	float m_YScale = 100.0f;

	FText m_HeightMapPath;
	FText m_BitMapPath;

	TSharedPtr<SWidget> ToolkitWidget;
};
