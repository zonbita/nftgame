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

void UExampleCPPSubsystem::StartUpdatePresence(
    const UObject *WorldContextObject,
    const FString &NewPresenceString,
    FExampleCPPSubsystemUpdatePresenceComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePresencePtr Presence = Subsystem->GetPresenceInterface();

    FOnlineUserPresenceStatus Status;
    Status.State = EOnlinePresenceState::Online;
    Status.StatusStr = NewPresenceString;

    Presence->SetPresence(
        *Identity->GetUniquePlayerId(0).Get(),
        Status,
        IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleUpdatePresenceComplete,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleUpdatePresenceComplete(
    const class FUniqueNetId &UserId,
    const bool bWasSuccessful,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemUpdatePresenceComplete OnDone)
{
    OnDone.ExecuteIfBound(bWasSuccessful);
}

void UExampleCPPSubsystem::QueryPresence(
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryPresenceComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    check(Subsystem != nullptr);
    IOnlinePresencePtr PresenceInterface = Subsystem->GetPresenceInterface();
    check(PresenceInterface != nullptr);
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    check(Identity != nullptr);

    PresenceInterface->QueryPresence(
        *Identity->GetUniquePlayerId(0),
        IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryPresenceComplete));
}

void UExampleCPPSubsystem::HandleQueryPresenceComplete(const class FUniqueNetId &UserId, const bool bWasSuccessful)
{
    UE_LOG(LogTemp, Verbose, TEXT("Query presence result: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));
}

void UExampleCPPSubsystem::OnPresenceReceived(
    const class FUniqueNetId &UserId,
    const TSharedRef<FOnlineUserPresence> &Presence)
{
    PresenceUpdated.Broadcast(FUniqueNetIdRepl(UserId).ToString(), Presence->Status.ToDebugString());
}