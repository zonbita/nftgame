// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"

#include "../ExampleCPPConfigLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetConnection.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UExampleCPPSubsystem::QueryStats(const UObject *WorldContextObject, FExampleCPPSubsystemQueryStatsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPStat>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineStatsPtr Stats = Subsystem->GetStatsInterface();

    TArray<TSharedRef<const FUniqueNetId>> Users;
    Users.Add(Identity->GetUniquePlayerId(0).ToSharedRef());

    TArray<FString> StatNames;
    StatNames.Add(TEXT("TestLatest"));
    StatNames.Add(TEXT("TestScore"));

    Stats->QueryStats(
        Identity->GetUniquePlayerId(0).ToSharedRef(),
        Users,
        StatNames,
        FOnlineStatsQueryUsersStatsComplete::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryStats,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleQueryStats(
    const FOnlineError &ResultState,
    const TArray<TSharedRef<const FOnlineStatsUserStats>> &UsersStatsResult,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryStatsComplete OnDone)
{
    auto QueriedStats = UsersStatsResult[0];

    if (!ResultState.bSucceeded)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPStat>());
        return;
    }

    TArray<FExampleCPPStat> Results;
    for (auto KV : QueriedStats->Stats)
    {
        FExampleCPPStat Stat;
        Stat.Id = KV.Key;
        KV.Value.GetValue(Stat.CurrentValue);
        Results.Add(Stat);
    }

    OnDone.ExecuteIfBound(true, Results);
}

void UExampleCPPSubsystem::IngestStat(
    const UObject *WorldContextObject,
    FString StatName,
    int32 IngestAmount,
    FExampleCPPSubsystemIngestStatComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineStatsPtr StatsInterface = Subsystem->GetStatsInterface();

    FOnlineStatsUserUpdatedStats Stat = FOnlineStatsUserUpdatedStats(Identity->GetUniquePlayerId(0).ToSharedRef());
    Stat.Stats.Add(StatName, FOnlineStatUpdate(IngestAmount, FOnlineStatUpdate::EOnlineStatModificationType::Unknown));

    TArray<FOnlineStatsUserUpdatedStats> Stats;
    Stats.Add(Stat);

    StatsInterface->UpdateStats(
        Identity->GetUniquePlayerId(0).ToSharedRef(),
        Stats,
        FOnlineStatsUpdateStatsComplete::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleIngestStat,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleIngestStat(
    const FOnlineError &ResultState,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemIngestStatComplete OnDone)
{
    OnDone.ExecuteIfBound(ResultState.bSucceeded);
}