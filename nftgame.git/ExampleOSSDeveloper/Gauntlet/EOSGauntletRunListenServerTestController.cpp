// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletRunListenServerTestController.h"

#include "Containers/Ticker.h"
#include "EOSGauntletTestLog.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/Parse.h"
#include "OnlineSubsystemUtils.h"

void UEOSGauntletRunListenServerTestController::OnLoginComplete(
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
        UGauntletTestController::EndTest(1);
        return;
    }

    FString BrowseError = TEXT("");
    FURL URL(
        nullptr,
        *FString::Printf(
            TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap?listen?netmode=forcep2p?game=%s"),
            *AGameModeBase::StaticClass()->GetPathName()),
        ETravelType::TRAVEL_Absolute);
    if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), URL, BrowseError) ==
        EBrowseReturnVal::Failure)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("Failed to start listen server: %s"), *BrowseError);
        UGauntletTestController::EndTest(1);
    }
}

void UEOSGauntletRunListenServerTestController::OnNetworkFailure(
    UWorld *World,
    UNetDriver *NetDriver,
    ENetworkFailure::Type ErrorType,
    const FString &Error)
{
    UE_LOG(LogEOSGauntlet, Error, TEXT("Got network failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletRunListenServerTestController::OnTravelFailure(
    UWorld *World,
    ETravelFailure::Type ErrorType,
    const FString &Error)
{
    UE_LOG(LogEOSGauntlet, Error, TEXT("Got travel failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletRunListenServerTestController::OnInit()
{
    GEngine->OnNetworkFailure().AddUObject(this, &UEOSGauntletRunListenServerTestController::OnNetworkFailure);
    GEngine->OnTravelFailure().AddUObject(this, &UEOSGauntletRunListenServerTestController::OnTravelFailure);

    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
        UGauntletTestController::EndTest(1);
        return;
    }

    IOnlineIdentityPtr OSSIdentity = OnlineSubsystem->GetIdentityInterface();
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS identity interface not available!"));
        UGauntletTestController::EndTest(1);
        return;
    }

    int32 GauntletRoleID;
    if (!FParse::Value(FCommandLine::Get(), TEXT("eosroleid="), GauntletRoleID))
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("no EOS role ID provided"));
        UGauntletTestController::EndTest(1);
        return;
    }

    FOnlineAccountCredentials Credentials;
    Credentials.Type = TEXT("AUTOMATED_TESTING");
    Credentials.Id = TEXT("CreateOnDemand:Gauntlet.P2PNetworking");
    Credentials.Token = FString::Printf(TEXT("%d"), GauntletRoleID);

    this->OnLoginCompleteDelegate = OSSIdentity->AddOnLoginCompleteDelegate_Handle(
        0,
        FOnLoginCompleteDelegate::CreateUObject(
            this,
            &UEOSGauntletRunListenServerTestController::OnLoginComplete,
            OSSIdentity));
    if (!OSSIdentity->Login(0, Credentials))
    {
        OSSIdentity->ClearOnLoginCompleteDelegate_Handle(0, this->OnLoginCompleteDelegate);
        UE_LOG(LogEOSGauntlet, Error, TEXT("Failed to start EOS login process!"));
        UGauntletTestController::EndTest(1);
        return;
    }
}

bool UEOSGauntletRunListenServerTestController::OnSeamlessServerTravel(float DeltaTime)
{
    AGameModeBase *GameModeBase = this->GetWorld()->GetAuthGameMode<AGameModeBase>();
    if (GameModeBase == nullptr)
    {
        return true;
    }

    GameModeBase->bUseSeamlessTravel = true;

    FURL URL(
        nullptr,
        *FString::Printf(
            TEXT("/Game/ExampleOSS/Common/Multiplayer/%s?game=%s"),
            this->GetWorld()->GetMapName().Contains("MultiplayerMap2") ? TEXT("MultiplayerMap")
                                                                       : TEXT("MultiplayerMap2"),
            *AGameModeBase::StaticClass()->GetPathName()),
        ETravelType::TRAVEL_Absolute);

    UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Requesting seamless travel to %s"), *URL.ToString());
    this->GetWorld()->ServerTravel(URL.ToString(), true);

    return true;
}

bool UEOSGauntletRunListenServerTestController::OnTestTimeRunOut(float DeltaTime)
{
    UGauntletTestController::EndTest(0);
    return false;
}

void UEOSGauntletRunListenServerTestController::OnTick(float TimeDelta)
{
    if (!bHasDoneMultiplayerSetup && this->GetCurrentMap().Contains("MultiplayerMap"))
    {
        bHasDoneMultiplayerSetup = true;

        UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Creating session for listen server"));

        IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
        if (OnlineSubsystem == nullptr)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
            UGauntletTestController::EndTest(1);
            return;
        }

        IOnlineIdentityPtr OSSIdentity = OnlineSubsystem->GetIdentityInterface();
        if (!OSSIdentity.IsValid())
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("EOS identity interface not available!"));
            UGauntletTestController::EndTest(1);
            return;
        }

        IOnlineSessionPtr OSSSession = OnlineSubsystem->GetSessionInterface();
        if (!OSSSession.IsValid())
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("EOS session interface not available!"));
            UGauntletTestController::EndTest(1);
            return;
        }

        this->CreateSessionDelegateHandle =
            OSSSession->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::CreateUObject(
                this,
                &UEOSGauntletRunListenServerTestController::HandleCreateSessionComplete));

        FString GauntletSessionGUID;
        if (!FParse::Value(FCommandLine::Get(), TEXT("eosgauntletsessionguid="), GauntletSessionGUID, true))
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("no gauntlet session GUID set on command line!"));
            UGauntletTestController::EndTest(1);
            return;
        }

        TSharedRef<FOnlineSessionSettings> GauntletSessionSettings = MakeShared<FOnlineSessionSettings>();
        GauntletSessionSettings->NumPublicConnections = 32;
        GauntletSessionSettings->bShouldAdvertise = true;
        GauntletSessionSettings->bUsesPresence = false;
        GauntletSessionSettings->Settings.Empty();
        GauntletSessionSettings->Settings.Add(
            FName(TEXT("GauntletSessionGUID")),
            FOnlineSessionSetting(GauntletSessionGUID, EOnlineDataAdvertisementType::ViaOnlineService));

        if (!OSSSession->CreateSession(
                *OSSIdentity->GetUniquePlayerId(0),
                FName(TEXT("EOSGauntletSession")),
                *GauntletSessionSettings))
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("Unable to start session creation call"));
            UGauntletTestController::EndTest(1);
            return;
        }
    }
}

void UEOSGauntletRunListenServerTestController::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (SessionName.IsEqual(FName(TEXT("EOSGauntletSession"))))
    {
        if (!bWasSuccessful)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("Unable to create session!"));
            UGauntletTestController::EndTest(1);
            return;
        }

        UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Scheduling map change every twenty seconds..."));
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UEOSGauntletRunListenServerTestController::OnSeamlessServerTravel),
            20.0f);

        UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Scheduling two minute run time for listen server..."));
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UEOSGauntletRunListenServerTestController::OnTestTimeRunOut),
            120.0f);
    }
}