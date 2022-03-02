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

#include "LeaderboardsTypes.generated.h"

USTRUCT(BlueprintType)
struct EXAMPLEOSS_API FExampleCPPLeaderboardEntry
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Rank;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PlayerId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    FString PlayerNickname;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int Score;
};