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

void UExampleCPPSubsystem::OnPartyDataReceived(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FName &Namespace,
    const FOnlinePartyData &PartyData)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem != nullptr);
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr PartySystem = Subsystem->GetPartyInterface();

    auto Party = PartySystem->GetParty(LocalUserId, PartyId);
    if (!Party.IsValid() || *Party->LeaderId == LocalUserId)
    {
        // Ignore this request because we're either not in the party (?) or
        // we're the leader.
        return;
    }

    FVariantData Data;
    if (PartyData.GetAttribute(TEXT("SessionFollowString"), Data))
    {
        if (Data.GetType() == EOnlineKeyValuePairDataType::String)
        {
            // This party has a SessionFollowString, parse it.
            FString SessionFollowString;
            Data.GetValue(SessionFollowString);
            TArray<FString> SessionFollowComponents;
            SessionFollowString.ParseIntoArray(SessionFollowComponents, TEXT("|"));
            FString RequestId = SessionFollowComponents[0];
            FString SessionId = SessionFollowComponents[1];

            if (this->SessionFollowConsumedIds.Contains(RequestId))
            {
                // We have already acted upon this before.
                return;
            }

            this->SessionFollowConsumedIds.Add(RequestId);

            // Try to find the session.
            Session->FindSessionById(
                *Identity->GetUniquePlayerId(0),
                *Session->CreateSessionIdFromString(SessionId),
                *Identity->GetUniquePlayerId(0),
                FOnSingleSessionResultCompleteDelegate::CreateUObject(
                    this,
                    &UExampleCPPSubsystem::OnFindSessionForLeaderFollow,
                    SessionFollowString));
        }
    }
}

void UExampleCPPSubsystem::OnPartyInvitesChanged(const FUniqueNetId &LocalUserId)
{
    this->OnInvitationsChanged.Broadcast();
}

TArray<UExampleCPPPartyId *> UExampleCPPSubsystem::GetJoinedParties(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TArray<UExampleCPPPartyId *>();
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    TArray<TSharedRef<const FOnlinePartyId>> Result;
    if (!Party->GetJoinedParties(*Identity->GetUniquePlayerId(0).Get(), Result))
    {
        return TArray<UExampleCPPPartyId *>();
    }

    TArray<UExampleCPPPartyId *> ResultIds;
    for (auto Entry : Result)
    {
        UExampleCPPPartyId *Id = NewObject<UExampleCPPPartyId>(this);
        Id->PartyId = Entry;
        ResultIds.Add(Id);
    }
    return ResultIds;
}

void UExampleCPPSubsystem::RequestPartyMembersFollowLeader(const UObject *WorldContextObject, FName SessionName)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr PartySystem = Subsystem->GetPartyInterface();
    IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

    auto LocalUser = Identity->GetUniquePlayerId(0).Get();

    TArray<TSharedRef<const FOnlinePartyId>> Result;
    if (!PartySystem->GetJoinedParties(*LocalUser, Result))
    {
        return;
    }

    auto SessionData = Session->GetNamedSession(SessionName);
    FString SessionFollowString = FString::Printf(TEXT("%d|%s"), FMath::Rand(), *SessionData->GetSessionIdStr());

    for (auto PartyId : Result)
    {
        auto Party = PartySystem->GetParty(*LocalUser, *PartyId);
        if (Party.IsValid())
        {
            if (*Party->LeaderId == *LocalUser)
            {
                // We are the leader, so we can make other members follow.
                auto PartyDataUpdate = MakeShared<FOnlinePartyData>();
                PartyDataUpdate->SetAttribute(TEXT("SessionFollowString"), SessionFollowString);
                if (!PartySystem->UpdatePartyData(*LocalUser, *PartyId, NAME_Default, *PartyDataUpdate))
                {
                    UE_LOG(
                        LogTemp,
                        Error,
                        TEXT("Could not request other members to follow because UpdatePartyData failed."));
                }
            }
        }
    }
}

void UExampleCPPSubsystem::StartCreateParty(
    const UObject *WorldContextObject,
    int PartyTypeId,
    FExampleCPPSubsystemCreatePartyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    TSharedRef<FPartyConfiguration> Config = MakeShared<FPartyConfiguration>();
    Config->bIsAcceptingMembers = true;
    Config->MaxMembers = 4;
    // You must set InvitePermissions to ::Anyone in order for synthetic parties to work.
    Config->InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;

    if (!Party->CreateParty(
            *Identity->GetUniquePlayerId(0).Get(),
            (FOnlinePartyTypeId)PartyTypeId,
            *Config,
            FOnCreatePartyComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleCreatePartyComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleCreatePartyComplete(
    const FUniqueNetId &LocalUserId,
    const TSharedPtr<const FOnlinePartyId> &PartyId,
    const ECreatePartyCompletionResult Result,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemCreatePartyComplete OnDone)
{
    OnDone.ExecuteIfBound(Result == ECreatePartyCompletionResult::Succeeded);
}

TArray<UExampleCPPPartyInvite *> UExampleCPPSubsystem::GetInvitations(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TArray<UExampleCPPPartyInvite *>();
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    TArray<IOnlinePartyJoinInfoConstRef> PendingInvites;
    if (!Party->GetPendingInvites(*Identity->GetUniquePlayerId(0).Get(), PendingInvites))
    {
        return TArray<UExampleCPPPartyInvite *>();
    }

    TArray<UExampleCPPPartyInvite *> Results;
    for (auto Entry : PendingInvites)
    {
        UExampleCPPPartyInvite *Id = NewObject<UExampleCPPPartyInvite>(this);
        Id->PartyInvite = Entry;
        Results.Add(Id);
    }
    return Results;
}

TArray<UExampleCPPPartyMemberId *> UExampleCPPSubsystem::GetPartyMembers(
    const UObject *WorldContextObject,
    UExampleCPPPartyId *PartyId)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TArray<UExampleCPPPartyMemberId *>();
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    TArray<FOnlinePartyMemberConstRef> PartyMembers;
    if (!Party->GetPartyMembers(*Identity->GetUniquePlayerId(0).Get(), *PartyId->PartyId.Get(), PartyMembers))
    {
        return TArray<UExampleCPPPartyMemberId *>();
    }

    TArray<UExampleCPPPartyMemberId *> Results;
    for (auto Entry : PartyMembers)
    {
        UExampleCPPPartyMemberId *Id = NewObject<UExampleCPPPartyMemberId>(this);
        Id->PartyMember = Entry;
        Results.Add(Id);
    }
    return Results;
}

bool UExampleCPPSubsystem::IsPartyLeader(const UObject *WorldContextObject, UExampleCPPPartyId *PartyId)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return false;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    return Party->IsMemberLeader(
        *Identity->GetUniquePlayerId(0).Get(),
        *PartyId->PartyId.Get(),
        *Identity->GetUniquePlayerId(0).Get());
}

void UExampleCPPSubsystem::StartJoinParty(
    const UObject *WorldContextObject,
    UExampleCPPPartyInvite *Invite,
    FExampleCPPSubsystemJoinPartyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    if (!Party->JoinParty(
            *Identity->GetUniquePlayerId(0).Get(),
            *Invite->PartyInvite,
            FOnJoinPartyComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleJoinPartyComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleJoinPartyComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const EJoinPartyCompletionResult Result,
    const int32 NotApprovedReason,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemJoinPartyComplete OnDone)
{
    OnDone.ExecuteIfBound(Result == EJoinPartyCompletionResult::Succeeded);
}

void UExampleCPPSubsystem::StartLeaveParty(
    const UObject *WorldContextObject,
    UExampleCPPPartyId *PartyId,
    FExampleCPPSubsystemLeavePartyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    if (!Party->LeaveParty(
            *Identity->GetUniquePlayerId(0).Get(),
            *PartyId->PartyId,
            FOnLeavePartyComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleLeavePartyComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleLeavePartyComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const ELeavePartyCompletionResult Result,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemLeavePartyComplete OnDone)
{
    OnDone.ExecuteIfBound(Result == ELeavePartyCompletionResult::Succeeded);
}

void UExampleCPPSubsystem::StartKickMember(
    const UObject *WorldContextObject,
    UExampleCPPPartyId *PartyId,
    UExampleCPPPartyMemberId *MemberId,
    FExampleCPPSubsystemKickMemberComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    if (!Party->KickMember(
            *Identity->GetUniquePlayerId(0).Get(),
            *PartyId->PartyId,
            *MemberId->PartyMember->GetUserId(),
            FOnKickPartyMemberComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleKickMemberComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleKickMemberComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &MemberId,
    const EKickMemberCompletionResult Result,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemKickMemberComplete OnDone)
{
    OnDone.ExecuteIfBound(Result == EKickMemberCompletionResult::Succeeded);
}

void UExampleCPPSubsystem::StartInviteFriend(
    const UObject *WorldContextObject,
    UExampleCPPPartyId *PartyId,
    UExampleCPPFriend *Friend,
    FExampleCPPSubsystemInviteFriendComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

    FPartyInvitationRecipient Recipient = FPartyInvitationRecipient(Friend->Friend->GetUserId());

    if (!Party->SendInvitation(
            *Identity->GetUniquePlayerId(0).Get(),
            *PartyId->PartyId,
            Recipient,
            FOnSendPartyInvitationComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleInviteFriendComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::CallInviteFriend(const UObject* WorldContextObject, UExampleCPPPartyId* PartyId,
    UExampleCPPPartyMemberId *MemberId, FExampleCPPSubsystemInviteFriendComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();


    FPartyInvitationRecipient Recipient = FPartyInvitationRecipient(*MemberId->PartyMember->GetUserId());

    if (!Party->SendInvitation(
            *Identity->GetUniquePlayerId(0).Get(),
            *PartyId->PartyId,
            Recipient,
            FOnSendPartyInvitationComplete::CreateUObject(
                this,
                &UExampleCPPSubsystem::HandleInviteFriendComplete,
                WorldContextObject,
                OnDone)))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleInviteFriendComplete(
    const FUniqueNetId &LocalUserId,
    const FOnlinePartyId &PartyId,
    const FUniqueNetId &RecipientId,
    const ESendPartyInvitationCompletionResult Result,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemInviteFriendComplete OnDone)
{
    OnDone.ExecuteIfBound(Result == ESendPartyInvitationCompletionResult::Succeeded);
}
