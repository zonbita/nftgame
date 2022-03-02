// Copyright Cosugames, LLC 2021


#include "Core/DVBaseGameInstance.h"

#include "JsonObjectConverter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/DVWalletInterface.h"
#include "Kismet/GameplayStatics.h"
#include "BlockchainRequest/DVCreateLoginLink.h"
#include "BlockchainRequest/DVUpdateWalletNft.h"

void UDVBaseGameInstance::Init()
{
	Super::Init();
	LoadPlayerProfile();
}

void UDVBaseGameInstance::LoadPlayerProfile()
{
	FString JsonString;
	bool isSuccess = FFileHelper::LoadFileToString(JsonString, *(FPaths::ProjectSavedDir() + TEXT("/Data/DataPlayerProfiles.json")));
	if(isSuccess)
	{
		FJsonObjectConverter::JsonObjectStringToUStruct(JsonString, &PlayerProfile, 0, 0);
	}else
	{
		UE_LOG(LogTemp, Warning, TEXT("Không có file DataPlayerProfiles.json") );
	}
	
}

void UDVBaseGameInstance::UpdateNativeWalletBalance(float InValue)
{
	Wallet.Balance = InValue;
}

void UDVBaseGameInstance::UpdateTokenBalance(FToken InToken)
{
	Wallet.AvailableTokens.Add(InToken);
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UDVWalletInterface::StaticClass(), OutActors);
	for (AActor* OutActor : OutActors) {
		IDVWalletInterface::Execute_OnTokenUpdate(OutActor, InToken);
	}
	TArray<UUserWidget*> OutWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld()->GetFirstPlayerController(), OutWidgets, UDVWalletInterface::StaticClass(), true);
	for (UUserWidget* OutWidget : OutWidgets) {
		IDVWalletInterface::Execute_OnTokenUpdate(OutWidget, InToken);
	}
}

void UDVBaseGameInstance::UpdateNftDisplay(const FString InAddress, const FString InTokenId, UObject* InDisplay)
{
	if (!InAddress.IsEmpty()) {
		for (int32 i = 0; i < Wallet.AvailableNfts.Num(); i++) {
			if (Wallet.AvailableNfts[i].TokenAddress.Equals(InAddress) && Wallet.AvailableNfts[i].TokenId.Equals(InTokenId)) {
				Wallet.AvailableNfts[i].Object = InDisplay;
				//UE_LOG(LogTemp, Warning, TEXT("Address %s"), *InAddress); 
				NftVault.Add(Wallet.AvailableNfts[i]);
				TArray<AActor*> OutActors;
				UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UDVWalletInterface::StaticClass(), OutActors);
				for (AActor* OutActor : OutActors) {
					if (OutActor->IsA<APlayerController>())
						IDVWalletInterface::Execute_FinishDownloadNft(OutActor, Wallet.AvailableNfts[i]);					
				}
				TArray<UUserWidget*> OutWidgets;
				UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld()->GetFirstPlayerController(), OutWidgets, UDVWalletInterface::StaticClass(), true);
				for (UUserWidget* OutWidget : OutWidgets) {
					IDVWalletInterface::Execute_FinishDownloadNft(OutWidget, Wallet.AvailableNfts[i]);
				}
				break;
			}
		}
	}
}

void UDVBaseGameInstance::UpdateNftHoldings(FNft InNft)
{
	int32 Index = Wallet.AvailableNfts.Add(InNft);
#if WITH_EDITORONLY_DATA
	// Fake nft ffs the ipfs not working
	Wallet.AvailableNfts[Index].Metadata.ImageURL = RandomImageURLs.IsValidIndex(Index) ? RandomImageURLs[Index] : "";
#endif
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UDVWalletInterface::StaticClass(), OutActors);
	for (AActor* OutActor : OutActors) {
		IDVWalletInterface::Execute_OnNftUpdate(OutActor, Wallet.AvailableNfts[Index]);
	}
	/*TArray<UUserWidget*> OutWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld()->GetFirstPlayerController(), OutWidgets, UDVWalletInterface::StaticClass(), true);
	for (UUserWidget* OutWidget : OutWidgets) {
		IDVWalletInterface::Execute_OnNftUpdate(OutWidget, InNft);
	}*/
	IDVWalletInterface::Execute_OnNftUpdate(this, Wallet.AvailableNfts[Index]);
}

void UDVBaseGameInstance::ClearBalance()
{
	Wallet.AvailableTokens.Empty();
}

void UDVBaseGameInstance::ClearNfts()
{
	Wallet.AvailableTokens.Empty();
}

void UDVBaseGameInstance::FinishUpdateBalance()
{
	IDVWalletInterface::Execute_OnFinishUpdateTokens(GetWorld()->GetFirstPlayerController());
}

void UDVBaseGameInstance::FinishQueryNfts(bool bUpdateToDatabase)
{
	if (!GetWorld()) return;
	IDVWalletInterface::Execute_OnFinishQueryNfts(GetWorld()->GetFirstPlayerController(), Wallet.AvailableNfts);
	UDVUpdateWalletNft* UpdateWalletTask = NewObject<UDVUpdateWalletNft>();
	TArray<FLiteNft> LiteNftArr;
	if (UpdateWalletTask) {
		for (FNft Nft : Wallet.AvailableNfts) {
			if (!Nft.Metadata.ImageURL.IsEmpty()) {
				FLiteNft LiteNft;
				LiteNft.Metadata = Nft.Metadata;
				LiteNft.TokenAddress = Nft.TokenAddress;
				LiteNftArr.Add(LiteNft);
			}
		}
		if (bUpdateToDatabase)
			UpdateWalletTask->HandleUpdateWallet(this, LiteNftArr);
	}
}


void UDVBaseGameInstance::SavePlayerProfile(FPlayerProfile Profile)
{ 
	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(Profile, JsonString);
	FFileHelper::SaveStringToFile(*JsonString, *(FPaths::ProjectSavedDir() + TEXT("/Data/DataPlayerProfiles.json")));
}

void UDVBaseGameInstance::SetProfileAvatar(FLiteNft InAvatar)
{
	// TODO check if player still owns this nft
	PlayerProfile.CurrentAvatar = InAvatar;
	TArray<UUserWidget*> OutWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld()->GetFirstPlayerController(), OutWidgets, UDVWalletInterface::StaticClass(), true);
	for (UUserWidget* OutWidget : OutWidgets) {
		IDVWalletInterface::Execute_UpdateProfileAvatar(OutWidget, PlayerProfile.CurrentAvatar);
	}
}

bool UDVBaseGameInstance::IsNftStored(FLiteNft InNft)
{
	for (FNft Nft : NftVault) {
		if (Nft.TokenAddress.Equals(InNft.TokenAddress) && Nft.TokenId.Equals(InNft.TokenId))
			return true;
	}
	return false;
}

FNft UDVBaseGameInstance::GetNftFromVault(FLiteNft InNft)
{
	FNft FoundNft;
	for (FNft Nft : NftVault) {
		if (Nft.TokenAddress.Equals(InNft.TokenAddress) && Nft.TokenId.Equals(InNft.TokenId)) {
			FoundNft = Nft;
			break;
		}
	}
	return FoundNft;
}

FLiteNft UDVBaseGameInstance::InitLiteNft(FNft InNft)
{
	FLiteNft LiteNft = FLiteNft(InNft.TokenAddress, InNft.OwnerAddress, InNft.Name, InNft.Symbol, InNft.TokenId);
	LiteNft.Metadata.ImageURL = InNft.Metadata.ImageURL;
	return LiteNft;
}

void UDVBaseGameInstance::AddNftToVault(FLiteNft InNft, class UObject* InObject)
{
	FNft Nft;
	Nft.TokenAddress = InNft.TokenAddress;
	Nft.OwnerAddress = InNft.OwnerAddress;
	Nft.Name = InNft.Name;
	Nft.Symbol = InNft.Symbol;
	Nft.Metadata.ImageURL = InNft.Metadata.ImageURL;
	Nft.TokenId = InNft.TokenId;
	NftVault.Add(Nft);
}

void UDVBaseGameInstance::OnWalletConnect_Implementation(FWalletConnectData WalletData)
{
	Wallet.Address = WalletData.Address;
	TArray<UUserWidget*> OutWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(GetWorld()->GetFirstPlayerController(), OutWidgets, UDVWalletInterface::StaticClass(), true);
	for (UUserWidget* OutWidget : OutWidgets) {
		IDVWalletInterface::Execute_OnWalletConnect(OutWidget, WalletData);
	}
}

void UDVBaseGameInstance::SetCurrentRace(const FString& NewRace)
{
	CurrentRace = NewRace;
	PlayerProfile.RaceId = NewRace;
}

void UDVBaseGameInstance::SetCurrentSkin(const FString& NewSkin)
{
	PlayerProfile.SkinId = NewSkin;
}

void UDVBaseGameInstance::SetAccountName(const FString& NewName)
{
	PlayerProfile.PlayerName = NewName;
}

FPlayerProfile UDVBaseGameInstance::GetPlayerProfile()
{
	return PlayerProfile;
}
