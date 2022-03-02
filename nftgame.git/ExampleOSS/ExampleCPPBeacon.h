// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "OnlineBeaconClient.h"
#include "OnlineBeaconHost.h"
#include "OnlineBeaconHostObject.h"
#include "UObject/ObjectMacros.h"

#include "ExampleCPPBeacon.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FExampleCPPSubsystemBeaconPingComplete, bool, WasSuccessful, FString, Info);

UCLASS(transient, notplaceable)
class EXAMPLEOSS_API AExampleCPPBeaconHost : public AOnlineBeaconHostObject
{
    GENERATED_UCLASS_BODY()

public:
    virtual AOnlineBeaconClient *SpawnBeaconActor(class UNetConnection *ClientConnection) override;
    virtual void OnClientConnected(class AOnlineBeaconClient *NewClientActor, class UNetConnection *ClientConnection)
        override;

    virtual bool Init();
};

UCLASS(transient, notplaceable)
class EXAMPLEOSS_API AExampleCPPBeaconClient : public AOnlineBeaconClient
{
    GENERATED_BODY()

public:
    virtual void OnFailure() override;

    UPROPERTY()
    FExampleCPPSubsystemBeaconPingComplete OnComplete;

    UFUNCTION(client, reliable)
    virtual void ClientSendPing();

    UFUNCTION(server, reliable)
    virtual void ServerRespondToPing();

    UFUNCTION(client, reliable)
    virtual void ClientReceivePong();
};

UCLASS(BlueprintType)
class EXAMPLEOSS_API AExampleCPPBeaconHostActor : public AActor
{
    GENERATED_BODY()

private:
    UPROPERTY()
    AOnlineBeaconHost *Beacon;

public:
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
};