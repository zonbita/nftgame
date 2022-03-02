// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "DVNftQuery.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVNftQuery : public UObject
{
	GENERATED_BODY()

public:
	UDVNftQuery();

	void RequestNft(FString InURL, class UDVBaseGameInstance* InGI);
private:
	void HandleNftRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	UPROPERTY(Transient)
	class UDVBaseGameInstance* GI;
};
