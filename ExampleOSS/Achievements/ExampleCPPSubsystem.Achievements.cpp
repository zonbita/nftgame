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

void UExampleCPPSubsystem::QueryAchievements(
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryAchievementsComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPAchievement>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();

    AchievementsInterface->QueryAchievementDescriptions(
        *Identity->GetUniquePlayerId(0),
        FOnQueryAchievementsCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryAchievementDescriptions,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleQueryAchievementDescriptions(
    const FUniqueNetId &UserId,
    const bool bWasSuccessful,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryAchievementsComplete OnDone)
{
    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPAchievement>());
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPAchievement>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();

    AchievementsInterface->QueryAchievements(
        *Identity->GetUniquePlayerId(0),
        FOnQueryAchievementsCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleQueryAchievements,
            WorldContextObject,
            OnDone));
}

void UExampleCPPSubsystem::HandleQueryAchievements(
    const FUniqueNetId &UserId,
    const bool bWasSuccessful,
    const UObject *WorldContextObject,
    FExampleCPPSubsystemQueryAchievementsComplete OnDone)
{
    if (!bWasSuccessful)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPAchievement>());
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FExampleCPPAchievement>());
        return;
    }
    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    IOnlineAchievementsPtr AchievementsInterface = Subsystem->GetAchievementsInterface();

    TArray<FOnlineAchievement> AchievementsData;
    verify(
        AchievementsInterface->GetCachedAchievements(*Identity->GetUniquePlayerId(0), AchievementsData) ==
        EOnlineCachedResult::Success);

    TArray<FExampleCPPAchievement> Results;
    for (auto Data : AchievementsData)
    {
        FExampleCPPAchievement Achievement;
        Achievement.Id = Data.Id;
        Achievement.Progress = Data.Progress;
        Achievement.bUnlocked = Data.Progress >= 100.0f;

        FOnlineAchievementDesc AchievementDesc;
        verify(
            AchievementsInterface->GetCachedAchievementDescription(Data.Id, AchievementDesc) ==
            EOnlineCachedResult::Success);
        Achievement.DisplayName = AchievementDesc.Title;
        Achievement.Description = Achievement.bUnlocked ? AchievementDesc.UnlockedDesc : AchievementDesc.LockedDesc;

        Results.Add(Achievement);
    }

    OnDone.ExecuteIfBound(true, Results);
}