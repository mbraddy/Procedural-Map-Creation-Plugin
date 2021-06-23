// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralMapGenerationPluginEdModeToolkit.h"
#include "ProceduralMapGenerationPluginEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Input/SEditableText.h"
#include "Misc/FileHelper.h"
//	#include "ProceduralMeshComponent.h"
//	#include "KismetProceduralMeshLibrary.h"
#include "ATerrainActor.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "FProceduralMapGenerationPluginEdModeToolkit"

FProceduralMapGenerationPluginEdModeToolkit::FProceduralMapGenerationPluginEdModeToolkit()
{
}

void FProceduralMapGenerationPluginEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	//	Establish the Border/Window for the Tool	
	SAssignNew(ToolkitWidget, SBorder)
	.Padding(2)
	.IsEnabled(true)
	[
		//	Window Vertical Box
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			//	Expandable area for the Window Detail header
			SNew(SExpandableArea)
			.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0.5, 0.5, 0.5)))
			.BorderBackgroundColor(FSlateColor(FLinearColor(0.3, 0.3, 0.3)))
			.InitiallyCollapsed(false)
			.Padding(8.0f)
			.HeaderContent()
			[
				//	Header for the Landscape Generator category block
				SNew(STextBlock)
				.Text(NSLOCTEXT("ProceduralToolWindow", "LandscapeGeneratorCategoryHeader", "Landscape Generation Tool"))
			]
			.BodyContent()
			[
				//	Wrapper for the Text Block
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				.Padding(10)
				[
					SNew(STextBlock)
					.AutoWrapText(true)
					.Text(LOCTEXT("HelperLabel", "This tool will generate a terrain mesh based on a heightmap that is passed in via the Heightmap field."))
				]
			]
		]

		//	Heightmap Category Block
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			//	Category Block Definition
			SNew(SExpandableArea)
			.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0.5, 0.5, 0.5)))
			.BorderBackgroundColor(FSlateColor(FLinearColor(0.3, 0.3, 0.3)))
			.InitiallyCollapsed(false)
			.Padding(8.0f)
			.HeaderContent()
			[
				//	Header for the Heightmap category block
				SNew(STextBlock)
				.Text(NSLOCTEXT("ProceduralToolWindow", "HeightMapCategoryHeader", "Heightmap Information"))
			]
			.BodyContent()
			[
				//	Splitter for resizing horizontally
				SNew(SSplitter)

				//	Heightmap input line
				+ SSplitter::Slot()
				[
					//	Heightmap File descriptor
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("ProceduralToolWindow", "HeightmapTag", "Heightmap file"))
					]
				]
						
				//	Heightmap Text field
				+ SSplitter::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(SEditableTextBox)
						.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FProceduralMapGenerationPluginEdModeToolkit::SetHeightMapFile))
					]
				]
			]
		]

		//	Bitmap Category Block
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			//	Category Block Definition
			SNew(SExpandableArea)
			.BodyBorderBackgroundColor(FSlateColor(FLinearColor(0.5, 0.5, 0.5)))
			.BorderBackgroundColor(FSlateColor(FLinearColor(0.3, 0.3, 0.3)))
			.InitiallyCollapsed(false)
			.Padding(8.0f)
			.HeaderContent()
			[
				//	Header for the Bitmap category block
				SNew(STextBlock)
				.Text(NSLOCTEXT("ProceduralToolWindow", "BitMapCategoryHeader", "Bitmap Information"))
			]
			.BodyContent()
			[
				//	Splitter for resizing horizontally
				SNew(SSplitter)

				//	Bitmap input line
				+ SSplitter::Slot()
				[
					//	Bitmap File descriptor
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(STextBlock)
						.Text(NSLOCTEXT("ProceduralToolWindow", "BitmapTag", "Bitmap file"))
					]
				]

					//	Bitmap Text field
				+ SSplitter::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					[
						SNew(SEditableTextBox)
						.OnTextCommitted(FOnTextCommitted::CreateSP(this, &FProceduralMapGenerationPluginEdModeToolkit::SetBitMapFile))
					]
				]
			]
		]

		//	Generation Button
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.AutoHeight()
		[
			SNew(SButton)
			.OnClicked(this, &FProceduralMapGenerationPluginEdModeToolkit::OnButtonClick)
			.Text(LOCTEXT("GenerateButtonLabel", "Generate Terrain"))
		]
	];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FProceduralMapGenerationPluginEdModeToolkit::GetToolkitFName() const
{
	return FName("ProceduralMapGenerationPluginEdMode");
}

FText FProceduralMapGenerationPluginEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("ProceduralMapGenerationPluginEdModeToolkit", "DisplayName", "ProceduralMapGenerationPluginEdMode Tool");
}

class FEdMode* FProceduralMapGenerationPluginEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FProceduralMapGenerationPluginEdMode::EM_ProceduralMapGenerationPluginEdModeId);
}

void FProceduralMapGenerationPluginEdModeToolkit::SetHeightMapFile(const FText& input, ETextCommit::Type TextType)
{
	m_HeightMapPath = input;
}

void FProceduralMapGenerationPluginEdModeToolkit::SetBitMapFile(const FText& input, ETextCommit::Type TextType)
{
	m_BitMapPath = input;
}

void FProceduralMapGenerationPluginEdModeToolkit::GenerateTerrain()
{
	//	Populate the array from the heightmap
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	TArray<uint8> FileContent;

	if (FileManager.FileExists(*(m_HeightMapPath.ToString())))
	{
		//	Populate the array of bytes with the raw heightmap file information.
		if (FFileHelper::LoadFileToArray(FileContent, *(m_HeightMapPath.ToString())))
		{
			TArray<FVector> Vertices;
			TArray<FVector2D> UVs;
			TArray<int32> Triangles;

			TArray<float> HeightMapValues;

			uint16 PixelColorBuffer = 0x0000;
			uint16 MaxPixelValue16Bit = 0xFFFF;
			uint64 PixelCount = m_ResolutionSize * m_ResolutionSize;
			//	Adjust for two bytes per pixel - 16 bit color. Every two bytes = 1 pixel's color information.
			uint64 ByteCount = PixelCount * 2;

			//	Establish the vertex height values
			for (size_t i = 0; i < ByteCount; ++i)
			{
				if (i % 2 == 0)
				{
					PixelColorBuffer = PixelColorBuffer | FileContent[i];
				}
				else
				{
					PixelColorBuffer = PixelColorBuffer << 8;
					PixelColorBuffer = PixelColorBuffer | FileContent[i];

					//	Save off the heightmap vertex values for each pixel
					HeightMapValues.Add(((float)PixelColorBuffer / (float)MaxPixelValue16Bit) * m_TerrainMaxHeight);
					PixelColorBuffer = 0x0000;
				}
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("End of Heightmap population"));

			//	Populate the Vertices array and the UVs Array
			size_t index = 0;
			for (size_t i = 0; i < m_ResolutionSize; ++i)
			{
				for (size_t j = 0; j < m_ResolutionSize; ++j)
				{
					Vertices.Add(FVector((i * m_XScale), (j * m_YScale), HeightMapValues[index]));
					++index;
					UVs.Add(FVector2D(((float)i / (float)m_ResolutionSize), ((float)j / (float)m_ResolutionSize)));

					size_t MaxMinusOne = m_ResolutionSize - 1;
					size_t CurrentVertex = 0;

					//	Dont add triangles if on the last column (since we are building from Left to Right)
					if (j != MaxMinusOne)
					{
						//	Update the current Vertex
						CurrentVertex = i + j;

						//	Add Triangles "downward" if not on the last row
						if (i != MaxMinusOne)
						{
							Triangles.Add(CurrentVertex);
							Triangles.Add(CurrentVertex + 1);
							Triangles.Add(CurrentVertex + m_ResolutionSize);
						}

						//	Add Triangles "upwards" filling in the row of quads if not on the first row
						if (i != 0)
						{
							Triangles.Add(CurrentVertex);
							Triangles.Add(CurrentVertex + 1);
							Triangles.Add((CurrentVertex + 1) - m_ResolutionSize);
						}
					}
				}
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString("End of Vertex, Triangles, and UV population"));
			//	Draw the terrain based on the verts

			//GenerateTangentsNormalsAndMesh(Vertices, UVs, Triangles);
		}
	}
	else
	{
		//	Error reporting / File input argument validation.
		UE_LOG(LogTemp, Error, TEXT("FileManipulation: ERROR: Unable to access Heightmap file."));
		UE_LOG(LogTemp, Error, TEXT("FileManipulation: Expected Location: %s"), *(m_HeightMapPath.ToString()));
	}
}

FReply FProceduralMapGenerationPluginEdModeToolkit::OnButtonClick()
{
	// Let editor know that we're about to do something that we want to undo/redo
	GEditor->BeginTransaction(LOCTEXT("GenerateTerrainTransactionName", "GenerateTerrain"));

	//	Create the Terrain's mesh
	GenerateTerrain();
	//	Texture Terrain

	//	Generate Set Dressing

	// We're done moving actors so close transaction
	GEditor->EndTransaction();

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
