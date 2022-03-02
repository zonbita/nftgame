// Copyright Cosugames, LLC 2021

#include "BlockchainRequest/DVGetPrivateWalletProfile.h"
#include "Engine/DVFunctionLibrary.h"

UDVGetPrivateWalletProfile::UDVGetPrivateWalletProfile()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

class UDVGetPrivateWalletProfile* UDVGetPrivateWalletProfile::GetWalletProfile(const UObject* InWorldContextObject)
{
	UDVGetPrivateWalletProfile* GetTask = NewObject<UDVGetPrivateWalletProfile>();
	GetTask->GetWallet(InWorldContextObject);
	return GetTask;
}

void UDVGetPrivateWalletProfile::GetWallet(const UObject* InWorldContextObject)
{
#if !UE_SERVER
	WorldContextObject = InWorldContextObject;
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVGetPrivateWalletProfile::HandleReceiveResponse);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/wallet/profile"), *UDVFunctionLibrary::GetMasterServerUrl()));
	//HttpRequest->SetURL("www.deverse.world:8081/api/wallet/profile");
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

void UDVGetPrivateWalletProfile::HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER
	RemoveFromRoot();
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		FWalletConnectData ConnectData;
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (HttpResponse->GetResponseCode() != 400) {
			if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
				ConnectData.Address = JsonObject->GetStringField("address");
				ConnectData.Id = JsonObject->GetIntegerField("id");
				ConnectData.Nonce = JsonObject->GetStringField("nonce");
				ConnectData.CreatedAt = JsonObject->GetStringField("created_at");
				ConnectData.UpdatedAt = JsonObject->GetStringField("updated_at");
				if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
					IDVWalletInterface::Execute_OnWalletConnect(World->GetGameInstance(), ConnectData);
				}
			}
			OnSuccess.Broadcast();
			return;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s"), *HttpResponse->GetContentAsString());
		}
	}
	OnFail.Broadcast();
#endif
}
