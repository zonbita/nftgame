// Copyright Cosugames, LLC 2021

#include "DWDownloadIpfsImage.h"
#include "Engine/Texture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "IImageWrapper.h"

#include "Interfaces/IHttpResponse.h"
#include "IImageWrapperModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Deverse/Deverse.h"
#include "Core/DVBaseGameInstance.h"
//#include "AnimatedTexture/Private/GifImageWrapper.h"

UDWDownloadIpfsImage::UDWDownloadIpfsImage()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

#if !UE_SERVER
static void WriteRawToTexture_RenderThread(FTexture2DDynamicResource* TextureResource, TArray64<uint8>* RawData, bool bUseSRGB = true)
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

UDWDownloadIpfsImage* UDWDownloadIpfsImage::DownloadIpfsImage(const UObject* InWorldContextObject, FString URL, FNft InNft)
{
	UDWDownloadIpfsImage* DownloadTask = NewObject<UDWDownloadIpfsImage>();
	DownloadTask->StartDownload(InWorldContextObject, URL, InNft);
	return DownloadTask;
}

class UDWDownloadIpfsImage* UDWDownloadIpfsImage::QueryAndDownloadIpfsImage(FString URL)
{
	UDWDownloadIpfsImage* DownloadTask = NewObject<UDWDownloadIpfsImage>();
	DownloadTask->StartQuery(URL);

	return DownloadTask;
}

void UDWDownloadIpfsImage::StartQuery(FString URL)
{
#if !UE_SERVER
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDWDownloadIpfsImage::HandleMetadataRequest);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader("X-API-Key", TEXT("YltOuKAIdIYVy6OG227AKD5RKbuhLWIMONGMb7rws0aLNt8UHricFcrGOtpqt5nw"));
	HttpRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDWDownloadIpfsImage::StartDownload(const UObject* InWorldContextObject, FString URL, FNft InNft)
{
#if !UE_SERVER
	NftData = InNft;
	WorldContextObject = InWorldContextObject;
	// Create the Http request and add to pending request list

	FString FinalURL = URL;

	if (URL.Contains("ipfs://")) { // Dump way of detecting ipfs protocal instead of https
		// Redirect through gateway instead
		FString CID = URL.RightChop(7);
		FinalURL = "dweb.link/ipfs/" + CID;
	}
	UE_LOG(LogTemp, Warning, TEXT("Curl URL %s"), *FinalURL); 
	HttpImageRequest->OnProcessRequestComplete().BindUObject(this, &UDWDownloadIpfsImage::HandleImageRequest);
	HttpImageRequest->SetURL(FinalURL);
	HttpImageRequest->SetVerb(TEXT("GET"));
	HttpImageRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif 
}

void UDWDownloadIpfsImage::BeginDestroy()
{
	HttpImageRequest->CancelRequest();
	Super::BeginDestroy();
}

void UDWDownloadIpfsImage::HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER

	RemoveFromRoot();

	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		//FImageWrapperBase* GifImageWrapper = NULL;
		//GifImageWrapper = new FGifImageWrapper();
		TSharedPtr<IImageWrapper> ImageWrappers[4] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
			ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP)//,
			//MakeShareable(GifImageWrapper)
		};
		EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength());
		TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
		//for (auto ImageWrapper : ImageWrappers)
		//{
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()))
			{
				TArray64<uint8>* RawData = new TArray64<uint8>();
				const ERGBFormat InFormat = ERGBFormat::BGRA;
				if (ImageWrapper->GetRaw(InFormat, 8, *RawData))
				{
					if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
					{
						Texture->SRGB = true;
						Texture->UpdateResource();

						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->Resource);
						if (TextureResource)
						{
							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
								[TextureResource, RawData](FRHICommandListImmediate& RHICmdList)
								{
									WriteRawToTexture_RenderThread(TextureResource, RawData);
								});
						}
						else
						{
							delete RawData;
						}
						FImageData ImageData;
						ImageData.PixelData = RawData;
						ImageData.SizeX = ImageWrapper->GetWidth();
						ImageData.SizeY = ImageWrapper->GetHeight();
						ImageData.ImageFormat = ImageFormat;
						if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
							UDVBaseGameInstance* GI = Cast<UDVBaseGameInstance>(World->GetGameInstance());
							if (GI && !NftData.TokenAddress.IsEmpty()) {
								GI->UpdateNftDisplay(NftData.TokenAddress, NftData.TokenId, Texture);
							}
								
						}
						OnSuccess.Broadcast(Texture);
						return;
					}
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Image fail")); 
			}
		//}
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed")); 
	OnFail.Broadcast(nullptr);

#endif
}

void UDWDownloadIpfsImage::HandleMetadataRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse->GetContentLength() > 0) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			TArray<TSharedPtr<FJsonValue>> ResultArr = JsonObject->GetArrayField("result");
			for (auto Ele : ResultArr) {
				TSharedPtr<FJsonObject> Object = Ele->AsObject();
				TSharedPtr<FJsonObject> MetadataObj;
				Reader = TJsonReaderFactory<>::Create(Object->GetStringField("metadata"));
				if (FJsonSerializer::Deserialize(Reader, MetadataObj)) {
					
#if !UE_SERVER
					//StartDownload(MetadataObj->GetStringField("image"));
#else
					// On the server we don't execute fail or success we just don't fire the request.
					RemoveFromRoot();
					return;
#endif 
				}
			}
			RemoveFromRoot();
		}
	}
}
