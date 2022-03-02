// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "DVGetPrivateWalletProfile.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGetWalletResponse);

UCLASS()
class DEVERSE_API UDVGetPrivateWalletProfile : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UDVGetPrivateWalletProfile();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "InWorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
	static class UDVGetPrivateWalletProfile* GetWalletProfile(const UObject* InWorldContextObject);

	UPROPERTY(BlueprintAssignable)
	FOnGetWalletResponse OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FOnGetWalletResponse OnFail;

	void GetWallet(const UObject* InWorldContextObject);

	void HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

private:
	UPROPERTY()
		const UObject* WorldContextObject = nullptr;
};
