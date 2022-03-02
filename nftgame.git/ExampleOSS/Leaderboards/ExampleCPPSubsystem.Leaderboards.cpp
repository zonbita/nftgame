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

void UExampleCPPSubsystem::QueryFriendsLeaderboards(
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryLeaderboardsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();

    FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
    ReadRef->ColumnMetadata.Add(FColumnMetaData(FName(TEXT("TestScore")), EOnlineKeyValuePairDataType::Int32));

    // note: This example code only supports running one leaderboard process at once; if you want to support
    // multiple in flight you probably need to make a latent blueprint node.
    if (this->QueryLeaderboardsDelegateHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Already a leaderboard operation running."));
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
        return;
    }

    this->QueryLeaderboardsDelegateHandle = LeaderboardsInterface->AddOnLeaderboardReadCompleteDelegate_Handle(
        FOnLeaderboardReadComplete::FDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleLeaderboardResult,
            ReadRef,
            WorldContextObject,
            OnDone));
    if (!LeaderboardsInterface->ReadLeaderboardsForFriends(0, ReadRef))
    {
        LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
        this->QueryLeaderboardsDelegateHandle.Reset();
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
    }
}

void UExampleCPPSubsystem::QueryGlobalLeaderboards(
    const UObject *WorldContextObject,
    const FString &LeaderboardId,
    FExampleCPPSubsystemQueryLeaderboardsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();

    FOnlineLeaderboardReadRef ReadRef = MakeShared<FOnlineLeaderboardRead, ESPMode::ThreadSafe>();
    ReadRef->LeaderboardName = FName(LeaderboardId);

    // note: This example code only supports running one leaderboard process at once; if you want to support
    // multiple in flight you probably need to make a latent blueprint node.
    if (this->QueryLeaderboardsDelegateHandle.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Already a leaderboard operation running."));
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
        return;
    }

    this->QueryLeaderboardsDelegateHandle = LeaderboardsInterface->AddOnLeaderboardReadCompleteDelegate_Handle(
        FOnLeaderboardReadComplete::FDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleLeaderboardResult,
            ReadRef,
            WorldContextObject,
            OnDone));
    if (!LeaderboardsInterface->ReadLeaderboardsAroundRank(0, 100, ReadRef))
    {
        LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
        this->QueryLeaderboardsDelegateHandle.Reset();
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
    }
}

void UExampleCPPSubsystem::HandleLeaderboardResult(
    const bool bWasSuccessful,
    FOnlineLeaderboardReadRef LeaderboardRef,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryLeaderboardsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    check(Subsystem != nullptr);
    IOnlineLeaderboardsPtr LeaderboardsInterface = Subsystem->GetLeaderboardsInterface();
    LeaderboardsInterface->ClearOnLeaderboardReadCompleteDelegate_Handle(this->QueryLeaderboardsDelegateHandle);
    this->QueryLeaderboardsDelegateHandle.Reset();

    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPLeaderboardEntry>());
        return;
    }

    TArray<FExampleCPPLeaderboardEntry> Results;
    for (auto Row : LeaderboardRef->Rows)
    {
        FExampleCPPLeaderboardEntry Entry;
        Entry.PlayerId = Row.PlayerId->ToString();
        Entry.PlayerNickname = Row.NickName;
        Entry.Rank = Row.Rank;
        if (Row.Columns.Contains(TEXT("Score")))
        {
            // For global leaderboards, column is always called "Score".
            Row.Columns[TEXT("Score")].GetValue(Entry.Score);
        }
        else
        {
            // For friend leaderboards, we requested the "TestScore" stat.
            Row.Columns[TEXT("TestScore")].GetValue(Entry.Score);
        }
        Results.Add(Entry);
    }

    OnDone.ExecuteIfBound(true, Results);
}