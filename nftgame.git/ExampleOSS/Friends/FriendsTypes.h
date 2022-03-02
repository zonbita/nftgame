// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "Containers/Set.h"
#include "CoreMinimal.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "FriendsTypes.generated.h"

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPFriend : public UObject
{
    GENERATED_BODY()

public:
    TSharedPtr<FOnlineFriend> Friend;

    UFUNCTION(BlueprintPure)
    FUniqueNetIdRepl GetUserId()
    {
        return FUniqueNetIdRepl(*Friend->GetUserId());
    }

    UFUNCTION(BlueprintPure)
    FString GetUserIdString()
    {
        return Friend->GetUserId()->ToString();
    }

    UFUNCTION(BlueprintPure)
    FName GetUserIdType()
    {
        return Friend->GetUserId()->GetType();
    }

    UFUNCTION(BlueprintPure)
    FString GetDisplayName()
    {
        return Friend->GetDisplayName();
    }

    UFUNCTION(BlueprintPure)
    FString GetRealName()
    {
        return Friend->GetRealName();
    }

    UFUNCTION(BlueprintPure)
    FString GetWrappedFriendsSummary()
    {
        TArray<FString> Summary;
        FString SubsystemNamesStr;
        if (Friend->GetUserAttribute(TEXT("eosSynthetic.subsystemNames"), SubsystemNamesStr))
        {
            TArray<FString> SubsystemNames;
            SubsystemNamesStr.ParseIntoArray(SubsystemNames, TEXT(","));
            for (const auto &SubsystemName : SubsystemNames)
            {
                FString Id, RealName, DisplayName;
                Friend->GetUserAttribute(FString::Printf(TEXT("eosSynthetic.friend.%s.id"), *SubsystemName), Id);
                Friend->GetUserAttribute(
                    FString::Printf(TEXT("eosSynthetic.friend.%s.realName"), *SubsystemName),
                    RealName);
                Friend->GetUserAttribute(
                    FString::Printf(TEXT("eosSynthetic.friend.%s.displayName"), *SubsystemName),
                    DisplayName);
                Summary.Add(FString::Printf(
                    TEXT("subsystem: %s id: %s real name: %s display name: %s"),
                    *SubsystemName,
                    *Id,
                    *RealName,
                    *DisplayName));
            }
        }
        return FString::Join(Summary, TEXT("\n"));
    }

    UFUNCTION(BlueprintCallable)
    FString ToString()
    {
        if (Friend == nullptr)
        {
            return TEXT("");
        }

        return Friend->GetDisplayName();
    }
};