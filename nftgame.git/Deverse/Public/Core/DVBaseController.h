// Copyright Cosugames, LLC 2021

#pragma once

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "VoiceChat.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Core/DVEventInterface.h"
#include "Core/DVWalletInterface.h"
#include "Deverse/Deverse.h"
#include "ExampleOSS/VoiceChat/VoiceTypes.h"
#include "DVBaseController.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnCreatePartyDone, bool, bResult);

UCLASS()
class DEVERSE_API ADVBaseController : public APlayerController, public IDVEventInterface, public IDVWalletInterface
{
	GENERATED_BODY()

public:
	ADVBaseController();

	//IVoiceChat* VoiceChat;

	IVoiceChatUser* VoiceChatUser;
	
	UFUNCTION(Client, Reliable)
	void JoinVoiceChannel(const FString& InChannelName, const FString& InChannelCredentials);
	
	void OnLoginComplete(const FString& PlayerName, const FVoiceChatResult& Result);

	UFUNCTION(BlueprintPure)
	bool IsPlayerTalking();

	// Beginning integrating EOS back into our code

	UFUNCTION(BlueprintCallable)
	void CreateParty(const UObject* WorldContextObject, int32 PartyTypeId, FOnCreatePartyDone OnDone);

	void HandleCreatePartyComplete(
		const FUniqueNetId& LocalUserId,
		const TSharedPtr<const FOnlinePartyId>& PartyId,
		const ECreatePartyCompletionResult Result,
		const UObject* WorldContextObject,
		FOnCreatePartyDone OnDone);



	///////////////////////////////////////////

	//UFUNCTION()
	//void OnCPPLoginDone(const FString& PlayerName, const FVoiceChatResultCPP& VoiceChatLogin);

	UPROPERTY(BlueprintReadWrite)
	class UUserWidget* PersistentHUD;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void ToggleChat();

	UFUNCTION(BlueprintCallable)
		void ToggleMenu();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		class UDVNftManagerComponent* NftManagerComp;

	// ~IDVWalletInterface
	void FinishUpdateBalance_Implementation();
	void OnNftSelect_Implementation(FNft InNft);
	void RequestSpawnNft_Implementation(FNft InNft);
	void RequestSetNftAvatar_Implementation(FLiteNft InNft);
	void OnNftSpawn_Implementation(AActor* SpawnedActor);
	void UpdateAvatar_Implementation(bool bHasAvatar);
	void RequestChangeRace_Implementation(FRace InRace);
	void GetSpawnClass_Implementation(TSubclassOf<AActor>& SpawnClass);
	void FinishDownloadNft_Implementation(FNft InNft);
	void OnFinishQueryNfts_Implementation(const TArray<FNft>& Nfts);
	// ~IDVEventInterface
	void EventSpaceshipSpawned_Implementation(AActor* SpawnedShip);
	void RequestRespawn_Implementation(const FVector InSpawnLocation, const FRotator InSpawnRotation);
	void SetDestination_Implementation(const FString& TravelURL, const FString& TravelId);
	/// <summary>
	/// ///////////////////////////////////////
	/// </summary>
	
	void OnControlledPawnClick();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestChangeRace(FRace InRace);

	void RequestRespawn(const FVector InSpawnLocation, const FRotator InSpawnRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestRespawn(const FVector InSpawnLocation, const FRotator InSpawnRotation);

	UPROPERTY(BlueprintReadWrite, Category = "Assets")
	TArray<class ADVGallerysPlace*> OwningGalleries;

	// Basic gameplay
	void EventChat_Implementation(const FString& InMessage, EChatGroup ChatGroup);

	void Chat(const FString& InMessage, EChatGroup ChatGroup);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChat(const FString& InMessage, EChatGroup ChatGroup);

	UFUNCTION(Client, Reliable)
	void ClientChatUpdate(const FString& SenderId, const FString& InMessage, EChatGroup ChatGroup);

	FString GetPlayerName();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Toggle Menu"))
	void K2_ToggleMenu(bool IsActive);

	UFUNCTION(BlueprintPure)
		bool IsMenuActive() { return bMenuActive; }
	
#if WITH_EDITOR
	UFUNCTION(Exec)
	void RequestDestroySpaceship();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDestroySpaceship();
#endif
	// ~ Controller interface
	void OnRep_PlayerState() override;
	virtual void InitPlayerState() override;
	virtual void BeginDestroy() override;
protected:
	UPROPERTY(BlueprintReadOnly)
	class AActor* SpaceShip;

private:
	bool bChatActive;

	bool bMenuActive;
};
