// Copyright June Rhodes. MIT Licensed.

#include "ExampleCPPBeacon.h"

#include "CoreGlobals.h"
#include "ExampleCPPConfigLibrary.h"
#include "Misc/ConfigCacheIni.h"

AExampleCPPBeaconHost::AExampleCPPBeaconHost(const FObjectInitializer &ObjectInitializer) : Super(ObjectInitializer)
{
    ClientBeaconActorClass = AExampleCPPBeaconClient::StaticClass();
    BeaconTypeName = ClientBeaconActorClass->GetName();
}

bool AExampleCPPBeaconHost::Init()
{
    UE_LOG(LogTemp, Verbose, TEXT("Server beacon was initialized"));
    return true;
}

void AExampleCPPBeaconHost::OnClientConnected(AOnlineBeaconClient *NewClientActor, UNetConnection *ClientConnection)
{
    Super::OnClientConnected(NewClientActor, ClientConnection);

    AExampleCPPBeaconClient *BeaconClient = Cast<AExampleCPPBeaconClient>(NewClientActor);
    if (BeaconClient != NULL)
    {
        BeaconClient->ClientSendPing();
    }
}

AOnlineBeaconClient *AExampleCPPBeaconHost::SpawnBeaconActor(UNetConnection *ClientConnection)
{
    return Super::SpawnBeaconActor(ClientConnection);
}

void AExampleCPPBeaconClient::OnFailure()
{
    UE_LOG(LogTemp, Error, TEXT("Demo beacon connection failure; please check the logs."));

    Super::OnFailure();
}

void AExampleCPPBeaconClient::ClientSendPing_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Pinging server through beacon..."));
    this->ServerRespondToPing();
}

void AExampleCPPBeaconClient::ServerRespondToPing_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Server responding to beacon ping..."));
    this->ClientReceivePong();
}

void AExampleCPPBeaconClient::ClientReceivePong_Implementation()
{
    UE_LOG(LogTemp, Verbose, TEXT("Client got server response."));
    this->OnComplete.ExecuteIfBound(true, TEXT("Beacon ping successful"));
    this->DestroyBeacon();
}

void AExampleCPPBeaconHostActor::BeginPlay()
{
    if (!UExampleCPPConfigLibrary::GetAreBeaconsSupported(this))
    {
        // Beacons are not supported on the legacy networking stack.
        return;
    }

    if (Beacon)
    {
        Beacon->DestroyBeacon();
        Beacon = NULL;
    }

    AOnlineBeaconHost *BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());

    // If you explicitly set the port number here, then you should pass the port number
    // as e.g. FName(TEXT("12345")) as the port name when calling GetResolvedConnectString.
    //
    // If you *don't* set the port number here, and leave it as the default, then you should
    // pass NAME_BeaconPort as the port name when calling GetResolvedConnectString.
    BeaconHost->ListenPort = 12345;

    if (BeaconHost && BeaconHost->InitHost())
    {
        AOnlineBeaconHostObject *BeaconHostObject =
            GetWorld()->SpawnActor<AOnlineBeaconHostObject>(AExampleCPPBeaconHost::StaticClass());
        if (BeaconHostObject)
        {
            BeaconHost->RegisterHost(BeaconHostObject);
            Beacon = BeaconHost;
        }
        BeaconHost->PauseBeaconRequests(false);
    }
}

void AExampleCPPBeaconHostActor::BeginDestroy()
{
    if (Beacon)
    {
        Beacon->DestroyBeacon();
        Beacon = NULL;
    }

    Super::BeginDestroy();
}