// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "GauntletTestController.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "EOSGauntletRunListenServerTestController.generated.h"

UCLASS(transient)
class UEOSGauntletRunListenServerTestController : public UGauntletTestController
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

    bool OnSeamlessServerTravel(float DeltaTime);
    bool OnTestTimeRunOut(float DeltaTime);

    bool bHasDoneMultiplayerSetup;

    FDelegateHandle CreateSessionDelegateHandle;
    void HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful);

    void OnNetworkFailure(
        UWorld *World,
        class UNetDriver *NetDriver,
        ENetworkFailure::Type ErrorType,
        const FString &Error);
    void OnTravelFailure(UWorld *World, ETravelFailure::Type ErrorType, const FString &Error);

public:
    UEOSGauntletRunListenServerTestController() : UGauntletTestController(FObjectInitializer())
    {
        this->bHasDoneMultiplayerSetup = false;
    };
    virtual ~UEOSGauntletRunListenServerTestController(){};

protected:
    virtual void OnInit() override;
    virtual void OnTick(float TimeDelta) override;
};