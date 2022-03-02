// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"

#include "../ExampleCPPConfigLibrary.h"
#include "Engine/LocalPlayer.h"
#include "Engine/NetConnection.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UExampleCPPSubsystem::SeamlessTravel(const UObject *WorldContextObject)
{
    AGameModeBase *GameModeBase = WorldContextObject->GetWorld()->GetAuthGameMode<AGameModeBase>();
    if (GameModeBase == nullptr)
    {
        return;
    }

    GameModeBase->bUseSeamlessTravel = true;
    if (WorldContextObject->GetWorld()->GetMapName().Contains("MultiplayerMap2"))
    {
        WorldContextObject->GetWorld()->ServerTravel(TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap"), true);
    }
    else
    {
        WorldContextObject->GetWorld()->ServerTravel(TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap2"), true);
    }
}

void UExampleCPPSubsystem::NonSeamlessTravel(const UObject *WorldContextObject)
{
    AGameModeBase *GameModeBase = WorldContextObject->GetWorld()->GetAuthGameMode<AGameModeBase>();
    if (GameModeBase == nullptr)
    {
        return;
    }

    GameModeBase->bUseSeamlessTravel = false;
    if (WorldContextObject->GetWorld()->GetMapName().Contains("MultiplayerMap2"))
    {
        WorldContextObject->GetWorld()->ServerTravel(TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap"), true);
    }
    else
    {
        WorldContextObject->GetWorld()->ServerTravel(TEXT("/Game/ExampleOSS/Common/Multiplayer/MultiplayerMap2"), true);
    }
}

void UExampleCPPSubsystem::BeginRecordingReplay(AGameModeBase *GameMode)
{
    const ENetMode NetMode = GameMode->GetWorld()->GetNetMode();
    const bool bIsServer = (NetMode == ENetMode::NM_DedicatedServer || NetMode == ENetMode::NM_ListenServer);
    if (GameMode->GetWorld()->IsGameWorld() && bIsServer)
    {
        // start our recording
        GameMode->GetGameInstance()->StartRecordingReplay(FString("Replay"), FString("Replay"));
    }
}