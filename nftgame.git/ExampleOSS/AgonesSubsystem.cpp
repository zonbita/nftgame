// Copyright June Rhodes. MIT Licensed.

#include "AgonesSubsystem.h"

#include "Containers/Ticker.h"
#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "HAL/PlatformMisc.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAgones, All, All);
DEFINE_LOG_CATEGORY(LogAgones);

void UAgonesSubsystem::Initialize(FSubsystemCollectionBase &Collection)
{
    this->AgonesHttpPort = FPlatformMisc::GetEnvironmentVariable(TEXT("AGONES_SDK_HTTP_PORT"));
    if (this->AgonesHttpPort.IsEmpty())
    {
        UE_LOG(LogAgones, Verbose, TEXT("Agones port not detected, not running health checks."));
        return;
    }

    this->bHasSentReady = false;

    this->HealthCheckTimer = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::SendHealthCheck),
        1.0f);
}

void UAgonesSubsystem::Deinitialize()
{
    if (this->HealthCheckTimer.IsValid())
    {
        FTicker::GetCoreTicker().RemoveTicker(this->HealthCheckTimer);
    }
}

void UAgonesSubsystem::StartGetPortsFromAgones(FAgonesPortsCallback Callback)
{
    if (this->AgonesHttpPort.IsEmpty())
    {
        Callback.ExecuteIfBound(false, 0, 0);
        return;
    }

    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Format(TEXT("http://localhost:{0}/gameserver"), {*this->AgonesHttpPort}));
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("");

    Request->OnProcessRequestComplete().BindUObject(this, &UAgonesSubsystem::HandleGameServer, Callback);
    Request->ProcessRequest();
}

bool UAgonesSubsystem::RetryGetGamePorts(float DeltaSeconds, FAgonesPortsCallback Callback)
{
    this->StartGetPortsFromAgones(Callback);
    return false;
}

bool UAgonesSubsystem::SendHealthCheck(float DeltaSeconds)
{
    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Format(TEXT("http://localhost:{0}/health"), {*this->AgonesHttpPort}));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("");

    Request->OnProcessRequestComplete().BindUObject(this, &UAgonesSubsystem::HandleHealthCheck);
    Request->ProcessRequest();

    this->HealthCheckTimer = FTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::SendHealthCheck),
        1.0f);

    return false;
}

void UAgonesSubsystem::SendReady()
{
    FHttpModule *Http = &FHttpModule::Get();
    FHttpRequestRef Request = Http->CreateRequest();
    Request->SetURL(FString::Format(TEXT("http://localhost:{0}/ready"), {*this->AgonesHttpPort}));
    Request->SetVerb("POST");
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
    Request->SetContentAsString("");

    Request->OnProcessRequestComplete().BindUObject(this, &UAgonesSubsystem::HandleReady);
    Request->ProcessRequest();
}

void UAgonesSubsystem::HandleHealthCheck(
    FHttpRequestPtr HttpRequest,
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded)
{
    if (!bSucceeded)
    {
        UE_LOG(LogAgones, Verbose, TEXT("Agones health check failed (not successful)"));
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(LogAgones, Verbose, TEXT("Agones health check failed (error code %d)"), HttpResponse->GetResponseCode());
        return;
    }

    if (!this->bHasSentReady)
    {
        this->SendReady();
        this->bHasSentReady = true;
    }
}

void UAgonesSubsystem::HandleReady(
    FHttpRequestPtr HttpRequest,
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded)
{
    if (!bSucceeded)
    {
        this->bHasSentReady = false;
        UE_LOG(LogAgones, Verbose, TEXT("Agones ready failed (not successful)"));
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        this->bHasSentReady = false;
        UE_LOG(LogAgones, Verbose, TEXT("Agones ready failed (error code %d)"), HttpResponse->GetResponseCode());
        return;
    }

    UE_LOG(LogAgones, Verbose, TEXT("Agones marked as ready"));
}

void UAgonesSubsystem::HandleGameServer(
    FHttpRequestPtr HttpRequest,
    const FHttpResponsePtr HttpResponse,
    const bool bSucceeded,
    FAgonesPortsCallback Callback)
{
    if (!bSucceeded)
    {
        UE_LOG(LogAgones, Verbose, TEXT("Unable to get game server ports from Agones, retrying"));
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::RetryGetGamePorts, Callback),
            5.0f);
        return;
    }

    if (!EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
    {
        UE_LOG(
            LogAgones,
            Verbose,
            TEXT("Unable to get game server ports from Agones, error code %d, retrying"),
            HttpResponse->GetResponseCode());
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::RetryGetGamePorts, Callback),
            5.0f);
        return;
    }

    const FString Json = HttpResponse->GetContentAsString();
    TSharedPtr<FJsonObject> JsonObject;
    const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Json);
    if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(
            LogAgones,
            Verbose,
            TEXT("Unable to get game server ports from Agones, invalid JSON, retrying"),
            HttpResponse->GetResponseCode());
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::RetryGetGamePorts, Callback),
            5.0f);
        return;
    }

    auto StatusObj = JsonObject->GetObjectField("status");
    if (!StatusObj.IsValid() || StatusObj->GetStringField("state") != TEXT("Ready") || !StatusObj->HasField("ports"))
    {
        UE_LOG(LogAgones, Verbose, TEXT("Game server is not ready yet, retrying"), HttpResponse->GetResponseCode());
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UAgonesSubsystem::RetryGetGamePorts, Callback),
            5.0f);
        return;
    }

    UE_LOG(LogAgones, Verbose, TEXT("Received game server JSON: %s"), *Json);

    int32 OutGamePort = 0;
    int32 OutBeaconPort = 0;
    const TArray<TSharedPtr<FJsonValue>> &PortsArray = JsonObject->GetObjectField("status")->GetArrayField("ports");
    for (auto PortValue : PortsArray)
    {
        auto PortObject = PortValue->AsObject();

        if (PortObject->GetStringField("name") == FString(TEXT("game")))
        {
            OutGamePort = PortObject->GetIntegerField("port");
            UE_LOG(LogAgones, Verbose, TEXT("Detected game port as: %d"), OutGamePort);
        }
        else if (PortObject->GetStringField("name") == FString(TEXT("beacon")))
        {
            OutBeaconPort = PortObject->GetIntegerField("port");
            UE_LOG(LogAgones, Verbose, TEXT("Detected beacon port as: %d"), OutBeaconPort);
        }
    }

    Callback.ExecuteIfBound(OutGamePort != 0 && OutBeaconPort != 0, OutGamePort, OutBeaconPort);
}