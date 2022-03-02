// Copyright Cosugames, LLC 2021


#include "BlockchainRequest/DVCreateLoginLink.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/DVFunctionLibrary.h"

UDVCreateLoginLink::UDVCreateLoginLink()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot();
	}
}

class UDVCreateLoginLink* UDVCreateLoginLink::CreateLoginLink(const UObject* InWorldContextObject)
{
	UDVCreateLoginLink* LoginTask = NewObject<UDVCreateLoginLink>();
	LoginTask->CreateLink(InWorldContextObject);
	return LoginTask;
}

void UDVCreateLoginLink::CreateLink(const UObject* InWorldContextObject)
{
#if !UE_SERVER
		WorldContextObject = InWorldContextObject;
		// Create the Http request and add to pending request list
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

		HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVCreateLoginLink::HandleReceiveResponse);
		HttpRequest->SetURL(FString::Printf(TEXT("%s/api/wallet/createLoginLink"), *UDVFunctionLibrary::GetMasterServerUrl()));
		//HttpRequest->SetURL("www.deverse.world:8081/api/wallet/createLoginLink");
		//HttpRequest->SetURL("http:/127.0.0.1:8080/api/wallet/createLoginLink");
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
		HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
		HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
		HttpRequest->ProcessRequest();
#else
		// On the server we don't execute fail or success we just don't fire the request.
		RemoveFromRoot();
#endif
}

void UDVCreateLoginLink::PollConnectStatus()
{
#if !UE_SERVER
	// Create the Http request and add to pending request list
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDVCreateLoginLink::HandleReceiveLoginStatus);
	HttpRequest->SetURL(FString::Printf(TEXT("%s/api/wallet/pollLoginLink/%s"), *UDVFunctionLibrary::GetMasterServerUrl(), *LoginToken));
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader("Content-Type", TEXT("application/json"));
	HttpRequest->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	HttpRequest->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	HttpRequest->ProcessRequest();
#else
	// On the server we don't execute fail or success we just don't fire the request.
	RemoveFromRoot();
#endif
}

void UDVCreateLoginLink::HandleReceiveResponse(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER
	RemoveFromRoot();
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0)
	{
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
		if (FJsonSerializer::Deserialize(Reader, JsonObject)) {
			LoginToken = JsonObject->GetStringField("login_url");
			/*FString Token;
			FString LeftS;
			LoginToken.Split("login/", &LeftS, &Token);
			LoginToken = Token;*/
			//OnSuccess.Broadcast(FString::Printf(TEXT("http://www.deverse.world/login/%s"), *LoginToken));
			OnSuccess.Broadcast(FString::Printf(TEXT("http://localhost:3000/login/%s"), *LoginToken));
			// Proceed to poll login status
			PollConnectStatus();
			return;
		}
	}
	OnFail.Broadcast("");
#endif
}

void UDVCreateLoginLink::HandleReceiveLoginStatus(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
#if !UE_SERVER
	if (bSucceeded && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 && HttpResponse->GetResponseCode() == 200) {
		UE_LOG(LogTemp, Warning, TEXT("Response %s"), *HttpResponse->GetContentAsString());
		FString Cookie = HttpResponse->GetHeader("Set-Cookie");
		// Split
		FString LeftS;
		FString RightS;
		Cookie.Split(";", &LeftS, &RightS);
		LeftS.Split("deverse-jwt=", &LeftS, &RightS);
		Cookie = RightS;
		//
		// Store it to a file
		StoreCookieToFile(Cookie);
		GEngine->GameViewport->GetWindow()->GetNativeWindow()->HACK_ForceToFront();
		FWalletConnectData ConnectData;
		TSharedPtr<FJsonObject> JsonObject;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
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
		OnLoginSuccess.Broadcast(ConnectData.Address);
		RemoveFromRoot();
		return;
	}
	else if (CurrentRetry < MaxTry){
		// retry
		CurrentRetry++;
		UE_LOG(LogTemp, Warning, TEXT("Retry: %d"), CurrentRetry);
		PollConnectStatus();
	}
	OnLoginFail.Broadcast("");
	RemoveFromRoot();
#else
	RemoveFromRoot();
#endif
}

void UDVCreateLoginLink::StoreCookieToFile(const FString& InCookie)
{
	FString SaveDir = FPaths::ProjectSavedDir();
	IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();
	if (File.CreateDirectory(*SaveDir)) {
		FFileHelper::SaveStringToFile(InCookie, *(SaveDir+"/cookie"), FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), EFileWrite::FILEWRITE_NoFail);
	}
}
