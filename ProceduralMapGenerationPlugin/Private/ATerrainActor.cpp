// Fill out your copyright notice in the Description page of Project Settings.


#include "ATerrainActor.h"

// Sets default values
AATerrainActor::AATerrainActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATerrainActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AATerrainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<FVector> AATerrainActor::GetPixels(UTexture2D* Texture)
{
	TArray<FVector> LandscapeVertexInformation;
	FTexture2DMipMap HeightMipMap =(Texture->PlatformData->Mips[0]);
	FByteBulkData* RawHeightmapData = &(HeightMipMap.BulkData);

	const FColor* FormattedImageData = static_cast<const FColor*>(RawHeightmapData->Lock(LOCK_READ_ONLY));
	if (FormattedImageData == nullptr)
	{
		UE_LOG(LogTemp, Error, L"Unable to open image data.");
	}


	int32 XDimension = Texture->PlatformData->Mips[0].SizeX;
	int32 YDimension = Texture->PlatformData->Mips[0].SizeY;

	for (int32 X = 0; X < XDimension; ++X)
	{
		for (int32 Y = 0; Y < YDimension; ++Y)
		{
			//	Get the individual Pixel Color
			FColor PixelColor = FormattedImageData[Y * XDimension + X];

			FLinearColor RGBPixelValue = (FLinearColor(PixelColor) / FLinearColor::White);
			float ValueAdjustedForContribution = (RGBPixelValue.R + RGBPixelValue.G + RGBPixelValue.B) / 3;
			
			LandscapeVertexInformation.Add(FVector((float)X * 10.0f, (float)Y * 10.0f, (ValueAdjustedForContribution * 1000.0f)));
		}
	}

	RawHeightmapData->Unlock();

	return TArray<FVector>(LandscapeVertexInformation);
}

TArray<FVector2D> AATerrainActor::GetUVs(UTexture2D* Texture)
{
	TArray<FVector2D> UVs;


	int32 XDimension = Texture->PlatformData->Mips[0].SizeX;
	int32 YDimension = Texture->PlatformData->Mips[0].SizeY;

	for (int32 X = 0; X < XDimension; ++X)
	{
		for (int32 Y = 0; Y < YDimension; ++Y)
		{
			UVs.Add(FVector2D(((float)X / (float)XDimension), ((float)Y / (float)YDimension)));
		}
	}

	return TArray<FVector2D>(UVs);
}