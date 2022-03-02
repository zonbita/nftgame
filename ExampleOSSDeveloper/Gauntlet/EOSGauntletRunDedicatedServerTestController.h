// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "GauntletTestController.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "EOSGauntletRunDedicatedServerTestController.generated.h"

UCLASS(transient)
class UEOSGauntletRunDedicatedServerTestController : public UGauntletTestController
{
    GENERATED_BODY()

private:
    bool OnSeamlessServerTravel(float DeltaTime);
    bool OnTestTimeRunOut(float DeltaTime);

    bool bHasDoneMapLoadFromEmpty;
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
    UEOSGauntletRunDedicatedServerTestController() : UGauntletTestController(FObjectInitializer())
    {
        this->bHasDoneMultiplayerSetup = false;
    };
    virtual ~UEOSGauntletRunDedicatedServerTestController(){};

protected:
    virtual void OnInit() override;
    virtual void OnTick(float TimeDelta) override;
};