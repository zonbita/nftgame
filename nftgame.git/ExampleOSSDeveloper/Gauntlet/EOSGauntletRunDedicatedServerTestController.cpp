// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletRunDedicatedServerTestController.h"

#include "Containers/Ticker.h"
#include "EOSGauntletTestLog.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/Parse.h"
#include "OnlineSubsystemUtils.h"

void UEOSGauntletRunDedicatedServerTestController::OnNetworkFailure(
    UWorld *World,
    UNetDriver *NetDriver,
    ENetworkFailure::Type ErrorType,
    const FString &Error)
{
    UE_LOG(LogEOSGauntlet, Error, TEXT("Got network failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletRunDedicatedServerTestController::OnTravelFailure(
    UWorld *World,
    ETravelFailure::Type ErrorType,
    const FString &Error)
{
    UE_LOG(LogEOSGauntlet, Error, TEXT("Got travel failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletRunDedicatedServerTestController::OnInit()
{
    GEngine->OnNetworkFailure().AddUObject(this, &UEOSGauntletRunDedicatedServerTestController::OnNetworkFailure);
    GEngine->OnTravelFailure().AddUObject(this, &UEOSGauntletRunDedicatedServerTestController::OnTravelFailure);
}

bool UEOSGauntletRunDedicatedServerTestController::OnSeamlessServerTravel(float DeltaTime)
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

bool UEOSGauntletRunDedicatedServerTestController::OnTestTimeRunOut(float DeltaTime)
{
    UGauntletTestController::EndTest(0);
    return false;
}

void UEOSGauntletRunDedicatedServerTestController::OnTick(float TimeDelta)
{
    if (!bHasDoneMapLoadFromEmpty && this->GetCurrentMap().Contains("EmptyMap"))
    {
        bHasDoneMapLoadFromEmpty = true;

        FString BrowseError = TEXT("");
        FURL URL(
            nullptr,
            *FString::Printf(
                TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap?listen?netmode=forceip?game=%s"),
                *AGameModeBase::StaticClass()->GetPathName()),
            ETravelType::TRAVEL_Absolute);
        if (GEngine->Browse((FWorldContext &)GEngine->GetWorldContexts()[0], URL, BrowseError) ==
            EBrowseReturnVal::Failure)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("Failed to start dedicated server: %s"), *BrowseError);
            UGauntletTestController::EndTest(1);
        }
    }
    else if (!bHasDoneMultiplayerSetup && this->GetCurrentMap().Contains("MultiplayerMap"))
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
                &UEOSGauntletRunDedicatedServerTestController::HandleCreateSessionComplete));

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

void UEOSGauntletRunDedicatedServerTestController::HandleCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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
            FTickerDelegate::CreateUObject(this, &UEOSGauntletRunDedicatedServerTestController::OnSeamlessServerTravel),
            20.0f);

        UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Scheduling two minute run time for dedicated server..."));
        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UEOSGauntletRunDedicatedServerTestController::OnTestTimeRunOut),
            120.0f);
    }
}