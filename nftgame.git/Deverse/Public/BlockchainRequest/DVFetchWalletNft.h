// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Deverse/Deverse.h"
#include "DVFetchWalletNft.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFetchWalletNft);

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVFetchWalletNft : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UDVFetchWalletNft();

	UPROPERTY(BlueprintAssignable)
		FOnFetchWalletNft OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FOnFetchWalletNft OnFail;

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "InWorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class UDVFetchWalletNft* GetWalletNft(const UObject* InWorldContextObject);

	void HandleGetWalletNft(const UObject* InWorldContextObject);

	void HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
private:
	UPROPERTY()
		const UObject* WorldContextObject = nullptr;
};
