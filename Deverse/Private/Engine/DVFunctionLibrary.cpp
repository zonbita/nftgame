// Copyright Cosugames, LLC 2021


#include "Engine/DVFunctionLibrary.h"
#include "BlockchainRequest/DVNftQuery.h"
#include "BlockchainRequest/DVWalletQuery.h"
#include "Core/DVBaseGameInstance.h"
#include "Core/DVBaseGameInstance.h"

float UDVFunctionLibrary::FromWei(float InWei, int32 InDecimals)
{
	float Decimals = 1.f;
	for (int i = 0; i < InDecimals; i++) {
		Decimals *= 10.f;
	}
	return InWei / Decimals;
}

void UDVFunctionLibrary::GetNFTFromAddress(const UObject* WorldContextObject, FString InAddress)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
		if (!InAddress.IsEmpty()) {
			UDVNftQuery* NftQuery = NewObject<UDVNftQuery>();
			NftQuery->RequestNft(InAddress, Cast<UDVBaseGameInstance>(World->GetGameInstance()));
		}
	}
}

void UDVFunctionLibrary::GetNFTFromMS(const UObject* WorldContextObject)
{
	UDVFetchWalletNft* FetchNft = NewObject<UDVFetchWalletNft>();
	if (FetchNft)
		FetchNft->HandleGetWalletNft(WorldContextObject);
}

void UDVFunctionLibrary::QueryAddressBalance(const UObject* WorldContextObject, FString InAddress, EChain InChain)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
		if (!InAddress.IsEmpty()) {
			UDVWalletQuery* WalletQuery = NewObject<UDVWalletQuery>();
			WalletQuery->RequestBalance(InAddress, InChain, Cast<UDVBaseGameInstance>(World->GetGameInstance()));
		}
	}
}

FString UDVFunctionLibrary::GetWalletAddress(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) {
		UDVBaseGameInstance* GI = Cast<UDVBaseGameInstance>(World->GetGameInstance());
		if (GI)
			return GI->GetWallet().Address;
	}
	return "NULL";
}

UDVBaseGameInstance* UDVFunctionLibrary::GetDVGameInstance(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return Cast<UDVBaseGameInstance>(World->GetGameInstance());
	}
	return nullptr;
}

FRace UDVFunctionLibrary::GetRaceFromId(const FString& InId)
{
	UDataTable* Data = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/Data/DeverseRace.DeverseRace'"));
	if (Data) {
		TArray<FRace*> RaceRows;
		Data->GetAllRows("", RaceRows);
		for (FRace* RaceRow : RaceRows) {
			if ((*RaceRow).ClassId.Equals(InId)) {
				return *RaceRow;
			}
		}
	}
	return FRace();
}

FSkin UDVFunctionLibrary::GetSkinFromId(const FString& InId)
{
	UDataTable* Data = LoadObject<UDataTable>(nullptr, TEXT("DataTable'/Game/Data/DeverseSkin.DeverseSkin'"));
	if (Data) {
		TArray<FSkin*> RaceRows;
		Data->GetAllRows("", RaceRows);
		for (FSkin* RaceRow : RaceRows) {
			if ((*RaceRow).SkinId.Equals(InId)) {
				return *RaceRow;
			}
		}
	}
	return FSkin();
}
