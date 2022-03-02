// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "IImageWrapper.h"
#include "Engine/DataTable.h"
#include "Modules/ModuleInterface.h"
#include "Deverse.generated.h"

#define SURFACE_SAND SurfaceType1
#define SURFACE_CONCRETE SurfaceType2
#define SURFACE_METAL SurfaceType3
#define SURFACE_BRICK SurfaceType4
#define SURFACE_ROCK SurfaceType5
#define SURFACE_BODY SurfaceType6
#define SURFACE_GRAVEL SurfaceType7
#define SURFACE_PEBBLE SurfaceType8
#define SURFACE_HEAD SurfaceType9
#define SURFACE_CLAY SurfaceType10
#define SURFACE_WATER SurfaceType11
#define SURFACE_WOOD SurfaceType12
#define SURFACE_ELECTRONIC SurfaceType13
#define SURFACE_DIRT SurfaceType14
#define SURFACE_GLASS SurfaceType15
#define SURFACE_GRASS SurfaceType16
#define SURFACE_SHEETROCK SurfaceType17
#define SURFACE_TILE SurfaceType18
#define SURFACE_ASPHALT SurfaceType19
#define SURFACE_CARDBOARD SurfaceType20
#define SURFACE_CARPET SurfaceType21
#define SURFACE_LEAVES SurfaceType22
#define SURFACE_PLASTER SurfaceType23
#define SURFACE_PLASTIC SurfaceType24
#define SURFACE_CLOTH SurfaceType25
#define SURFACE_SNOW SurfaceType26
#define SURFACE_ICE SurfaceType27
#define SURFACE_THINMETAL SurfaceType28
#define SURFACE_SOLIDMETAL SurfaceType29

UENUM(BlueprintType)
enum class ECharacterActivity : uint8 {
  EIdle = 0	UMETA(DisplayName = "Idle"),
  EWalk = 1	UMETA(DisplayName = "Walk"),
  ERun = 2	UMETA(DisplayName = "Run"),
  EJump = 3	UMETA(DisplayName = "Jump"),
  ECrouch = 4	UMETA(DisplayName = "Crouch"),
  EProne = 5	UMETA(DisplayName = "Prone"),
  ESlowWalk = 6	UMETA(DisplayName = "SlowWalk")
};

UENUM(BlueprintType)
enum class EChatGroup : uint8 {
	EAll UMETA(DisplayName = "All"),
	ETeam UMETA(DisplayName = "Team"),
	EDAO UMETA(DisplayName=  "DAO"),
	EPrivate UMETA(DisplayName= "Private")
};

USTRUCT(BlueprintType)
struct FNftMetadata {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString ImageURL;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Name;

	FNftMetadata() {
		Description = "";
		ImageURL = "";
		Name = "";
	}
};

USTRUCT(BlueprintType)
struct FImageData {
	GENERATED_BODY()
public:
	TArray64<uint8>* PixelData;

	int32 SizeX; 
	
	int32 SizeY; 
	
	EPixelFormat InFormat;

	EImageFormat ImageFormat = EImageFormat::Invalid;

	FImageData() {

	}
};

USTRUCT(BlueprintType)
struct FNft {
	GENERATED_BODY()
public:
	/** This is db id */
	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		int64 Id;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString TokenAddress;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString TokenId;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		int32 Amount;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString OwnerAddress;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		int64 BlockNumber;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		int64 MintedBlockNumber;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString ContractType;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString TokenUri;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FNftMetadata Metadata;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Symbol;

	UPROPERTY(BlueprintReadWrite, Category = "NFT")
	UObject* Object;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
	FImageData ImageData;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FDateTime CreatedAt;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FDateTime UpdatedAt;

	FNft() {
		TokenAddress = "";
		TokenId = "";
		Amount = 0;
		OwnerAddress = "";
		BlockNumber = 0;
		MintedBlockNumber = 0;
		ContractType = "";
		TokenUri = "";
		Name = "";
		Symbol = "";
		Object = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FLiteNft {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "NFT")
	FString TokenAddress;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString TokenId;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString OwnerAddress;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FNftMetadata Metadata;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "NFT")
		FString Symbol;

	FLiteNft() {
		TokenAddress = "";
		OwnerAddress = "";
		Name = "";
		Symbol = "";
	}

	FLiteNft(const FString InAddress, const FString InOwnerAddress, const FString InName, const FString InSymbol, const FString InTokenId) {
		TokenAddress = InAddress;
		TokenId = InTokenId;
		OwnerAddress = InOwnerAddress;
		Name = InName;
		Symbol = InSymbol;
	}
};

USTRUCT(BlueprintType)
struct FCustomEvent {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
		FString Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
		FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
		FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Event")
		class UObject* Object;
};

USTRUCT(BlueprintType)
struct FWalletCard {
	GENERATED_BODY()
public:
	void SetWalletCardMasterPassword(const FString InPassword) {
		WalletCardMasterPassword = InPassword;
	}

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Wallet Card")
	FString WalletCardMasterPassword;

};

USTRUCT(BlueprintType)
struct FToken {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
	FString TokenAddress;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		FString Symbol;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		FString Logo;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		FString Thumbnail;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		int32 Decimals;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Token")
		float Balance;
};

USTRUCT(BlueprintType)
struct FWallet {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "Deverse|Wallet")
	FString Address;

	UPROPERTY(BlueprintReadWrite, Category = "Deverse|Wallet")
		FString NameService;

	UPROPERTY(BlueprintReadWrite, Category = "Deverse|Wallet")
		float Balance;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Wallet")
	TArray<FToken> AvailableTokens;

	UPROPERTY(BlueprintReadOnly, Category = "Deverse|Wallet")
		TArray<FNft> AvailableNfts;
};

USTRUCT(BlueprintType)
struct FRace : public FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FString ClassId;

	UPROPERTY(EditDefaultsOnly)
		FText RaceName;

	UPROPERTY(EditDefaultsOnly)
		FText Description;

	// TODO make this soft pointer
	UPROPERTY(EditDefaultsOnly)
	class TSubclassOf<APawn> RaceClass;
};

USTRUCT(BlueprintType)
struct FSkin : public FTableRowBase {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	FString SkinId;

	UPROPERTY(EditDefaultsOnly)
		FText SkinName;

	UPROPERTY(EditDefaultsOnly)
		FText Description;

	// TODO make this soft pointer
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class USkeletalMesh* SkinMesh;
};

USTRUCT(BlueprintType)
struct FWalletConnectData {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	int32 Id;

	UPROPERTY(BlueprintReadOnly)
		FString Address;

	UPROPERTY(BlueprintReadOnly)
		FString Nonce;

	UPROPERTY(BlueprintReadOnly)
		FString CreatedAt;

	UPROPERTY(BlueprintReadOnly)
		FString UpdatedAt;

	FWalletConnectData() {};
};

USTRUCT(BlueprintType)
struct FVerseAddress {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString Url;

	UPROPERTY(BlueprintReadWrite)
		FString VerseName;

	UPROPERTY(BlueprintReadWrite)
		FString Description;
};

USTRUCT(BlueprintType)
struct FLineKeyPass {
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<int32> KeyCodes;
};

UENUM(BlueprintType)
enum class EChain : uint8 {
	EAll UMETA(DisplayName =  "All"),
	EEth UMETA(DisplayName = "Ethereum"),
	EBsc UMETA(DisplayName = "Binance smart chain"),
	EPolygon UMETA(DisplayName = "Polygon"),
	EAvax UMETA(DisplayName = "Avalanche")	
};

USTRUCT(BlueprintType)
struct FPlayerProfile {
	GENERATED_BODY();
public:
	UPROPERTY(BlueprintReadOnly, Category = "Player Profile")
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, Category = "Player Profile")
		FString SkinId;

	UPROPERTY(BlueprintReadOnly, Category = "Player Profile")
		FString RaceId;

	UPROPERTY(BlueprintReadOnly, Category = "Player Profile")
		FLiteNft CurrentAvatar;
};

UENUM(BlueprintType)
enum class EGameSetting : uint8 {
	EWindowsmode UMETA(DisplayName =  "WindowMode"),
	EResolution UMETA(DisplayName = "Resolution"),
	EVS UMETA(DisplayName = "Vertical sync"),
	EAA UMETA(DisplayName = "Anti-Aliasing"),
	EVDi UMETA(DisplayName = "View Distance"),
	
	EPP UMETA(DisplayName = "Post Processing"),
	ESD UMETA(DisplayName = "Shadows"),
	ETex UMETA(DisplayName = "Texture"),
	EEf UMETA(DisplayName = "Effects"),
	EFoliage UMETA(DisplayName = "Foliage"),
	ESP UMETA(DisplayName = "SharPen"),
	EScP UMETA(DisplayName = "ScreenPercentage"),

	Emaster UMETA(DisplayName = "Master"),
	EMusic UMETA(DisplayName = "Music"),
	ESoundFX UMETA(DisplayName = "SoundFX"),
	ERadio UMETA(DisplayName = "Radio"),
	EVoice UMETA(DisplayName = "Voice")
};