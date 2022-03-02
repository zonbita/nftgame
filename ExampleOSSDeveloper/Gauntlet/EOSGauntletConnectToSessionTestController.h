// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "GauntletTestController.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "EOSGauntletConnectToSessionTestController.generated.h"

UCLASS(transient)
class UEOSGauntletConnectToSessionTestController : public UGauntletTestController
{
    GENERATED_BODY()

private:
    FDelegateHandle OnLoginCompleteDelegate;
    void OnLoginComplete(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &Error,
        IOnlineIdentityPtr OSSIdentity);

    void StartSearch();

    FDelegateHandle FindSessionsDelegateHandle;
    void HandleFindSessionsComplete(bool bWasSuccessful, TSharedRef<FOnlineSessionSearch> Search);

    void JoinSession(const FOnlineSessionSearchResult &SearchResult, const FString &ConnectionString);

    FDelegateHandle JoinSessionDelegateHandle;
    void HandleJoinSessionComplete(
        FName SessionName,
        EOnJoinSessionCompleteResult::Type JoinResult,
        FString ConnectionString);

    bool bHasDoneMultiplayerSetup;

    void OnNetworkFailure(
        UWorld *World,
        class UNetDriver *NetDriver,
        ENetworkFailure::Type ErrorType,
        const FString &Error);
    void OnTravelFailure(UWorld *World, ETravelFailure::Type ErrorType, const FString &Error);

    FString ConnectionStringForDisconnectReconnect;

    bool DisconnectFromServer(float TimeDelta);
    bool ReconnectToServer(float TimeDelta);

public:
    UEOSGauntletConnectToSessionTestController() : UGauntletTestController(FObjectInitializer())
    {
        this->bHasDoneMultiplayerSetup = false;
    };
    virtual ~UEOSGauntletConnectToSessionTestController(){};

protected:
    virtual void OnInit() override;
    virtual void OnTick(float TimeDelta) override;
};