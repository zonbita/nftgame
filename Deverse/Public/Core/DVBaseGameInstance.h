// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Deverse/Deverse.h"
#include "Core/DVEventInterface.h"
#include "Core/DVWalletInterface.h"
#include "DVBaseGameInstance.generated.h"

/**
 * 
 */

UCLASS(config=Game)
class DEVERSE_API UDVBaseGameInstance : public UGameInstance, public IDVWalletInterface
{
	GENERATED_BODY()

	virtual void Init() override;
public:
	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
	FWallet GetWallet() { return Wallet; }

	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
		TArray<FNft> GetNfts() { return Wallet.AvailableNfts; }

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Deverse|GameInstance")
	TArray<FString> RandomImageURLs;
#endif

	/** Use to travel between worlds */
	UPROPERTY(BlueprintReadOnly, Category = "Deverse|GameInstance")
		FString TravelURL;

	/** Use to travel between portals */
	UPROPERTY(BlueprintReadOnly, Category = "Deverse|GameInstance")
		FString TravelId;

	UPROPERTY(Config, BlueprintReadOnly)
		TArray<FVerseAddress> VerseAddresses;

	void UpdateNativeWalletBalance(float InValue);

	void UpdateTokenBalance(FToken InToken);

	/** 
		Update display texture of specific nft
	*/
	UFUNCTION(BlueprintCallable, Category = "Deverse|GameInstance")
	void UpdateNftDisplay(const FString InAddress, const FString TokenId, UObject* InDisplay);

	void UpdateNftHoldings(FNft InNft);

	void ClearBalance();

	void ClearNfts();

	void FinishUpdateBalance();

	void FinishQueryNfts(bool bUpdateToDatabase);

	UFUNCTION(BlueprintCallable,  Category = "Deverse|GameInstance")
	void SetProfileAvatar(FLiteNft InAvatar);

	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
	bool IsNftStored(FLiteNft InNft);

	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
	FNft GetNftFromVault(FLiteNft InNft);

	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
		FLiteNft InitLiteNft(FNft InNft);

	UFUNCTION(BlueprintCallable)
	void AddNftToVault(FLiteNft InNft, class UObject* InObject);

	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
	const FString GetCurrentRace() { return CurrentRace; }

	// ~IDVWalletInterface
	void OnWalletConnect_Implementation(FWalletConnectData WalletData);

	/////////////////////////
	void SetCurrentRace(const FString& NewRace);

	void SetCurrentSkin(const FString& NewSkin);

	void SetAccountName(const FString& NewName);
	UFUNCTION(BlueprintPure, Category = "Deverse|GameInstance")
	
	FPlayerProfile GetPlayerProfile();

	void SavePlayerProfile(FPlayerProfile Profile);
	void LoadPlayerProfile();
protected:
	FWallet Wallet;

	/** Use this to store downloaded nft images */
	UPROPERTY(BlueprintReadWrite)
	TArray<FNft> NftVault;

	UPROPERTY(BlueprintReadWrite)
		FString CurrentRace = "Human"; // For now let's assume human is base

	UPROPERTY(BlueprintReadWrite)
		FString CurrentSkin = "Mannequin";

	UPROPERTY(BlueprintReadOnly)
	FLiteNft ProfileAvatar;

	/** This is mostly used for anon. For logged in users, we use data from EOS instead */
	UPROPERTY(BlueprintReadOnly)
		FPlayerProfile PlayerProfile;
};
