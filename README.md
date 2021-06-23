# Procedural-Map-Creation-Plugin
Source code for a plugin for UE4 that is designed to generate a terrain based on a height map then texture and dress the surface of the terrain based on a color map.

The primary body for the tool window and it's components is in /Private/ProceduralMapGenerationPluginEdModeToolkit.cpp

The .uassets and .umap provided are the heightmaps and level where the generation takes place.

ATerrainActor_BP.uasset is required with the plugin as that's what provides the blueprint to reach the ProceduralMeshComponent API allowing me to leverage their Tangent and Normal Calculation.
