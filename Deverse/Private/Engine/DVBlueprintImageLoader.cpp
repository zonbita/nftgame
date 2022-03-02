// Copyright Cosugames, LLC 2021


#include "Engine/DVBlueprintImageLoader.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

void UDVBlueprintImageLoader::LoadImage(FImageData ImageData)
{
	UDVBlueprintImageLoader* LoadImageTask = NewObject<UDVBlueprintImageLoader>();
	LoadImageTask->StartLoad(ImageData);
}

#if !UE_SERVER
static void WriteRawToTexture_RenderThread2(FTexture2DDynamicResource* TextureResource, TArray64<uint8>* RawData, bool bUseSRGB = true)
{
	check(IsInRenderingThread());

	if (TextureResource)
	{
		FRHITexture2D* TextureRHI = TextureResource->GetTexture2DRHI();

		int32 Width = TextureRHI->GetSizeX();
		int32 Height = TextureRHI->GetSizeY();

		uint32 DestStride = 0;
		uint8* DestData = reinterpret_cast<uint8*>(RHILockTexture2D(TextureRHI, 0, RLM_WriteOnly, DestStride, false, false));

		for (int32 y = 0; y < Height; y++)
		{
			uint8* DestPtr = &DestData[((int64)Height - 1 - y) * DestStride];

			const FColor* SrcPtr = &((FColor*)(RawData->GetData()))[((int64)Height - 1 - y) * Width];
			for (int32 x = 0; x < Width; x++)
			{
				*DestPtr++ = SrcPtr->B;
				*DestPtr++ = SrcPtr->G;
				*DestPtr++ = SrcPtr->R;
				*DestPtr++ = SrcPtr->A;
				SrcPtr++;
			}
		}

		RHIUnlockTexture2D(TextureRHI, 0, false, false);
	}

	delete RawData;
}
#endif

void UDVBlueprintImageLoader::StartLoad(FImageData ImageData)
{
#if !UE_SERVER
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageData.ImageFormat);
	TArray64<uint8>* RawData = ImageData.PixelData;
	const ERGBFormat InFormat = ERGBFormat::BGRA;
	if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageData.SizeX, ImageData.SizeY))
	{
		Texture->SRGB = true;
		Texture->UpdateResource();

		FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
		if (TextureResource)
		{
			ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
				[TextureResource, RawData](FRHICommandListImmediate& RHICmdList)
				{
					WriteRawToTexture_RenderThread2(TextureResource, RawData);
				});
		}
		else
		{
			delete RawData;
		}
		return;
	}

#endif
}
