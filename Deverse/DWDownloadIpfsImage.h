// Copyright Cosugames

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Core/DVWalletInterface.h"
#include "HttpModule.h"
#include "DWDownloadIpfsImage.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIpfsImageDelegate, UTexture2DDynamic*, Texture);


UCLASS()
class UDWDownloadIpfsImage : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UDWDownloadIpfsImage();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "InWorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class UDWDownloadIpfsImage* DownloadIpfsImage(const UObject* InWorldContextObject, FString URL, FNft InNft);

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static class UDWDownloadIpfsImage* QueryAndDownloadIpfsImage(FString URL);

	UPROPERTY(BlueprintAssignable)
		FIpfsImageDelegate OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FIpfsImageDelegate OnFail;

	void StartQuery(FString URL);

	void StartDownload(const UObject* InWorldContextObject, FString URL, FNft InNft);

	UPROPERTY()
	FNft NftData;
	
	UPROPERTY()
	const UObject* WorldContextObject = nullptr;

	virtual void BeginDestroy() override;
private:
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpImageRequest = FHttpModule::Get().CreateRequest();

	/** Handles image requests coming from the web */
	void HandleImageRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void HandleMetadataRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
};
