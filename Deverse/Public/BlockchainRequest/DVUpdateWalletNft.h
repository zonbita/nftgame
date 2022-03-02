// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Deverse/Deverse.h"
#include "Interfaces/IHttpRequest.h"
#include "DVUpdateWalletNft.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateNftResponse);

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVUpdateWalletNft : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UDVUpdateWalletNft();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "InWorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
	static class UDVUpdateWalletNft* UpdateWalletNft(const UObject* InWorldContextObject, const TArray<FLiteNft> InNfts);

	UPROPERTY(BlueprintAssignable)
		FOnUpdateNftResponse OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FOnUpdateNftResponse OnFail;

	void HandleUpdateWallet(const UObject* InWorldContextObject, const TArray<FLiteNft> InNfts);

	void HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
private:
	UPROPERTY()
		const UObject* WorldContextObject = nullptr;
};
