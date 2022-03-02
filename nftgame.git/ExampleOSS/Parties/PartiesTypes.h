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

#include "PartiesTypes.generated.h"

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPPartyId : public UObject
{
    GENERATED_BODY()

public:
    TSharedPtr<const FOnlinePartyId> PartyId;

    UFUNCTION(BlueprintCallable)
    FString ToString()
    {
        if (PartyId == nullptr)
        {
            return TEXT("");
        }

        return PartyId->ToString();
    }
};

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPPartyMemberId : public UObject
{
    GENERATED_BODY()

public:
    FOnlinePartyMemberConstPtr PartyMember;

    UFUNCTION(BlueprintCallable)
    FString ToString()
    {
        if (PartyMember == nullptr)
        {
            return TEXT("");
        }

        return PartyMember->GetUserId()->ToString();
    }
};

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPPartyInvite : public UObject
{
    GENERATED_BODY()

public:
    IOnlinePartyJoinInfoConstPtr PartyInvite;

    UFUNCTION(BlueprintCallable)
    FString ToString()
    {
        if (PartyInvite == nullptr)
        {
            return TEXT("");
        }

        return PartyInvite->GetPartyId()->ToString();
    }
};