// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Deverse/Deverse.h"
#include "Interfaces/IHttpRequest.h"
#include "DVWalletQuery.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVWalletQuery : public UObject
{
	GENERATED_BODY()

public:
	UDVWalletQuery();

	void RequestBalance(FString InWalletAddress, EChain InChain, class UDVBaseGameInstance* InGI);

	void RequestTokens();
private:
	void HandleReceiveBalance(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void HandleReceiveTokens(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString WalletAddress;

	EChain RequestedChain;

	UPROPERTY()
	class UDVBaseGameInstance* GI;
};
