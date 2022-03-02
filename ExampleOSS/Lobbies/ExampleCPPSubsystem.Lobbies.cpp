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

void UExampleCPPSubsystem::CreateLobby(
    const int64 &ExampleAttributeData,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    FVariantData ExampleVariantData = FVariantData();
    ExampleVariantData.SetValue(ExampleAttributeData);

    TSharedPtr<FOnlineLobbyTransaction> OnlineLobbyTransaction =
        Lobby->MakeCreateLobbyTransaction(*Identity->GetUniquePlayerId(0));
    OnlineLobbyTransaction->SetMetadata.Add("ExampleAttributeData", ExampleVariantData);
    OnlineLobbyTransaction->SetMetadata.Add("SearchValue", true);
    OnlineLobbyTransaction->Public = true;
    OnlineLobbyTransaction->Locked = false;

    FOnLobbyCreateOrConnectComplete OnLobbyCreateOrConnectComplete;
    OnLobbyCreateOrConnectComplete.BindUObject(this, &UExampleCPPSubsystem::HandleCreateLobbyComplete, OnDone);

    if (!Lobby->CreateLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyTransaction, OnLobbyCreateOrConnectComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby creation exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void UExampleCPPSubsystem::ConnectLobby(
    UExampleCPPLobbyId *LobbyId,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    FOnLobbyCreateOrConnectComplete OnLobbyCreateOrConnectComplete;
    OnLobbyCreateOrConnectComplete.BindUObject(this, &UExampleCPPSubsystem::HandleConnectLobbyComplete, OnDone);

    if (!Lobby->ConnectLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OnLobbyCreateOrConnectComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby connect exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void UExampleCPPSubsystem::DisconnectLobby(
    UExampleCPPLobbyId *LobbyId,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage =
            FText::FromString("Unable to create native lobby id from passed in lobby id");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
        return;
    }

    FOnLobbyOperationComplete OnLobbyOperationComplete;
    OnLobbyOperationComplete.BindUObject(this, &UExampleCPPSubsystem::HandleDisconnectLobbyComplete, OnDone);

    if (!Lobby->DisconnectLobby(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OnLobbyOperationComplete))
    {
        ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = FText::FromString("Lobby disconnect exited early.");
        OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, false);
    }
}

void UExampleCPPSubsystem::SearchLobby(FExampleCPPSubssytemOnSearchLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FExampleCPPLobbySearchResult ExampleCPPLobbySearchResult;

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
        return;
    }

    FOnLobbySearchComplete OnLobbySearchComplete;
    OnLobbySearchComplete.BindUObject(this, &UExampleCPPSubsystem::HandleSearchLobbyComplete, OnDone);

    /* This is an example search attribute. You should modify this to customize your own lobby search criteria.
       You can structure your lobby search attributes similar to this by creating additional variant data and
       Adding however many parameters you would like to refine your search. It's likely better as it's own
       separate function, the example does it in one place so it's easier to read. */

    FOnlineLobbySearchQueryFilter OnlineLobbySearchQueryFilter =
        FOnlineLobbySearchQueryFilter("SearchValue", true, EOnlineLobbySearchQueryFilterComparator::Equal);

    FOnlineLobbySearchQuery OnlineLobbySearchQuery;
    OnlineLobbySearchQuery.Filters.Add(OnlineLobbySearchQueryFilter);

    if (!Lobby->Search(*Identity->GetUniquePlayerId(0), OnlineLobbySearchQuery, OnLobbySearchComplete))
    {
        ExampleCPPLobbySearchResult.ErrorMessage = FText::FromString("Lobby search exited early.");
        OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, false);
    }
}

void UExampleCPPSubsystem::UpdateLobby(
    UExampleCPPLobbyId *LobbyId,
    const int64 &ExampleAttributeData,
    FExampleCPPSubssytemOnUpdateLobbyComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    FExampleCPPUpdateLobbyResult ExampleCPPUpdateLobbyResult;

    if (LobbyId == nullptr)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("LobbyId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Unable to get identity interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("UniqueNetId was not valid");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Unable to get lobby interface from subsystem");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage =
            FText::FromString("Unable to create native lobby id from passed in lobby id");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
        return;
    }

    FVariantData ExampleVariantData = FVariantData();
    ExampleVariantData.SetValue(ExampleAttributeData);

    TSharedPtr<FOnlineLobbyTransaction> OnlineLobbyTransaction =
        Lobby->MakeCreateLobbyTransaction(*Identity->GetUniquePlayerId(0));
    OnlineLobbyTransaction->SetMetadata.Add("ExampleAttributeData", ExampleVariantData);

    FOnLobbyOperationComplete OnLobbyOperationComplete;
    OnLobbyOperationComplete.BindUObject(this, &UExampleCPPSubsystem::HandleUpdateLobbyComplete, OnDone);

    if (!Lobby->UpdateLobby(
            *Identity->GetUniquePlayerId(0),
            *OnlineLobbyId,
            *OnlineLobbyTransaction,
            OnLobbyOperationComplete))
    {
        ExampleCPPUpdateLobbyResult.ErrorMessage = FText::FromString("Lobby update exited early.");
        OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, false);
    }
}

bool UExampleCPPSubsystem::GetMemberCount(UExampleCPPLobbyId *LobbyId, int32 &OutMemberCount)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return false;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return false;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return false;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return false;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return false;
    }

    return Lobby->GetMemberCount(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, OutMemberCount);
}

bool UExampleCPPSubsystem::GetMemberUserId(UExampleCPPLobbyId *LobbyId, const int32 &MemberIndex, FString &OutMemberId)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return false;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return false;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return false;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return false;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return false;
    }

    int32 MemberCount;
    const bool bWasSuccessful = Lobby->GetMemberCount(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, MemberCount);

    if (bWasSuccessful == false || MemberCount <= 0)
    {
        return false;
    }

    TSharedPtr<const FUniqueNetId> MemberId;
    const bool bFoundMember =
        Lobby->GetMemberUserId(*Identity->GetUniquePlayerId(0), *OnlineLobbyId, MemberIndex, MemberId);

    if (MemberId.IsValid() == false)
    {
        return false;
    }

    OutMemberId = MemberId->ToString();
    return bFoundMember;
}

UExampleCPPPartyInvite *UExampleCPPSubsystem::ConvertLobbyIdToPartyJoinInfo(UExampleCPPLobbyId *LobbyId)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    check(Subsystem);

    if (LobbyId == nullptr)
    {
        return nullptr;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return nullptr;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        return nullptr;
    }

    TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> Lobby = Online::GetLobbyInterface(Subsystem);
    if (Lobby == nullptr || Lobby.IsValid() == false)
    {
        return nullptr;
    }

    TSharedPtr<const FOnlineLobbyId> OnlineLobbyId = LobbyId->ToNative().AsShared();

    if (OnlineLobbyId.IsValid() == false)
    {
        return nullptr;
    }

    IOnlinePartyPtr Party = Subsystem->GetPartyInterface();
    if (Party == nullptr)
    {
        return nullptr;
    }

    FString JoinInfoJson = Party->MakeJoinInfoJson(*Identity->GetUniquePlayerId(0), *OnlineLobbyId);

    if (JoinInfoJson.IsEmpty())
    {
        return nullptr;
    }

    IOnlinePartyJoinInfoConstPtr JoinInfoPtr = Party->MakeJoinInfoFromJson(JoinInfoJson);
    if (!JoinInfoPtr.IsValid())
    {
        return nullptr;
    }

    UExampleCPPPartyInvite *PartyInvite = NewObject<UExampleCPPPartyInvite>(this);
    PartyInvite->PartyInvite = JoinInfoPtr;

    return PartyInvite;
}

void UExampleCPPSubsystem::HandleCreateLobbyComplete(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TSharedPtr<FOnlineLobby> &Lobby,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;
    ExampleCPPCreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    ExampleCPPCreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    ExampleCPPCreateOrConnectLobbyResult.UserId = UserId.AsShared();
    ExampleCPPCreateOrConnectLobbyResult.Lobby = UExampleCPPLobby::FromNative(Lobby);

    OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, Error.bSucceeded);
}

void UExampleCPPSubsystem::HandleConnectLobbyComplete(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TSharedPtr<FOnlineLobby> &Lobby,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;
    ExampleCPPCreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    ExampleCPPCreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    ExampleCPPCreateOrConnectLobbyResult.UserId = UserId.AsShared();
    ExampleCPPCreateOrConnectLobbyResult.Lobby = UExampleCPPLobby::FromNative(Lobby);

    OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, Error.bSucceeded);
}

void UExampleCPPSubsystem::HandleDisconnectLobbyComplete(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone)
{
    FExampleCPPCreateOrConnectLobbyResult ExampleCPPCreateOrConnectLobbyResult;
    ExampleCPPCreateOrConnectLobbyResult.ErrorRaw = Error.ErrorRaw;
    ExampleCPPCreateOrConnectLobbyResult.ErrorCode = Error.ErrorCode;
    ExampleCPPCreateOrConnectLobbyResult.ErrorMessage = Error.ErrorMessage;
    ExampleCPPCreateOrConnectLobbyResult.UserId = UserId.AsShared();

    OnDone.ExecuteIfBound(ExampleCPPCreateOrConnectLobbyResult, Error.bSucceeded);
}

void UExampleCPPSubsystem::HandleSearchLobbyComplete(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    const TArray<TSharedRef<const FOnlineLobbyId>> &LobbyIds,
    FExampleCPPSubssytemOnSearchLobbyComplete OnDone)
{
    FExampleCPPLobbySearchResult ExampleCPPLobbySearchResult;
    ExampleCPPLobbySearchResult.ErrorRaw = Error.ErrorRaw;
    ExampleCPPLobbySearchResult.ErrorCode = Error.ErrorCode;
    ExampleCPPLobbySearchResult.ErrorMessage = Error.ErrorMessage;
    ExampleCPPLobbySearchResult.UserId = UserId.AsShared();

    TArray<UExampleCPPLobbyId *> Lobbies;
    Lobbies.Reserve(LobbyIds.Num());

    for (auto &&LobbyId : LobbyIds)
    {
        UExampleCPPLobbyId *ExampleLobbyId = UExampleCPPLobbyId::FromNative(LobbyId);
        Lobbies.Add(ExampleLobbyId);
    }

    ExampleCPPLobbySearchResult.Lobbies = Lobbies;
    OnDone.ExecuteIfBound(ExampleCPPLobbySearchResult, Error.bSucceeded);
}

void UExampleCPPSubsystem::HandleUpdateLobbyComplete(
    const FOnlineError &Error,
    const FUniqueNetId &UserId,
    FExampleCPPSubssytemOnUpdateLobbyComplete OnDone)
{
    FExampleCPPUpdateLobbyResult ExampleCPPUpdateLobbyResult;
    ExampleCPPUpdateLobbyResult.ErrorRaw = Error.ErrorRaw;
    ExampleCPPUpdateLobbyResult.ErrorCode = Error.ErrorCode;
    ExampleCPPUpdateLobbyResult.ErrorMessage = Error.ErrorMessage;
    ExampleCPPUpdateLobbyResult.UserId = UserId.AsShared();

    OnDone.ExecuteIfBound(ExampleCPPUpdateLobbyResult, Error.bSucceeded);
}

/********** Lobbies.LobbiesTypes.h **********/

UExampleCPPLobbyId *UExampleCPPLobbyId::FromNative(const FOnlineLobbyId &InObj)
{
    auto Ref = NewObject<UExampleCPPLobbyId>();
    Ref->LobbyId = TSharedPtr<const FOnlineLobbyId>(InObj.AsShared());
    return Ref;
}

UExampleCPPLobbyId *UExampleCPPLobbyId::FromNative(const TSharedPtr<const FOnlineLobbyId> &InObj)
{
    auto Ref = NewObject<UExampleCPPLobbyId>();
    Ref->LobbyId = InObj;
    return Ref;
}

UExampleCPPLobbyId *UExampleCPPLobby::GetId()
{
    if (!this->Lobby.IsValid())
    {
        return nullptr;
    }

    return UExampleCPPLobbyId::FromNative(this->Lobby->Id);
}

FUniqueNetIdRepl UExampleCPPLobby::GetOwnerId()
{
    if (!this->Lobby.IsValid())
    {
        return FUniqueNetIdRepl();
    }

    return Lobby->OwnerId;
}

UExampleCPPLobby *UExampleCPPLobby::FromNative(TSharedPtr<FOnlineLobby> InObj)
{
    auto Ref = NewObject<UExampleCPPLobby>();
    Ref->Lobby = TSharedPtr<FOnlineLobby>(InObj);
    return Ref;
}

void UExampleCPPOnlineLobbyTransaction::SetMetadata(const FString &Key, const FVariantData &Value)
{
    Txn->SetMetadata.Add(Key, Value);
}

void UExampleCPPOnlineLobbyTransaction::SetMetadataByMap(const TMap<FString, FVariantData> &Metadata)
{
    for (auto &&It : Metadata)
    {
        Txn->SetMetadata.Add(It.Key, It.Value);
    }
}

void UExampleCPPOnlineLobbyTransaction::DeleteMetadata(const FString &Key)
{
    Txn->DeleteMetadata.Remove(Key);
}

void UExampleCPPOnlineLobbyTransaction::DeleteMetadataByArray(const TArray<FString> &MetadataKeys)
{
    for (auto &&It : MetadataKeys)
    {
        Txn->DeleteMetadata.Remove(It);
    }
}

void UExampleCPPOnlineLobbyTransaction::SetLocked(bool Locked)
{
    Txn->Locked = Locked;
}

void UExampleCPPOnlineLobbyTransaction::SetCapacity(int64 Capacity)
{
    Txn->Capacity = Capacity;
}

void UExampleCPPOnlineLobbyTransaction::SetPublic(bool Public)
{
    Txn->Public = Public;
}

UExampleCPPOnlineLobbyTransaction *UExampleCPPOnlineLobbyTransaction::FromNative(
    TSharedPtr<FOnlineLobbyTransaction> InObj)
{
    auto Ref = NewObject<UExampleCPPOnlineLobbyTransaction>();
    Ref->Txn = InObj;
    return Ref;
}

void UExampleCPPOnlineLobbyMemberTransaction::SetMetadata(const FString &Key, const FVariantData &Value)
{
    Txn->SetMetadata.Add(Key, Value);
}

void UExampleCPPOnlineLobbyMemberTransaction::SetMetadataByMap(const TMap<FString, FVariantData> &Metadata)
{
    for (auto &&It : Metadata)
    {
        Txn->SetMetadata.Add(It.Key, It.Value);
    }
}

void UExampleCPPOnlineLobbyMemberTransaction::DeleteMetadata(const FString &Key)
{
    Txn->DeleteMetadata.Remove(Key);
}

void UExampleCPPOnlineLobbyMemberTransaction::DeleteMetadataByArray(const TArray<FString> &MetadataKeys)
{
    for (auto &&It : MetadataKeys)
    {
        Txn->DeleteMetadata.Remove(It);
    }
}

UExampleCPPOnlineLobbyMemberTransaction *UExampleCPPOnlineLobbyMemberTransaction::FromNative(
    TSharedPtr<FOnlineLobbyMemberTransaction> InObj)
{
    auto Ref = NewObject<UExampleCPPOnlineLobbyMemberTransaction>();
    Ref->Txn = InObj;
    return Ref;
}