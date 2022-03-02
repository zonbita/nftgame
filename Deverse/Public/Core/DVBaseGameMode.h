// Copyright Cosugames, LLC 2021

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "CoreMinimal.h"
#include "DVBaseController.h"
#include "DVSpaceShip.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "ExampleOSS/Interfaces/OnlineVoiceAdminInterface.h"
#include "DVBaseGameMode.generated.h"


UCLASS()
class DEVERSE_API ADVBaseGameMode : public AGameMode, public IDVEventInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<AActor> SpaceShipClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		TSubclassOf<AActor> FrameClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		TSubclassOf<AActor> PortalClass;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	int SpaceArea = 10000000;
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int SpaceRange = 100000;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	int32 MaxDebugShip = 2;
#endif
#if WITH_EDITOR
	void Debug();
#endif
	void SpawnShip(APlayerController* controller);

	void Calculator();
	
	bool RequestRespawn(AController* InController, const FVector SpawnLocation, const FRotator SpawnRotation);

	// ~Gamemode interface
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	void RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform) override;
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation) override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	// ~IDVEvent interface
	void EventRaceUpdated_Implementation(AController* InController);
	void EventPlayerFirstJoin_Implementation(AController* NewController);
private:
	int Number = 1;
	TArray<FVector> SSLoc;
	TArray<int> SSInt;

	void IssuedCredentialsForPlayer(
	const FOnlineError& Result,
	const FUniqueNetId& LocalUserId,
	const TArray<FVoiceAdminChannelCredentials>& Credentials,
	FString InChannelName,
	ADVBaseController* NewPlayer);
	
};
