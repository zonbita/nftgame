// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "GauntletTestController.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "EOSGauntletLoginWithNativeCredentialsTestController.generated.h"

UCLASS(transient)
class UEOSGauntletLoginWithNativeCredentialsTestController : public UGauntletTestController
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

protected:
    virtual void OnInit() override;
    virtual void OnTick(float DeltaTime) override;
};