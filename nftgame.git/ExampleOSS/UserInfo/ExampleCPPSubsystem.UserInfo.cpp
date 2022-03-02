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

void UExampleCPPSubsystem::QueryUserInfo(
    const UObject *WorldContextObject,
    FString ProductUserIdInput,
    FString EpicAccountIdInput,
    FExampleCPPSubsystemQueryUserInfoComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    TSharedPtr<const FUniqueNetId> UniqueNetId =
        Identity->CreateUniquePlayerId(FString::Printf(TEXT("%s|%s"), *ProductUserIdInput, *EpicAccountIdInput));
    if (UniqueNetId == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("The IDs you entered were invalid."));
        return;
    }

    // note: This example code only supports running one QueryUserInfo process at once; if you want to support
    // multiple in flight you probably need to make a latent blueprint node.
    if (this->QueryUserInfoDelegateHandle.IsValid())
    {
        OnDone.ExecuteIfBound(TEXT("Already a QueryUserInfo running."));
        return;
    }

    this->QueryUserInfoDelegateHandle = User->AddOnQueryUserInfoCompleteDelegate_Handle(
        0,
        FOnQueryUserInfoComplete::FDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryUserInfoComplete,
            WorldContextObject,
            OnDone));

    TArray<TSharedRef<const FUniqueNetId>> UserIds;
    UserIds.Add(UniqueNetId.ToSharedRef());
    User->QueryUserInfo(0, UserIds);
}

void UExampleCPPSubsystem::HandleQueryUserInfoComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
    const FString &ErrorStr,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryUserInfoComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
        return;
    }
    IOnlineUserPtr User = Subsystem->GetUserInterface();
    if (User == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Interface not available."));
        return;
    }

    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *ErrorStr));
    }
    else
    {
        if (UserIds.Num() == 0)
        {
            OnDone.ExecuteIfBound(TEXT("No such user was found."));
        }
        else if (UserIds.Num() == 1)
        {
            auto Id = UserIds[0];
            TSharedPtr<FOnlineUser> UserAcc = User->GetUserInfo(0, *Id);

            if (UserAcc.IsValid())
            {
                OnDone.ExecuteIfBound(FString::Printf(TEXT("%s -> %s"), *Id->ToString(), *UserAcc->GetDisplayName()));
            }
            else
            {
                OnDone.ExecuteIfBound(FString::Printf(TEXT("%s -> (bug!!)"), *Id->ToString()));
            }
        }
        else
        {
            OnDone.ExecuteIfBound(TEXT("Bug: Too many user IDs returned!"));
        }
    }

    User->ClearOnQueryUserInfoCompleteDelegate_Handle(0, this->QueryUserInfoDelegateHandle);
    this->QueryUserInfoDelegateHandle.Reset();
}

void UExampleCPPSubsystem::QueryUserIdMapping(
    const UObject *WorldContextObject,
    FString DisplayNameInput,
    FExampleCPPSubsystemQueryUserIdMappingComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    User->QueryUserIdMapping(
        *Identity->GetUniquePlayerId(0),
        DisplayNameInput,
        IOnlineUser::FOnQueryUserMappingComplete::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryUserMappingComplete,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleQueryUserMappingComplete(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &DisplayNameOrEmail,
    const FUniqueNetId &FoundUserId,
    const FString &Error,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryUserIdMappingComplete OnDone)
{
    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *Error));
        return;
    }

    OnDone.ExecuteIfBound(FString::Printf(TEXT("Found user: %s"), *FoundUserId.ToString()));
}

void UExampleCPPSubsystem::QueryExternalIdMappings(
    const UObject *WorldContextObject,
    FString PlatformType,
    FString ExternalIdsStr,
    FExampleCPPSubsystemQueryExternalIdMappingsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineUserPtr User = Subsystem->GetUserInterface();

    TArray<FString> ExternalIdsTemp, ExternalIds;
    ExternalIdsStr.ParseIntoArray(ExternalIdsTemp, TEXT("\n"));
    for (auto ExternalIdTemp : ExternalIdsTemp)
    {
        ExternalIds.Add(ExternalIdTemp.TrimStartAndEnd());
    }

    FExternalIdQueryOptions Opts = {};
    Opts.AuthType = PlatformType;
    Opts.bLookupByDisplayName = false;

    User->QueryExternalIdMappings(
        *Identity->GetUniquePlayerId(0),
        Opts,
        ExternalIds,
        IOnlineUser::FOnQueryExternalIdMappingsComplete::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryExternalIdMappings,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleQueryExternalIdMappings(
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FExternalIdQueryOptions &QueryOptions,
    const TArray<FString> &ExternalIds,
    const FString &Error,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryExternalIdMappingsComplete OnDone)
{
    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(FString::Printf(TEXT("Error: %s"), *Error));
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Subsystem not available."));
        return;
    }
    IOnlineUserPtr User = Subsystem->GetUserInterface();
    if (User == nullptr)
    {
        OnDone.ExecuteIfBound(TEXT("Interface not available."));
        return;
    }

    TArray<FString> Lines;
    for (auto ExternalId : ExternalIds)
    {
        auto EUID = User->GetExternalIdMapping(QueryOptions, ExternalId);
        if (EUID.IsValid())
        {
            Lines.Add(FString::Printf(TEXT("%s = %s"), *ExternalId, *EUID->ToString()));
        }
        else
        {
            Lines.Add(FString::Printf(TEXT("%s = not found"), *ExternalId));
        }
    }
    OnDone.ExecuteIfBound(FString::Join(Lines, TEXT("\n")));
}