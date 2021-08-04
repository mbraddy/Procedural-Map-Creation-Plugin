// Fill out your copyright notice in the Description page of Project Settings.

#include "ATerrainActor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "AssetRegistryModule.h"

const TMap<int, FName> AATerrainActor::TextureParamNameTable =
{
	{ 0, FName("ColorOneTexture") },
	{ 1, FName("ColorTwoTexture") },
	{ 2, FName("ColorThreeTexture") }
};

const TMap<int, FName> AATerrainActor::MaskParamNameTable =
{
	{ 0, FName("ColorOneMask") },
	{ 1, FName("ColorTwoMask") },
	{ 2, FName("ColorThreeMask") }
};

const TMap<int, FString> AATerrainActor::MaskFileNameTable =
{
	{ 0, FString("ColorOneMask") },
	{ 1, FString("ColorTwoMask") },
	{ 2, FString("ColorThreeMask") }
};

const TMap<int, FName> AATerrainActor::OutputFileName =
{
	{ 0, FName("COneMask") },
	{ 1, FName("CTwoMask") },
	{ 2, FName("CThreeMask") },
	{ 3, FName("EndOfList") }
};

TArray<UPackage*> AATerrainActor::CreatedPackages;

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

//TMap<int, FName> AATerrainActor::InitializeLookupTable(int TableSelector)
//{
//	TMap<int, FName> ConstructedMap;
//
//	TArray<FName> ParameterNames;
//	switch (TableSelector)
//	{
//	case 0:
//		ParameterNames.Add(FName("ColorOneTexture"));
//		ParameterNames.Add(FName("ColorTwoTexture"));
//		ParameterNames.Add(FName("ColorThreeTexture"));
//		break;
//
//	case 1:
//		ParameterNames.Add(FName("ColorOneMask"));
//		ParameterNames.Add(FName("ColorTwoMask"));
//		ParameterNames.Add(FName("ColorThreeMask"));
//		break;
//
//	default:
//		break;
//	}
//
//	int i = 0;
//
//	for (auto Value : ParameterNames)
//	{
//		ConstructedMap.Add(i, ParameterNames[i]);
//		++i;
//	}
//
//	return ConstructedMap;
//}

TArray<UTexture2D*> AATerrainActor::GetMaskPackages()
{
	TArray<UTexture2D*> RetVal;
	for (auto Package : CreatedPackages)
	{
		const TCHAR* Fpointer = *(Package->FileName.ToString());
		UTexture2D* ConvertedPackage = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, Fpointer));
		RetVal.Add(ConvertedPackage);
	}
	return RetVal;
}

//	Applies the Generated Masks to a Dynamic material instance based on the base material set on the actor
UMaterialInstanceDynamic* AATerrainActor::ApplyMasks(TArray<UTexture2D*> Textures, TArray<UTexture2D*> Masks, UObject* Object)
{
	AActor* TerrainActor = dynamic_cast<AActor*>(Object);
	if (TerrainActor == nullptr)
	{
		UE_LOG(LogTemp, Error, L"Unable to successfully cast UObject to AActor");
		return nullptr;
	}
	else if (Textures.Num() != Masks.Num())
	{
		UE_LOG(LogTemp, Error, L"Expecting the same number of textuers as masks. Is every color in the image defined?");
		return nullptr;
	}

	TArray<UActorComponent*> ProceduralMeshComponents = TerrainActor->GetComponentsByTag(UProceduralMeshComponent::StaticClass(), "Mesh");

	//	Get the Material Base assigned to the Procedural Mesh
	UProceduralMeshComponent* GeneratedMesh = static_cast<UProceduralMeshComponent*>(ProceduralMeshComponents[0]);
	UMaterialInterface* Material = GeneratedMesh->GetMaterial(0);

	//	Create a dynamic material instance and set it to the procedural mesh
	UMaterialInstanceDynamic* MatInstance = UMaterialInstanceDynamic::Create(Material, TerrainActor);
	MatInstance->ClearParameterValues();

	if (MatInstance != nullptr)
	{
		UE_LOG(LogTemp, Error, L"Dynamic Material Instance Created.");

		int NumberOfColors = Textures.Num();
		//	For each Color
		for (int i = 0; i < NumberOfColors; ++i)
		{
			UTexture2D* ColorMask = Masks[i];
			UTexture2D* ColorTexture = Textures[i];
			
			MatInstance->SetTextureParameterValue(TextureParamNameTable[i], ColorTexture);
			MatInstance->SetTextureParameterValue(MaskParamNameTable[i], ColorMask);
		}

		if (NumberOfColors != 0)
		{
			MatInstance->SetScalarParameterValue(FName("HeightValue"), 50.0f);
			MatInstance->SetScalarParameterValue(FName("IsDynamicDefined?"), 1.0f);
			MatInstance->SetScalarParameterValue(FName("DynamicDefined"), 1.0f);
			GeneratedMesh->SetMaterial(0, MatInstance);
		}
	}

	return MatInstance;
}

TArray<UTexture2D*> AATerrainActor::CreateMasks(UTexture2D* Texture, TArray<FLinearColor> Colors)
{
	TArray<UTexture2D*> ColoredMaskInformation;
	FTexture2DMipMap CombinedMipMap = (Texture->PlatformData->Mips[0]);
	FByteBulkData* RawCombinedData = &(CombinedMipMap.BulkData);

	const FColor* FormattedImageData = static_cast<const FColor*>(RawCombinedData->Lock(LOCK_READ_ONLY));
	if (FormattedImageData == nullptr)
	{
		UE_LOG(LogTemp, Error, L"Unable to open image data.");
	}

	int32 XDimension = Texture->PlatformData->Mips[0].SizeX;
	int32 YDimension = Texture->PlatformData->Mips[0].SizeY;

	FString PackageName = TEXT("/Game/ProceduralTextures/");
	FString TempPackageName = TEXT("");

	int TableIndex = 0;

	//	For each color contained in the image (Passed as arg)
	for (auto Color : Colors)
	{
		if (TableIndex >= Colors.Num())
		{
			continue;
		}

		TempPackageName += PackageName;
		TempPackageName += MaskFileNameTable[TableIndex];
		UPackage* Package = CreatePackage(*TempPackageName);
		Package->FullyLoad();

		UTexture2D* NewTexture = NewObject<UTexture2D>(Package, OutputFileName[TableIndex], RF_Public | RF_Standalone | RF_MarkAsRootSet);

		NewTexture->AddToRoot();				// This line prevents garbage collection of the texture
		NewTexture->PlatformData = new FTexturePlatformData();	// Then we initialize the PlatformData
		NewTexture->PlatformData->SizeX = XDimension;
		NewTexture->PlatformData->SizeY = YDimension;
		NewTexture->PlatformData->SetNumSlices(1);
		NewTexture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;

		//	Create the new Mask
		UTexture2D* ColorTextureMap = UTexture2D::CreateTransient(XDimension, YDimension, EPixelFormat::PF_FloatRGBA, FName(Color.ToString()));

		//	Access the bulk data
		FTexture2DMipMap ColorMipMap = (ColorTextureMap->PlatformData->Mips[0]);
		FByteBulkData* RawMaskData = &(ColorMipMap.BulkData);

		//	Lock the data for read/write
		FColor* FormattedMaskData = static_cast<FColor*>(RawMaskData->Lock(LOCK_READ_WRITE));
		if (FormattedMaskData == nullptr)
		{
			UE_LOG(LogTemp, Error, L"Unable to open generated mask data.");
		}

#pragma region Object Saving
		uint8* Pixels = new uint8[XDimension * YDimension * 4];
		for (int32 y = 0; y < YDimension; y++)
		{
			for (int32 x = 0; x < XDimension; x++)
			{
				int32 curPixelIndex = ((y * XDimension) + x);

				//	Get the individual Pixel Color
				FColor PixelColor = FormattedImageData[y * XDimension + x];

				//	Created this solely for debugging in the watch window
				FLinearColor RGBColor = FLinearColor(PixelColor);
				//	Alphas are unused, so making sure they don't affect the comparison.
				RGBColor.A = Color.A;

				//	If the color matches a pixel
				if (RGBColor == Color)
				{
					//	Mark the pixel as white, but scale it based on the alpha of the pixel
					Pixels[4 * curPixelIndex] = 255;
					Pixels[4 * curPixelIndex + 1] = 255;
					Pixels[4 * curPixelIndex + 2] = 255;
					Pixels[4 * curPixelIndex + 3] = 255;
				}
				else
				{
					//	Mark the pixel black for the mask
					Pixels[4 * curPixelIndex] = 0;
					Pixels[4 * curPixelIndex + 1] = 0;
					Pixels[4 * curPixelIndex + 2] = 0;
					Pixels[4 * curPixelIndex + 3] = 255;
				}
			}
		}

		// Allocate first mipmap.
		FTexture2DMipMap TempMap;
		int32 Index = NewTexture->PlatformData->Mips.Add(std::move(&TempMap));
		FTexture2DMipMap* Mip = &(NewTexture->PlatformData->Mips[Index]);
		Mip->SizeX = XDimension;
		Mip->SizeY = YDimension;

		// Lock the texture so it can be modified
		Mip->BulkData.Lock(LOCK_READ_WRITE);
		uint8* TextureData = (uint8*)Mip->BulkData.Realloc(XDimension * YDimension * 4);
		FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * YDimension * XDimension * 4);
		Mip->BulkData.Unlock();

		NewTexture->Source.Init(XDimension, YDimension, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);

		//	Busy waiting for thread synchronization
		for(int i = 0; i < INT32_MAX; ++i)
		{
			//	Waiting for sync
		}

		NewTexture->UpdateResource();
		

		Package->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(NewTexture);

		FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		bool bSaved = UPackage::SavePackage(Package, NewTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, true, true, SAVE_NoError);

		CreatedPackages.Add(Package);
		
		delete[] Pixels;	// Don't forget to free the memory here

		//	Loop Reset
		TableIndex++;
		TempPackageName = TEXT("");
#pragma endregion

		//	Traverse the map and convert to a black and white mask
		//for (int32 X = 0; X < XDimension; ++X)
		//{
		//	for (int32 Y = 0; Y < YDimension; ++Y)
		//	{
		//		//	Get the individual Pixel Color
		//		FColor PixelColor = FormattedImageData[Y * XDimension + X];

		//		//	Created this solely for debugging in the watch window
		//		FLinearColor RGBColor = FLinearColor(PixelColor);
		//		//	Alphas are unused, so making sure they don't affect the comparison.
		//		RGBColor.A = Color.A;

		//		//	If the color matches a pixel
		//		if (RGBColor == Color)
		//		{
		//			//	Mark the pixel as white, but scale it based on the alpha of the pixel
		//			FormattedMaskData[Y * XDimension + X] = FColor((FLinearColor::White).ToFColor(false));
		//		}
		//		else
		//		{
		//			//	Mark the pixel black for the mask
		//			FormattedMaskData[Y * XDimension + X] = FColor(FLinearColor::Black.ToFColor(false));
		//		}
		//	}
		//}

		//	Unlock the mask
		RawMaskData->Unlock();

		//	Store the Texture2D in the array to be returned
		ColoredMaskInformation.Add(std::move(ColorTextureMap));
	}


	RawCombinedData->Unlock();

	return ColoredMaskInformation;
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