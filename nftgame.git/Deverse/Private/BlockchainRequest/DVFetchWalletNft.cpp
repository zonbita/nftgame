// Copyright Cosugames, LLC 2021


#include "BlockchainRequest/DVFetchWalletNft.h"
#include "Engine/DVFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "UObject/WeakObjectPtr.h"

UDVFetchWalletNft::UDVFetchWalletNft()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

class UDVFetchWalletNft* UDVFetchWalletNft::GetWalletNft(const UObject* InWorldContextObject)
{
	UDVFetchWalletNft* GetTask = NewObject<UDVFetchWalletNft>();
	GetTask->HandleGetWalletNft(InWorldContextObject);
	return GetTask;
}

void UDVFetchWalletNft::HandleGetWalletNft(const UObject* InWorldContextObject)
{
#if !UE_SERVER
	if (RegisteredWithGameInstance.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("yes gi %s"), *RegisteredWithGameInstance.Get()->GetName());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("gi not valid")); 
	}
		
	WorldContextObject = InWorldContextObject;
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVFetchWalletNft::HandleReceiveResponse);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/wallet/fetchAssets/%s"), *UDVFunctionLibrary::GetMasterServerUrl(), *UDVFunctionLibrary::GetWalletAddress(WorldContextObject)));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("Cookie"), FString::Printf(TEXT("deverse-jwt=%s"), *UDVFunctionLibrary::GetCookie()));
	HttpRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDVFetchWalletNft::HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER
	RemoveFromRoot();
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		FWalletConnectData ConnectData;
		TSharedPtr<FJsonObject> JsonObject;
		TArray<TSharedPtr<FJsonValue>> ResultArr;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (HttpResponse->GetResponseCode() != 400) {
			if (FJsonSerializer::Deserialize(Reader, ResultArr)) {
				if (ResultArr.Num() == 0) {
					OnFail.Broadcast();
					return;
				}
				for (int32 i = 0; i < ResultArr.Num(); i++) {
					TSharedPtr<FJsonObject> Object = ResultArr[i]->AsObject();
					//TSharedPtr<FJsonObject> MetadataObj;
					//Reader = TJsonReaderFactory<>::Create(Object->GetStringField("metadata"));
					//if (FJsonSerializer::Deserialize(Reader, MetadataObj)) {
						//FNftMetadata NftMetadata;
						FNft Nft;
						//NftMetadata.Description = MetadataObj->GetStringField("description");
						//NftMetadata.ImageURL = MetadataObj->GetStringField("image");
						//NftMetadata.Name = MetadataObj->GetStringField("name");
						//Nft.Metadata = NftMetadata;
						Nft.TokenAddress = Object->GetStringField("token_address");
						Nft.Amount = FCString::Atoi(*Object->GetStringField("amount"));
						Nft.BlockNumber = FCString::Atoi64(*Object->GetStringField("block_number"));
						Nft.MintedBlockNumber = FCString::Atoi64(*Object->GetStringField("block_number_minted"));
						Nft.TokenId = Object->GetStringField("token_id");
						Nft.ContractType = Object->GetStringField("contract_type");
						Nft.Name = Object->GetStringField("name");
						Nft.OwnerAddress = Object->GetStringField("owner_of");
						Nft.TokenUri = Object->GetStringField("token_uri");
						Nft.Symbol = Object->GetStringField("symbol");
						Nft.Id = (int64)Object->GetNumberField("id");
						if ((FDateTime::Now().ToUnixTimestamp() - Nft.CreatedAt.ToUnixTimestamp()) >= 86400) { // More than a day, we do reupdate from Moralis							
							UE_LOG(LogTemp, Warning, TEXT("over a day")); 
							OnFail.Broadcast();
							return;
						}
						if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
							World->GetGameInstance<UDVBaseGameInstance>()->UpdateNftHoldings(Nft);
						}
					//}
				}
				if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
					World->GetGameInstance<UDVBaseGameInstance>()->FinishQueryNfts(false);
				}
			}
			OnSuccess.Broadcast();
			RemoveFromRoot();
			return;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s"), *HttpResponse->GetContentAsString());
		}
	}
	OnFail.Broadcast();
#else
	RemoveFromRoot();
#endif
}
