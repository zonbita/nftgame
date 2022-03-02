// Copyright Cosugames, LLC 2021


#include "BlockchainRequest/DVUpdateWalletNft.h"
#include "Engine/DVFunctionLibrary.h"

UDVUpdateWalletNft::UDVUpdateWalletNft()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

class UDVUpdateWalletNft* UDVUpdateWalletNft::UpdateWalletNft(const UObject* InWorldContextObject, const TArray<FLiteNft> InNfts)
{
	UDVUpdateWalletNft* UpdateTask = NewObject<UDVUpdateWalletNft>();
	UpdateTask->HandleUpdateWallet(InWorldContextObject, InNfts);
	return UpdateTask;
}

void UDVUpdateWalletNft::HandleUpdateWallet(const UObject* InWorldContextObject, const TArray<FLiteNft> InNfts)
{
#if !UE_SERVER
	WorldContextObject = InWorldContextObject;
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVUpdateWalletNft::HandleReceiveResponse);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/wallet/updateAssets"), *UDVFunctionLibrary::GetMasterServerUrl()));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));

	TMap<FString, FString> NftMap;
	for (FLiteNft Nft : InNfts) {
		NftMap.Add(Nft.Metadata.ImageURL, Nft.TokenAddress);
	}
	HttpRequest->SetContentAsString(UDVFunctionLibrary::ConstructJsonArrayStr(NftMap));
	// Load cookie from file
	FString CookieStr;
	FString CookieFile = FPaths::ProjectSavedDir() + "/cookie";
	IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*CookieFile)) {
		bool bLoaded = FFileHelper::LoadFileToString(CookieStr, *CookieFile);
		if (!bLoaded) {
			UE_LOG(LogTemp, Error, TEXT("Cookie not valid. Please try to sign first"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Cookie loaded: %s"), *CookieStr); 
		}
	}

	//
	HttpRequest->SetHeader("Cookie", FString::Printf(TEXT("deverse-jwt=%s"), *CookieStr));
	HttpRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDVUpdateWalletNft::HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER
	RemoveFromRoot();
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Response succesful")); 
		OnSuccess.Broadcast();
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Fail message %d"), HttpResponse->GetResponseCode()); 
	OnFail.Broadcast();
#endif
}
