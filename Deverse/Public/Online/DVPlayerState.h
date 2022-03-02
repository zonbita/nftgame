// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Deverse/Deverse.h"
#include "Core/DVEventInterface.h"
#include "DVPlayerState.generated.h"

/**
 * 
 */

UCLASS()
class DEVERSE_API ADVPlayerState : public APlayerState, public IDVEventInterface
{
	GENERATED_BODY()

public:
	ADVPlayerState();

	virtual void BeginPlay() override;

	bool bIsWalletInitialized;

	UFUNCTION(BlueprintPure, Category = "Deverse|Wallet")
		FWallet GetWalletData() { return WalletData; }

	// ~IDVEventInterface
	void UpdatePlayerName_Implementation(const FString& InNewName);

	void SetCurrentRace_Implementation(const FString& RaceId);
	
	void GetSpawnClass_Implementation(TSubclassOf<AActor>& SpawnClass);

	void SetCurrentSkin_Implementation(const FString& SkinId);
	//////////////////////

	virtual void SetPlayerName(const FString& S);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetPlayerName(const FString& S);
	
	virtual void OnRep_PlayerName() override;

	void SetCurrentRace(FString NewRace);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRace(const FString& NewRace);

	void SetCurrentSkin(const FString& NewSkin);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSkin(const FString& NewSkin);

	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(Replicated)
		FString CurrentPartyId;
protected:
	FWallet WalletData;

	UPROPERTY(Replicated)
		FString CurrentRace;

	UPROPERTY(Replicated)
		FString CurrentSkin;
};
