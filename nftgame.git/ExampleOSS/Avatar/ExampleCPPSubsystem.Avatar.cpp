// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"
#include "../Interfaces/OnlineAvatarInterface.h"
#include "Engine/Texture2D.h"
#include "OnlineSubsystemUtils.h"

void UExampleCPPSubsystem::GetAvatar(
    int32 LocalUserId,
    const FUniqueNetIdRepl &TargetUserId,
    UTexture2D *DefaultTexture,
    FExampleCPPSubsystemGetAvatarComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        OnDone.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    if (Identity->GetUniquePlayerId(LocalUserId).IsValid() == false)
    {
        OnDone.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    TSharedPtr<IOnlineAvatar, ESPMode::ThreadSafe> Avatar = Online::GetAvatarInterface(Subsystem);
    if (Avatar.IsValid() == false)
    {
        OnDone.ExecuteIfBound(false, DefaultTexture);
        return;
    }

    Avatar->GetAvatar(
        *Identity->GetUniquePlayerId(LocalUserId),
        *TargetUserId,
        DefaultTexture,
        FOnGetAvatarComplete::CreateUObject(this, &UExampleCPPSubsystem::HandleGetAvatarComplete, OnDone));
}

void UExampleCPPSubsystem::HandleGetAvatarComplete(
    bool bWasSucessful,
    TSoftObjectPtr<UTexture2D> ResultTexture,
    FExampleCPPSubsystemGetAvatarComplete OnDone)
{
    OnDone.ExecuteIfBound(bWasSucessful, ResultTexture);
}