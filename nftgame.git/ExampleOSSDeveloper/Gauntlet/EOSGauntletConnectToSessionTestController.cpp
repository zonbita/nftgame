// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletConnectToSessionTestController.h"

#include "Containers/Ticker.h"
#include "EOSGauntletTestLog.h"
#include "GameFramework/GameModeBase.h"
#include "Misc/Parse.h"
#include "OnlineSubsystemUtils.h"

void UEOSGauntletConnectToSessionTestController::OnLoginComplete(
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

    UE_LOG(LogEOSGauntlet, Verbose, TEXT("Successfully logged in, beginning search for session..."));

    this->StartSearch();
}

void UEOSGauntletConnectToSessionTestController::StartSearch()
{
    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
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

    FString GauntletSessionGUID;
    if (!FParse::Value(FCommandLine::Get(), TEXT("eosgauntletsessionguid="), GauntletSessionGUID, true))
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("no gauntlet session GUID set on command line!"));
        UGauntletTestController::EndTest(1);
        return;
    }

    TSharedRef<FOnlineSessionSearch> Search = MakeShared<FOnlineSessionSearch>();
    Search->QuerySettings.SearchParams.Empty();
    Search->QuerySettings.Set(FName(TEXT("GauntletSessionGUID")), GauntletSessionGUID, EOnlineComparisonOp::Equals);

    this->FindSessionsDelegateHandle =
        OSSSession->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsComplete::FDelegate::CreateUObject(
            this,
            &UEOSGauntletConnectToSessionTestController::HandleFindSessionsComplete,
            Search));
    if (!OSSSession->FindSessions(0, Search))
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("Unable to start FindSessions call"));
        UGauntletTestController::EndTest(1);
        return;
    }
}

void UEOSGauntletConnectToSessionTestController::HandleFindSessionsComplete(
    bool bWasSuccessful,
    TSharedRef<FOnlineSessionSearch> Search)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("search failed to run"));
        UGauntletTestController::EndTest(1);
        return;
    }

    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
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

    OSSSession->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionsDelegateHandle);
    this->FindSessionsDelegateHandle.Reset();

    if (Search->SearchResults.Num() == 0)
    {
        // Search again...
        UE_LOG(LogEOSGauntlet, Verbose, TEXT("Failed to find sessions, trying again..."));
        this->StartSearch();
        return;
    }
    else
    {
        checkf(
            Search->SearchResults.Num() == 1 || !Search->SearchResults[0].IsValid(),
            TEXT("Unexpected number of search results for search by EOS gauntlet GUID"));

        FString ConnectInfo;
        if (OSSSession->GetResolvedConnectString(Search->SearchResults[0], NAME_GamePort, ConnectInfo))
        {
            this->JoinSession(Search->SearchResults[0], ConnectInfo);
        }
        else
        {
            checkf(false, TEXT("Unable to get resolved connection string for EOS gauntlet session!"));
        }
    }
}

void UEOSGauntletConnectToSessionTestController::JoinSession(
    const FOnlineSessionSearchResult &SearchResult,
    const FString &ConnectionString)
{
    IOnlineSubsystem *OnlineSubsystem = Online::GetSubsystem(this->GetWorld(), FName(TEXT("RedpointEOS")));
    if (OnlineSubsystem == nullptr)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS online subsystem not available!"));
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

    IOnlineIdentityPtr OSSIdentity = OnlineSubsystem->GetIdentityInterface();
    if (!OSSIdentity.IsValid())
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("EOS identity interface not available!"));
        UGauntletTestController::EndTest(1);
        return;
    }

    this->JoinSessionDelegateHandle =
        OSSSession->AddOnJoinSessionCompleteDelegate_Handle(FOnJoinSessionCompleteDelegate::CreateUObject(
            this,
            &UEOSGauntletConnectToSessionTestController::HandleJoinSessionComplete,
            ConnectionString));

    if (!OSSSession->JoinSession(*OSSIdentity->GetUniquePlayerId(0), FName(TEXT("EOSGauntletSession")), SearchResult))
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("unable to join session"));
        UGauntletTestController::EndTest(1);
        return;
    }
}

void UEOSGauntletConnectToSessionTestController::HandleJoinSessionComplete(
    FName SessionName,
    EOnJoinSessionCompleteResult::Type JoinResult,
    FString ConnectionString)
{
    if (SessionName.IsEqual(FName(TEXT("EOSGauntletSession"))))
    {
        if (JoinResult != EOnJoinSessionCompleteResult::Success)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("join session failed to run"));
            UGauntletTestController::EndTest(1);
            return;
        }

        this->ConnectionStringForDisconnectReconnect = ConnectionString;
        UE_LOG(
            LogEOSGauntlet,
            GauntletLogLevel,
            TEXT("Connecting to server: %s"),
            *this->ConnectionStringForDisconnectReconnect);

        // Now try to connect to the server.
        FURL NewURL(nullptr, *this->ConnectionStringForDisconnectReconnect, ETravelType::TRAVEL_Absolute);
        FString BrowseError;
        if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
            EBrowseReturnVal::Failure)
        {
            UE_LOG(LogEOSGauntlet, Error, TEXT("failed to browse to session"));
            UGauntletTestController::EndTest(1);
            return;
        }
    }
}

void UEOSGauntletConnectToSessionTestController::OnNetworkFailure(
    UWorld *World,
    UNetDriver *NetDriver,
    ENetworkFailure::Type ErrorType,
    const FString &Error)
{
    if (Error == "Host closed the connection." || Error == "Your connection to the host has been lost.")
    {
        // This is fine, we expect to see these errors when the server shuts down during test end.
        return;
    }

    UE_LOG(LogEOSGauntlet, Error, TEXT("Got network failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletConnectToSessionTestController::OnTravelFailure(
    UWorld *World,
    ETravelFailure::Type ErrorType,
    const FString &Error)
{
    UE_LOG(LogEOSGauntlet, Error, TEXT("Got travel failure: %s"), *Error);
    UGauntletTestController::EndTest(1);
}

void UEOSGauntletConnectToSessionTestController::OnInit()
{
    GEngine->OnNetworkFailure().AddUObject(this, &UEOSGauntletConnectToSessionTestController::OnNetworkFailure);
    GEngine->OnTravelFailure().AddUObject(this, &UEOSGauntletConnectToSessionTestController::OnTravelFailure);

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
            &UEOSGauntletConnectToSessionTestController::OnLoginComplete,
            OSSIdentity));
    if (!OSSIdentity->Login(0, Credentials))
    {
        OSSIdentity->ClearOnLoginCompleteDelegate_Handle(0, this->OnLoginCompleteDelegate);
        UE_LOG(LogEOSGauntlet, Error, TEXT("Failed to start EOS login process!"));
        UGauntletTestController::EndTest(1);
        return;
    }
}

void UEOSGauntletConnectToSessionTestController::OnTick(float TimeDelta)
{
    if (!bHasDoneMultiplayerSetup && this->GetCurrentMap().Contains("MultiplayerMap"))
    {
        bHasDoneMultiplayerSetup = true;

        float SecondsUntilDisconnect = FMath::RandRange(1, 3) * 10.0f;
        UE_LOG(
            LogEOSGauntlet,
            GauntletLogLevel,
            TEXT("Connected to server, scheduling disconnect in %fs."),
            SecondsUntilDisconnect);

        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UEOSGauntletConnectToSessionTestController::DisconnectFromServer),
            SecondsUntilDisconnect);
    }

    if (bHasDoneMultiplayerSetup && this->GetCurrentMap().Contains("EmptyMap"))
    {
        this->bHasDoneMultiplayerSetup = false;

        UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Disconnected from server, scheduling reconnect in 100ms."));

        FTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UEOSGauntletConnectToSessionTestController::ReconnectToServer),
            0.1f);
    }
}

bool UEOSGauntletConnectToSessionTestController::DisconnectFromServer(float TimeDelta)
{
    UE_LOG(LogEOSGauntlet, GauntletLogLevel, TEXT("Disconnecting from server..."));

    FURL NewURL(
        nullptr,
        *FString::Printf(
            TEXT("/Game/ExampleOSS/Common/EmptyMap?game=%s"),
            *AGameModeBase::StaticClass()->GetPathName()),
        ETravelType::TRAVEL_Absolute);
    FString BrowseError;
    if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
        EBrowseReturnVal::Failure)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("Unable to call Browse() during disconnect"));
        UGauntletTestController::EndTest(1);
    }

    return false;
}

bool UEOSGauntletConnectToSessionTestController::ReconnectToServer(float TimeDelta)
{
    UE_LOG(
        LogEOSGauntlet,
        GauntletLogLevel,
        TEXT("Reconnecting to server: %s"),
        *this->ConnectionStringForDisconnectReconnect);

    FURL NewURL(nullptr, *this->ConnectionStringForDisconnectReconnect, ETravelType::TRAVEL_Absolute);
    FString BrowseError;
    if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
        EBrowseReturnVal::Failure)
    {
        UE_LOG(LogEOSGauntlet, Error, TEXT("Unable to call Browse() during reconnect"));
        UGauntletTestController::EndTest(1);
    }

    return false;
}