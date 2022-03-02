// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletLoginWithNativeCredentialsTestController.h"

#include "EOSGauntletTestLog.h"
#include "OnlineSubsystemRedpointEOS/Public/EOSSubsystem.h"
#include "OnlineSubsystemUtils.h"

#define TEST_NATIVE_AUTHENTICATION TEXT("OnlineSubsystemEOS.Gauntlet.NativeAuthenticationWorks")
#define TEST_CREDENTIAL_CAN_BE_REFRESHED TEXT("OnlineSubsystemEOS.Gauntlet.CredentialCanBeRefreshed")

void UEOSGauntletLoginWithNativeCredentialsTestController::OnLoginComplete(
    int32 LocalUserNum,
    bool bWasSuccessful,
    const FUniqueNetId &UserId,
    const FString &Error,
    IOnlineIdentityPtr OSSIdentity)
{
    OSSIdentity->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->OnLoginCompleteDelegate);

    if (!bWasSuccessful)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS failed to login! See above logs. Error: %s"), *Error);
        ::EmitTestFail(TEST_NATIVE_AUTHENTICATION);
        UGauntletTestController::EndTest(1);
        return;
    }

    ::EmitTestPass(TEST_NATIVE_AUTHENTICATION);

    ::EmitTestStart(TEST_CREDENTIAL_CAN_BE_REFRESHED);

    UEOSSubsystem *GlobalSubsystem = UEOSSubsystem::GetSubsystem(this->GetWorld());
    if (!IsValid(GlobalSubsystem))
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("UEOSSubsystem not available!"));
        ::EmitTestFail(TEST_CREDENTIAL_CAN_BE_REFRESHED);
        UGauntletTestController::EndTest(1);
        return;
    }

    GlobalSubsystem->OnCredentialRefreshComplete.AddLambda([](bool bWasSuccessful) {
        if (!bWasSuccessful)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("Credential refresh failed! See above logs."));
            ::EmitTestFail(TEST_CREDENTIAL_CAN_BE_REFRESHED);
            UGauntletTestController::EndTest(1);
            return;
        }

        ::EmitTestPass(TEST_CREDENTIAL_CAN_BE_REFRESHED);
        UGauntletTestController::EndTest(0);
    });

    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
        ::EmitTestFail(TEST_CREDENTIAL_CAN_BE_REFRESHED);
        UGauntletTestController::EndTest(1);
        return;
    }
    OnlineSubsystem->Exec(this->GetWorld(), TEXT("IDENTITY SIMULATEEXPIRY 0"), *GLog);
}

void UEOSGauntletLoginWithNativeCredentialsTestController::OnInit()
{
    ::EmitTestStart(TEST_NATIVE_AUTHENTICATION);

    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
        ::EmitTestFail(TEST_NATIVE_AUTHENTICATION);
        UGauntletTestController::EndTest(1);
        return;
    }

    IOnlineIdentityPtr OSSIdentity = OnlineSubsystem->GetIdentityInterface();
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS identity interface not available!"));
        ::EmitTestFail(TEST_NATIVE_AUTHENTICATION);
        UGauntletTestController::EndTest(1);
        return;
    }

    this->OnLoginCompleteDelegate = OSSIdentity->AddOnLoginCompleteDelegate_Handle(
        0,
        FOnLoginCompleteDelegate::CreateUObject(
            this,
            &UEOSGauntletLoginWithNativeCredentialsTestController::OnLoginComplete,
            OSSIdentity));
    if (!OSSIdentity->AutoLogin(0))
    {
        OSSIdentity->ClearOnLoginCompleteDelegate_Handle(0, this->OnLoginCompleteDelegate);
        UE_LOG(LogEOSGauntlet, Error, TEXT("Failed to start EOS login process!"));
        ::EmitTestFail(TEST_NATIVE_AUTHENTICATION);
        UGauntletTestController::EndTest(1);
        return;
    }
}

void UEOSGauntletLoginWithNativeCredentialsTestController::OnTick(float DeltaTime)
{
}