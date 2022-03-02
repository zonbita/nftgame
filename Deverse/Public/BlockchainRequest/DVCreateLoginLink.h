// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "DVCreateLoginLink.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginLinkResponse, FString, LoginURL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoginStatus, FString, WalletAddr);

UCLASS()
class DEVERSE_API UDVCreateLoginLink : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UDVCreateLoginLink();

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "InWorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
	static class UDVCreateLoginLink* CreateLoginLink(const UObject* InWorldContextObject);
	
	UPROPERTY(BlueprintAssignable)
		FOnLoginLinkResponse OnSuccess;

	UPROPERTY(BlueprintAssignable)
		FOnLoginLinkResponse OnFail;

	UPROPERTY(BlueprintAssignable)
		FOnLoginStatus OnLoginSuccess;

	UPROPERTY(BlueprintAssignable)
		FOnLoginStatus OnLoginFail;

	void CreateLink(const UObject* InWorldContextObject);

	void PollConnectStatus();

	void HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void HandleReceiveLoginStatus(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
private:
	int32 MaxTry = 9999;

	int32 CurrentRetry;

	FString LoginToken;

	UPROPERTY()
		const UObject* WorldContextObject = nullptr;
	
	void StoreCookieToFile(const FString& InCookie);
};
