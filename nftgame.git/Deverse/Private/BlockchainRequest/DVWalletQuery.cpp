// Copyright Cosugames, LLC 2021


#include "BlockchainRequest/DVWalletQuery.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Core/DVBaseGameInstance.h"
#include "Engine/DVFunctionLibrary.h"

UDVWalletQuery::UDVWalletQuery()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

void UDVWalletQuery::RequestBalance(FString InWalletAddress, EChain InChain, UDVBaseGameInstance* InGI)
{
	FString Chain;
	switch (InChain) {
	case EChain::EEth:
		Chain = "eth";
		break;
	case EChain::EBsc:
		Chain = "bsc";
		break;
	case EChain::EPolygon:
		Chain = "polygon";
		break;
	case EChain::EAvax:
		Chain = "avalanche";
		break;
	}
	RequestedChain = InChain;
	WalletAddress = InWalletAddress;
#if !UE_SERVER
	GI = InGI;
	FString Url = FString::Printf(TEXT("https://deep-index.moralis.io/api/v2/%s/balance?chain=%s"), *WalletAddress, *Chain);
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVWalletQuery::HandleReceiveBalance);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader("X-API-Key", TEXT("YltOuKAIdIYVy6OG227AKD5RKbuhLWIMONGMb7rws0aLNt8UHricFcrGOtpqt5nw"));
	HttpRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDVWalletQuery::RequestTokens()
{
#if !UE_SERVER
	FString Chain;
	switch (RequestedChain) {
	case EChain::EEth:
		Chain = "eth";
		break;
	case EChain::EBsc:
		Chain = "bsc";
		break;
	case EChain::EPolygon:
		Chain = "polygon";
		break;
	case EChain::EAvax:
		Chain = "avalanche";
		break;
	}
	FString Url = FString::Printf(TEXT("https://deep-index.moralis.io/api/v2/%s/erc20?chain=%s"), *WalletAddress, *Chain);
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVWalletQuery::HandleReceiveTokens);
	HttpRequest->SetURL(Url);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader("X-API-Key", TEXT("YltOuKAIdIYVy6OG227AKD5RKbuhLWIMONGMb7rws0aLNt8UHricFcrGOtpqt5nw"));
	HttpRequest->ProcessRequest();
#else
	RemoveFromRoot();
#endif
}

void UDVWalletQuery::HandleReceiveBalance(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse->GetContentLength() > 0) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		UE_LOG(LogTemp, Warning, TEXT("Response %s"), *HttpResponse->GetContentAsString()); 
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			FString BalanceStr = JsonObject->GetStringField("balance");		
			float Balance = FCString::Atof(*BalanceStr) / 1000000000000000000.f;
			if (GI) {		
				GI->UpdateNativeWalletBalance(Balance);
				UE_LOG(LogTemp, Warning, TEXT("Balance %f"), Balance);
			}
		}
		RequestTokens();
	}
}

void UDVWalletQuery::HandleReceiveTokens(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse->GetContentLength() > 0) {
		TArray<TSharedPtr<FJsonValue>> JsonObjects;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		UE_LOG(LogTemp, Warning, TEXT("Response %s"), *HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObjects)) {
			GI->ClearBalance();
			for (int32 i = 0; i < JsonObjects.Num(); i++) {
				TSharedPtr<FJsonObject> TokenObj = JsonObjects[i]->AsObject();
				FToken Token;
				Token.TokenAddress = TokenObj->GetStringField("token_address");
				Token.Name = TokenObj->GetStringField("name");
				Token.Symbol = TokenObj->GetStringField("symbol");
				Token.Logo = TokenObj->GetStringField("logo");
				Token.Thumbnail = TokenObj->GetStringField("thumbnail");
				Token.Decimals = FCString::Atoi(*TokenObj->GetStringField("decimals"));
				Token.Balance = UDVFunctionLibrary::FromWei(FCString::Atof(*TokenObj->GetStringField("balance")), Token.Decimals);
				if (GI) {
					GI->UpdateTokenBalance(Token);
				}
			}
			if (GI)
				GI->FinishUpdateBalance();
			GI = nullptr;
		}
	}
}
