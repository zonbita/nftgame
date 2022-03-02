// Copyright Cosugames, LLC 2021

#include "BlockchainRequest/DVNftQuery.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Deverse/Deverse.h"
#include "Deverse/DeverseCharacter.h"
#include "Core/DVBaseGameInstance.h"

UDVNftQuery::UDVNftQuery()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

void UDVNftQuery::RequestNft(FString InURL, UDVBaseGameInstance* InGI)
{
#if !UE_SERVER
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVNftQuery::HandleNftRequest);
	HttpRequest->SetURL(InURL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader("X-API-Key", TEXT("YltOuKAIdIYVy6OG227AKD5RKbuhLWIMONGMb7rws0aLNt8UHricFcrGOtpqt5nw"));
	HttpRequest->ProcessRequest();
	GI = InGI;
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDVNftQuery::HandleNftRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (bSucceeded && HttpResponse->GetContentLength() > 0) {
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			TArray<TSharedPtr<FJsonValue>> ResultArr = JsonObject->GetArrayField("result");
			for (auto Ele : ResultArr) {
				TSharedPtr<FJsonObject> Object = Ele->AsObject();
				TSharedPtr<FJsonObject> MetadataObj;
				Reader = TJsonReaderFactory<>::Create(Object->GetStringField("metadata"));
				if (FJsonSerializer::Deserialize(Reader, MetadataObj)) {
#if !UE_SERVER
					// Broadcast data for front-end use
					FNftMetadata NftMetadata;
					FNft Nft;
					NftMetadata.Description = MetadataObj->GetStringField("description");
					NftMetadata.ImageURL = MetadataObj->GetStringField("image");
					NftMetadata.Name = MetadataObj->GetStringField("name");
					Nft.Metadata = NftMetadata;
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
					UE_LOG(LogTemp, Warning, TEXT("Contract %s tokenid %s"), *Nft.TokenAddress, *Nft.TokenId); 
					if (GI)
						GI->UpdateNftHoldings(Nft);
#else
					// On the server we don't execute fail or success we just don't fire the request.
					RemoveFromRoot();
					return;
#endif 
				}
			}
			GI->FinishQueryNfts(true);
			GI = nullptr;
			RemoveFromRoot();
		}
	}
}
