// Copyright June Rhodes. MIT Licensed.

#include "ExampleCPPConfigLibrary.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

bool UExampleCPPConfigLibrary::GetPreferBlueprintsImplementation()
{
    return GetDefault<UExampleCPPConfig>()->bPreferBlueprintsImplementation;
}

bool UExampleCPPConfigLibrary::GetIsSteamEnabled(const UObject *WorldContextObject)
{
    bool Value = true;
    GConfig->GetBool(TEXT("OnlineSubsystemSteam"), TEXT("bEnabled"), Value, GEngineIni);
    return Value;
}

void UExampleCPPConfigLibrary::SetIsSteamEnabled(const UObject *WorldContextObject, bool EnableSteam)
{
    GConfig->SetBool(TEXT("OnlineSubsystemSteam"), TEXT("bEnabled"), EnableSteam, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

FString UExampleCPPConfigLibrary::GetSteamAppId(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), Value, GEngineIni);
    return Value;
}

void UExampleCPPConfigLibrary::SetSteamAppId(const UObject *WorldContextObject, FString InAppId)
{
    GConfig->SetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), *InAppId, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

TArray<FString> UExampleCPPConfigLibrary::GetDelegatedSubsystems(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("DelegatedSubsystems"), Value, GEngineIni);

    TArray<FString> Results;
    Value.ParseIntoArray(Results, TEXT(","), true);

    return Results;
}

void UExampleCPPConfigLibrary::SetDelegatedSubsystems(
    const UObject *WorldContextObject,
    TArray<FString> InSubsystemNames)
{
    FString NewValue = FString::Join(InSubsystemNames, TEXT(","));
    GConfig->SetString(TEXT("EpicOnlineServices"), TEXT("DelegatedSubsystems"), *NewValue, GEngineIni);
    GConfig->Flush(false, GEngineIni);
}

bool UExampleCPPConfigLibrary::GetAreBeaconsSupported(const UObject *WorldContextObject)
{
    FString Value = TEXT("");
    FString ApiVersion = TEXT("");
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("NetworkingStack"), Value, GEngineIni);
    GConfig->GetString(TEXT("EpicOnlineServices"), TEXT("ApiVersion"), ApiVersion, GEngineIni);
    if (Value == TEXT("Full"))
    {
        return true;
    }
    else if (Value == TEXT("Legacy"))
    {
        return false;
    }
    else
    {
        return !(
            ApiVersion == TEXT("v2020_12_09") || ApiVersion == TEXT("v2020_12_02") ||
            ApiVersion == TEXT("v2020_12_01"));
    }
}