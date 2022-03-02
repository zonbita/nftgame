// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Core/DVGameEngine.h"
#include "DVGlobalSettings.h"
#include "DVFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Global Library")
	static FString GetOnlineMasterServerUrl() {
		return GetMutableDefault<UDVGlobalSettings>()->MasterServerUrl;
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetLocalMasterServerUrl() {
		return GetMutableDefault<UDVGlobalSettings>()->LocalMasterServerUrl;
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static bool IsUsingLocalServer() {	
		return GetMutableDefault<UDVGlobalSettings>()->bUseLocalServer;
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetMasterServerUrl() {
		return IsUsingLocalServer() ? GetLocalMasterServerUrl() : GetOnlineMasterServerUrl();
	}

	UFUNCTION(BlueprintPure, Category = "EVM")
	static float FromWei(float InWei, int32 InDecimals);

	UFUNCTION(BlueprintCallable, Category = "EVM", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static void GetNFTFromAddress(const UObject* WorldContextObject, FString InAddress);

	UFUNCTION(BlueprintCallable, Category = "EVM", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void GetNFTFromMS(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "EVM", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static void QueryAddressBalance(const UObject* WorldContextObject, FString InAddress, EChain InChain);

	UFUNCTION(BlueprintCallable, Category = "EVM", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
		static FString GetWalletAddress(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Global Library", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true"))
	static UDVBaseGameInstance* GetDVGameInstance(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Global Library")
	static FRace GetRaceFromId(const FString& InId);

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FSkin GetSkinFromId(const FString& InId);

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetDisplayNameFromChainEnum(EChain InChain) {
		switch (InChain) {
		case EChain::EAll:
			return "All";
		case EChain::EAvax:
			return "Avalanche";
		case EChain::EBsc:
			return "Binance Smart Chain";
		case EChain::EEth:
			return "Ethereum";
		case EChain::EPolygon:
			return "Polygon";
		}
		return "NULL";
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static EChain GetChainFromDisplayName(const FString& InName) {
		if (InName.Contains("Binance")) {
			return EChain::EBsc;
		}
		else if (InName.Contains("Avax")) {
			return EChain::EAvax;
		}
		else if (InName.Contains("Eth")) {
			return EChain::EEth;
		}
		else if (InName.Contains("Poly")) {
			return EChain::EPolygon;
		}
		else if (InName.Contains("All")) {
			return EChain::EAll;
		}
		return EChain::EAll;
	}

	UFUNCTION(BlueprintCallable, Category = "Global Library")
		static void CheckLineKeyValid(const TArray<int32> InCorrectKeys, const TArray<int32> InputKeys, bool& bValid) {
		if (InputKeys.Num() != InCorrectKeys.Num()) {
			bValid = false;
			return;
		}
		for (int32 i = 0; i < InputKeys.Num(); i++) {
			if (!InCorrectKeys.Contains(InputKeys[i])) {
				UE_LOG(LogTemp, Warning, TEXT("index %d correct %d, current %d"), i, InCorrectKeys[i], InputKeys[i]);
				bValid = false;
				return;
			}
		}
		bValid = true;
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetCookie() {
		FString CookieStr;
		FString CookieFile = FPaths::ProjectSavedDir() + "/cookie";
		IPlatformFile& File = FPlatformFileManager::Get().GetPlatformFile();

		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*CookieFile)) {
			bool bLoaded = FFileHelper::LoadFileToString(CookieStr, *CookieFile);
			if (!bLoaded) {
				UE_LOG(LogTemp, Error, TEXT("Cookie not valid. Please try to sign first"));
			}
			else {
				return CookieStr;
			}
		}
		return "NULL";
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString ConstructJsonArrayStr(TMap<FString, FString> ValuesArr) {
		FString FinalString;
		TArray<TSharedPtr<FJsonValue>> JsonArr;
		TSharedPtr<FJsonObject> NftObject = MakeShareable(new FJsonObject);
		for (const TPair<FString, FString>& Pair : ValuesArr) {
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			JsonObject->SetStringField("image_url", Pair.Key);
			JsonObject->SetStringField("token_address", Pair.Value);
			JsonArr.Add(MakeShareable(new FJsonValueObject(JsonObject)));
		}
		NftObject->SetArrayField("nfts", JsonArr);
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&FinalString);
		FJsonSerializer::Serialize(NftObject.ToSharedRef(), Writer);
		UE_LOG(LogTemp, Warning, TEXT("Final string %s"), *FinalString); 
		return FinalString;
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetNetworkRoleStr(UActorComponent* InComponent) {
		switch (InComponent->GetOwnerRole()) {
		case ENetRole::ROLE_Authority:
			return "Authority";
		case ENetRole::ROLE_AutonomousProxy:
			return "Autonomous";
		case ENetRole::ROLE_SimulatedProxy:
			return "SimulatedProxy";
		case ENetRole::ROLE_None:
			return "None";
		}
		return "None";
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static FString GetRemoteRoleStr(UActorComponent* InComponent) {
		switch (InComponent->GetOwner()->GetRemoteRole()) {
		case ENetRole::ROLE_Authority:
			return "Authority";
		case ENetRole::ROLE_AutonomousProxy:
			return "Autonomous";
		case ENetRole::ROLE_SimulatedProxy:
			return "SimulatedProxy";
		case ENetRole::ROLE_None:
			return "None";
		}
		return "None";
	}

	UFUNCTION(BlueprintPure, Category = "Global Library")
		static bool HasNetConnection(AActor* InActor) {
		return InActor && InActor->GetNetConnection();
	}
};
