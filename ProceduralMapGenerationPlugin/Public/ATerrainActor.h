// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ATerrainActor.generated.h"

UCLASS()
class PROCEDURALMAPGENERATIONPLUGIN_API AATerrainActor : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AATerrainActor();

	UFUNCTION(BlueprintCallable, Category = General)
	static UMaterialInstanceDynamic* ApplyMasks(TArray<UTexture2D*> Textures, TArray<UTexture2D*> Masks , UObject* Object);

	UFUNCTION(BlueprintCallable, Category = General)
	static TArray<UTexture2D*> CreateMasks(UTexture2D* Texture, TArray<FLinearColor> Colors);

	UFUNCTION(BlueprintCallable, Category = General)
	static TArray<FVector> GetPixels(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = General)
	static TArray<FVector2D> GetUVs(UTexture2D* Texture);

	UFUNCTION(BlueprintCallable, Category = General)
	static TArray<UTexture2D*> GetMaskPackages();

	/*static TMap<int, FName> InitializeLookupTable(int TableSelector);*/

	static float m_TerrainMaxHeight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	static const TMap<int, FName> TextureParamNameTable;
	static const TMap<int, FName> MaskParamNameTable;
	static const TMap<int, FString> MaskFileNameTable;
	static const TMap<int, FName> OutputFileName;
	static TArray<UPackage*> CreatedPackages;

};
