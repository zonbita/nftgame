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

bool UExampleCPPSubsystem::IsSignedIn(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return false;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    return Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}

FString UExampleCPPSubsystem::GetLoggedInUsername(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TEXT("");
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
            if (Account.IsValid())
            {
                if (Account->GetDisplayName().IsEmpty())
                {
                    return TEXT("(No Username Set, but you are Logged In)");
                }
                else
                {
                    return Account->GetDisplayName();
                }
            }
        }
    }

    return TEXT("Not Logged In");
}

FString UExampleCPPSubsystem::GetLoggedInProductUserId(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TEXT("");
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
            if (Account.IsValid())
            {
                FString OutAttr;
                if (Account->GetUserAttribute(TEXT("productUserId"), OutAttr))
                {
                    return OutAttr;
                }
            }
        }
    }

    return TEXT("");
}

FString UExampleCPPSubsystem::GetLoggedInEpicAccountId(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TEXT("");
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
            if (Account.IsValid())
            {
                FString OutTypes;
                if (Account->GetUserAttribute(TEXT("externalAccountTypes"), OutTypes))
                {
                    TArray<FString> TypesArr;
                    OutTypes.ParseIntoArray(TypesArr, TEXT(","));
                    if (TypesArr.Contains("epic"))
                    {
                        FString OutAttr;
                        if (Account->GetUserAttribute(TEXT("externalAccount.epic.id"), OutAttr))
                        {
                            return OutAttr;
                        }
                    }
                }
            }
        }
    }

    return TEXT("");
}

FString UExampleCPPSubsystem::GetLoggedInAuthAttribute(const UObject *WorldContextObject, const FString &InAuthAttrName)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return TEXT("");
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
            if (Account.IsValid())
            {
                FString OutAttr;
                if (Account->GetAuthAttribute(InAuthAttrName, OutAttr))
                {
                    return OutAttr;
                }
            }
        }
    }

    return TEXT("");
}

FUniqueNetIdRepl UExampleCPPSubsystem::GetLoggedInUserId(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return FUniqueNetIdRepl();
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            return FUniqueNetIdRepl(Id);
        }
    }

    return FUniqueNetIdRepl();
}

bool UExampleCPPSubsystem::CanLinkToEpicGamesAccount(const UObject *WorldContextObject)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return false;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    if (Identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    {
        TSharedPtr<const FUniqueNetId> Id = Identity->GetUniquePlayerId(0);
        if (Id.IsValid())
        {
            TSharedPtr<FUserOnlineAccount> Account = Identity->GetUserAccount(*Id);
            if (Account.IsValid())
            {
                FString OutVal;
                if (Account->GetAuthAttribute(TEXT("crossPlatform.canLink"), OutVal))
                {
                    return OutVal == TEXT("true");
                }
            }
        }
    }
    return false;
}

void UExampleCPPSubsystem::StartLogin(
    const UObject *WorldContextObject,
    int32 LocalUserNum,
    FExampleCPPSubsystemLoginComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    // note: This example code only supports running one Login process at once; if you want to support
    // multiple in flight you probably need to make a latent blueprint node.
    if (this->LoginDelegateHandle.IsValid())
    {
        OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
        return;
    }
    this->LoginDelegateHandle = Identity->AddOnLoginCompleteDelegate_Handle(
        LocalUserNum,
        FOnLoginComplete::FDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleLoginComplete,
            WorldContextObject,
            OnDone));
    if (!Identity->AutoLogin(LocalUserNum))
    {
        OnDone.ExecuteIfBound(false, TEXT("Call didn't start"));
    }
}

void UExampleCPPSubsystem::HandleLoginComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &Error,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemLoginComplete OnDone)
{
    OnDone.ExecuteIfBound(bWasSuccessful, Error);

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return;
    }

    Identity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->LoginDelegateHandle);
    this->LoginDelegateHandle.Reset();
}

void UExampleCPPSubsystem::StartLogout(const UObject *WorldContextObject, FExampleCPPSubsystemLogoutComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();

    // note: This example code only supports running one Logout process at once; if you want to support
    // multiple in flight you probably need to make a latent blueprint node.
    if (this->LogoutDelegateHandle.IsValid())
    {
        OnDone.ExecuteIfBound(false);
        return;
    }
    this->LogoutDelegateHandle = Identity->AddOnLogoutCompleteDelegate_Handle(
        0,
        FOnLogoutComplete::FDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleLogoutComplete,
            WorldContextObject,
            OnDone));
    if (!Identity->Logout(0))
    {
        OnDone.ExecuteIfBound(false);
    }
}

void UExampleCPPSubsystem::HandleLogoutComplete(
    int32 _UnusedLocalUserNum,
    bool bWasSuccessful,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemLogoutComplete OnDone)
{
    OnDone.ExecuteIfBound(bWasSuccessful);

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        return;
    }

    Identity->ClearOnLogoutCompleteDelegate_Handle(0, this->LogoutDelegateHandle);
    this->LogoutDelegateHandle.Reset();
}