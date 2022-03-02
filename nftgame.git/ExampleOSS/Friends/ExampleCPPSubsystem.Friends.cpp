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

TArray<UExampleCPPFriend *> UExampleCPPSubsystem::GetFriends(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TArray<UExampleCPPFriend *>();
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

    TArray<TSharedRef<FOnlineFriend>> FriendsRaw;
    if (!Friends->GetFriendsList(0, TEXT(""), FriendsRaw))
    {
        return TArray<UExampleCPPFriend *>();
    }

    TArray<UExampleCPPFriend *> Results;
    for (auto Friend : FriendsRaw)
    {
        UExampleCPPFriend *FriendResult = NewObject<UExampleCPPFriend>(this);
        FriendResult->Friend = Friend;
        Results.Add(FriendResult);
    }
    return Results;
}

void UExampleCPPSubsystem::StartReadFriends(
    const UObject *WorldContextObject,
    FExampleCPPSubsystemReadFriendsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

    if (!Friends->ReadFriendsList(
            0,
            TEXT(""),
            FOnReadFriendsListComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleReadFriendsComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleReadFriendsComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const FString &ListName,
    const FString &ErrorStr,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemReadFriendsComplete OnDone)
{
    OnDone.ExecuteIfBound(bWasSuccessful);
}

// StartInviteFriend is inside ExampleCPPSubsystem.Parties.cpp.