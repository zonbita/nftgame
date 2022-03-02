// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Deverse/Deverse.h"
#include "DVWalletInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDVWalletInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEVERSE_API IDVWalletInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
	void OnNftUpdate(FNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void FinishDownloadNft(FNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void OnNftSpawn(AActor* SpawnedNft);

	void OnBalanceUpdate();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
	void OnTokenUpdate(FToken InToken);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void OnFinishUpdateTokens();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void OnFinishQueryNfts(const TArray<FNft>& Nfts);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void RequestSpawnNft(FNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void RequestChangeRace(FRace InRace);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void RequestUpdateNftDisplay(FLiteNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void RequestSetNftAvatar(FLiteNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void OnNftSelect(FNft InNft);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void GetNftSpawnTransform(FTransform& SpawnTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void GetNftAttachComponent(USceneComponent*& Component);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void GetAvatarSpawnTransform(FVector& SpawnLocation, FRotator& SpawnRotation, USceneComponent*& AttachComp, FName& BoneName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void GetCurrentAvatar(AActor*& Avatar);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void UpdateAvatar(bool bHasAvatar);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
		void OnWalletConnect(FWalletConnectData WalletData);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wallet Interface")
	void UpdateProfileAvatar(const FLiteNft& InAvatarNft);
};
