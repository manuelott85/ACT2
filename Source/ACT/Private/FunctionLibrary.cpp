// Fill out your copyright notice in the Description page of Project Settings.

#include "FunctionLibrary.h"
#include <UnrealClient.h>
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget.h"

TArray<FColor> UFunctionLibrary::getAverageValueFromAllColors(UTextureRenderTarget* TextureRenderTarget)
{
	// Read the pixels from the RenderTarget and store them in a FColor array
	TArray<FColor> SurfData;
	FRenderTarget *RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
	RenderTarget->ReadPixels(SurfData);

	// Index formula
	//int x = 0, y = 0;
	//FColor PixelColor = SurfData[x + y * 16/*TextureRenderTarget->SizeX*/];


	return SurfData;


	//---------------------------

//	// Creates Texture2D to store TextureRenderTarget content
//	UTexture2D * Texture = UTexture2D::CreateTransient(TextureRenderTarget->SizeX, TextureRenderTarget->SizeY, PF_B8G8R8A8);
//#if WITH_EDITORONLY_DATA
//	Texture->MipGenSettings = TMGS_NoMipmaps;
//#endif
//	Texture->SRGB = TextureRenderTarget->SRGB;
//
//	// Read the pixels from the RenderTarget and store them in a FColor array
//	TArray<FColor> SurfData;
//	FRenderTarget *RenderTarget = TextureRenderTarget->GameThread_GetRenderTargetResource();
//	RenderTarget->ReadPixels(SurfData);
//
//	// Lock and copies the data between the textures
//	void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
//	const int32 TextureDataSize = SurfData.Num() * 4;
//	FMemory::Memcpy(TextureData, SurfData.GetData(), TextureDataSize);
//	Texture->PlatformData->Mips[0].BulkData.Unlock();
//	// Apply Texture changes to GPU memory
//	Texture->UpdateResource();

}