// Copyright Cosugames, LLC 2021
#include "Core/DVBaseController.h"
#include "Core/Human/DVBaseCharacter.h"
#include "Core/Components/DVNftManagerComponent.h"
#include "Core/DVBaseGameMode.h"
#include "Core/DVBaseGameState.h"
#include "Core/DVBaseGameInstance.h"
#include "DVGallerysPlace.h"
#include "JsonObjectConverter.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Online/DVPlayerState.h"
#include "ExampleOSS/VoiceChat/VoiceChatServices.h"
#include "ExampleOSS/ExampleCPPSubsystem.h"

ADVBaseController::ADVBaseController()
{
	NftManagerComp = CreateDefaultSubobject<UDVNftManagerComponent>(TEXT("Nft Manager Component"));
}

void ADVBaseController::JoinVoiceChannel_Implementation(const FString& InChannelName, const FString& InChannelCredentials)
{
	this->VoiceChatUser->JoinChannel(InChannelName, InChannelCredentials, EVoiceChatChannelType::NonPositional,
		FOnVoiceChatChannelJoinCompleteDelegate::CreateLambda([](const FString& ChannelName, const FVoiceChatResult& Result)
			{
				if (Result.IsSuccess())
				{
					UE_LOG(LogTemp, Warning, TEXT("Success"));
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Result failed"));
				}
			}
		)
	);
}

void ADVBaseController::OnLoginComplete(const FString& PlayerName, const FVoiceChatResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("Login complete"));
	
}

bool ADVBaseController::IsPlayerTalking()
{
	if (VoiceChatUser) {
		IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();	
		TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
		//UE_LOG(LogTemp, Warning, TEXT("User name %s"), *Identity->GetPlayerNickname(0));
		VoiceChatUser->SetOutputDeviceId("0");
		return VoiceChatUser->IsPlayerTalking(UserId->ToString());
	}
	return false;
}

void ADVBaseController::CreateParty(const UObject* WorldContextObject, int32 PartyTypeId, FOnCreatePartyDone OnDone)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(WorldContextObject->GetWorld());
	if (Subsystem == nullptr)
	{
		OnDone.ExecuteIfBound(false);
		return;
	}

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IOnlinePartyPtr Party = Subsystem->GetPartyInterface();

	TSharedRef<FPartyConfiguration> Config = MakeShared<FPartyConfiguration>();
	Config->bIsAcceptingMembers = true;
	Config->MaxMembers = 4;
	// You must set InvitePermissions to ::Anyone in order for synthetic parties to work.
	Config->InvitePermissions = PartySystemPermissions::EPermissionType::Anyone;

	if (!Party->CreateParty(
		*Identity->GetUniquePlayerId(0).Get(),
		(FOnlinePartyTypeId)PartyTypeId,
		*Config,
		FOnCreatePartyComplete::CreateUObject(
			this,
			&ADVBaseController::HandleCreatePartyComplete,
			WorldContextObject,
			OnDone)))
	{
		OnDone.ExecuteIfBound(false);
	}
}

void ADVBaseController::HandleCreatePartyComplete(const FUniqueNetId& LocalUserId, const TSharedPtr<const FOnlinePartyId>& PartyId, const ECreatePartyCompletionResult Result, const UObject* WorldContextObject, FOnCreatePartyDone OnDone)
{
	OnDone.ExecuteIfBound(Result == ECreatePartyCompletionResult::Succeeded);
	if (ADVPlayerState* PS = Cast<ADVPlayerState>(PlayerState)) {
		PS->CurrentPartyId = PartyId->ToString();
		UE_LOG(LogTemp, Warning, TEXT("Current party id %s"), *PS->CurrentPartyId); 
	}
}

void ADVBaseController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority()) {
		ADVBaseGameMode* GM = Cast<ADVBaseGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->SpawnShip(this);
	}
	// if (IsLocalController())
	// {
	// 	
	// 	AVoiceChatServices* VoiceChatService = NewObject<AVoiceChatServices>();
	// 	if (VoiceChatService) {
	// 		UExampleCPPSubsystem* OSS = GetGameInstance()->GetSubsystem<UExampleCPPSubsystem>();
	// 		if (OSS) {
	// 			//OSS->LoginToVoice(VoiceChatService, FExampleCPPSubsystemOnVoiceChatLoginComplete::BindUObject(this, &ADVBaseController::OnCPPLoginDone));
	// 		}
	// 	}
	//
	// 	IVoiceChat* VoiceChat = IVoiceChat::Get();
	// 	if (VoiceChat)
	// 	{
	// 		VoiceChatUser = VoiceChat->CreateUser();
	// 		if (VoiceChatUser == nullptr) return;
	// 		IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	// 		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	// 		if (!Identity.IsValid()) {
	// 			return;
	// 		}
	// 		TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
	// 		if (!UserId.IsValid()) {
	// 			return;
	// 		}
	// 		FPlatformUserId PlatformUserId = Identity->GetPlatformUserIdFromUniqueNetId(*UserId);
	// 		VoiceChatUser->Login(
	// 			PlatformUserId,
	// 			UserId->ToString(), 
	// 			TEXT(""),
	// 			FOnVoiceChatLoginCompleteDelegate::CreateUObject(this, &ADVBaseController::OnLoginComplete));
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Warning, TEXT("Voice chat interface not available"));
	// 		return;
	// 	}
	// }
}

void ADVBaseController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADVBaseController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("ToggleChat", IE_Pressed, this, &ADVBaseController::ToggleChat);
	InputComponent->BindAction("ToggleMenu", IE_Pressed, this, &ADVBaseController::ToggleMenu);
}

void ADVBaseController::ToggleChat()
{
	bChatActive = !bChatActive;
	IDVEventInterface::Execute_EventToggleChat(PersistentHUD, bChatActive);
}

void ADVBaseController::ToggleMenu()
{
	bMenuActive = !bMenuActive;
	if (bMenuActive) {
		FInputModeGameAndUI InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = true;
	}
	else {
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}
	K2_ToggleMenu(bMenuActive);
}

void ADVBaseController::FinishUpdateBalance_Implementation()
{

}

void ADVBaseController::OnNftSelect_Implementation(FNft InNft)
{
	NftManagerComp->SetCurrentNft(InNft);
}

void ADVBaseController::RequestSpawnNft_Implementation(FNft InNft)
{
	NftManagerComp->SpawnNft(FVector::ZeroVector, FRotator::ZeroRotator, FVector(0.5f), NftManagerComp->GetCurrentSelectedNft(), nullptr, NAME_None, true);
}

void ADVBaseController::RequestSetNftAvatar_Implementation(FLiteNft InNft)
{
	// TODO store set nft avatar on our master server
	// For now we store locally
	UDVFunctionLibrary::GetDVGameInstance(this)->SetProfileAvatar(InNft);
	if (Cast<ADVBaseCharacter>(GetPawn())) {
		FVector SpawnLoc;
		FRotator SpawnRot;
		USceneComponent* AttachComp = nullptr;
		FName BoneName = NAME_None;
		IDVWalletInterface::Execute_GetAvatarSpawnTransform(GetPawn(), SpawnLoc, SpawnRot, AttachComp, BoneName);
		NftManagerComp->SpawnNftAvatar(SpawnLoc, SpawnRot, FVector(0.5f), InNft, AttachComp, BoneName);
	}
}

void ADVBaseController::OnControlledPawnClick()
{
	FHitResult HitResult;
	GetHitResultUnderCursorByChannel(TraceTypeQuery1, true, HitResult);
	NftManagerComp->SpawnNft(HitResult.Location, HitResult.Normal.Rotation(), FVector(0.5f), NftManagerComp->GetCurrentSelectedNft(), HitResult.GetComponent(), HitResult.BoneName, false);
}

void ADVBaseController::OnNftSpawn_Implementation(AActor* SpawnedActor)
{
	IDVWalletInterface::Execute_OnNftSpawn(GetPawn(), SpawnedActor);
}

void ADVBaseController::UpdateAvatar_Implementation(bool bHasAvatar)
{
	IDVWalletInterface::Execute_UpdateAvatar(GetPawn(), bHasAvatar);
}

void ADVBaseController::RequestChangeRace_Implementation(FRace InRace)
{
	UDVBaseGameInstance* GI = GetGameInstance<UDVBaseGameInstance>();
	if (GI) {
		GI->SetCurrentRace(InRace.ClassId);
	}
	IDVEventInterface::Execute_SetCurrentRace(PlayerState, InRace.ClassId);
	ServerRequestChangeRace(InRace);
}

void ADVBaseController::GetSpawnClass_Implementation(TSubclassOf<AActor>& SpawnClass)
{
	
}

void ADVBaseController::RequestRespawn_Implementation(const FVector InSpawnLocation, const FRotator InSpawnRotation)
{
	RequestRespawn(InSpawnLocation, InSpawnRotation);
}

void ADVBaseController::SetDestination_Implementation(const FString& TravelURL, const FString& TravelId)
{
	UDVBaseGameInstance* GI = Cast<UDVBaseGameInstance>(GetGameInstance());
	if (GI) {
		GI->TravelId = TravelId;
		GI->TravelURL = TravelURL;
	}
}

void ADVBaseController::FinishDownloadNft_Implementation(FNft InNft)
{
	if (SpaceShip && IsLocalController())
		IDVWalletInterface::Execute_FinishDownloadNft(SpaceShip, InNft);
}

void ADVBaseController::EventSpaceshipSpawned_Implementation(AActor* SpawnedShip)
{
	SpaceShip = SpawnedShip;
}

void ADVBaseController::RequestRespawn(const FVector InSpawnLocation, const FRotator InSpawnRotation)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerRequestRespawn(InSpawnLocation, InSpawnRotation);
	}
	else {
		ADVBaseGameMode* GM = Cast<ADVBaseGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->RequestRespawn(this, InSpawnLocation, InSpawnRotation);
	}
}

void ADVBaseController::ServerRequestRespawn_Implementation(const FVector InSpawnLocation, const FRotator InSpawnRotation)
{
	RequestRespawn(InSpawnLocation, InSpawnRotation);
}

bool ADVBaseController::ServerRequestRespawn_Validate(const FVector InSpawnLocation, const FRotator InSpawnRotation)
{
	return true;
}

void ADVBaseController::ServerRequestChangeRace_Implementation(FRace InRace)
{
	if (GetPawn()) {
		FTransform CurrentTransform;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		CurrentTransform = GetPawn()->GetActorTransform();
		CurrentTransform = GetPawn()->GetActorTransform();
		APawn* CachePawn = GetPawn();
		UnPossess();
		CachePawn->Destroy();
		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(InRace.RaceClass, CurrentTransform, SpawnParams);
		Possess(NewPawn);
	}
	else {
		// Just for debugging
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADVSpaceShip::StaticClass(), FoundActors);
		for (AActor* FoundActor : FoundActors) {
			if (FoundActor && !FoundActor->IsPendingKill() && FoundActor->GetOwner() == this) {
				FTransform SpawnTransform;
				IDVEventInterface::Execute_GetPortalSpawnTransform(FoundActor, SpawnTransform);
				RequestRespawn(SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator());
			}
		}
		
	}
}

bool ADVBaseController::ServerRequestChangeRace_Validate(FRace InRace)
{
	return true;
}

void ADVBaseController::OnFinishQueryNfts_Implementation(const TArray<FNft>& Nfts)
{
	for (ADVGallerysPlace* Gallery : OwningGalleries) {
		IDVWalletInterface::Execute_OnFinishQueryNfts(Gallery, Nfts);
	}
}

void ADVBaseController::EventChat_Implementation(const FString& InMessage, EChatGroup ChatGroup)
{
	Chat(InMessage, ChatGroup);
}

void ADVBaseController::Chat(const FString& InMessage, EChatGroup ChatGroup)
{
	if (InMessage.IsEmpty()) return;
	if (GetLocalRole() < ROLE_Authority) {
		ServerChat(InMessage, ChatGroup);
	}
	else {
		ADVBaseGameState* GS = Cast<ADVBaseGameState>(GetWorld()->GetGameState());
		if (GS) {
			GS->Chat(GetPlayerName(), InMessage, ChatGroup);
		}	
	}
}

void ADVBaseController::ClientChatUpdate_Implementation(const FString& SenderId, const FString& InMessage, EChatGroup ChatGroup)
{
	if (PersistentHUD) {
		IDVEventInterface::Execute_EventChatUpdate(PersistentHUD, SenderId, InMessage, ChatGroup);
	}
		
}

FString ADVBaseController::GetPlayerName()
{
	if (PlayerState)
		return PlayerState->GetPlayerName();
	return "";
}

#if WITH_EDITOR

void ADVBaseController::RequestDestroySpaceship()
{
	ServerDestroySpaceship();
}

void ADVBaseController::ServerDestroySpaceship_Implementation()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADVSpaceShip::StaticClass(), FoundActors);
	for (AActor* FoundActor : FoundActors)
	{
		if (FoundActor->GetOwner() == this)
		{
			ADVBaseGameState* GS = GetWorld()->GetGameState<ADVBaseGameState>();
			if (GS) {
				GS->RemoveSpaceshipId(FString::Printf(TEXT("portal_%s_ship"), *this->GetName()), FoundActor);
			}
			FoundActor->Destroy();
			break;
		}
	}
}

bool ADVBaseController::ServerDestroySpaceship_Validate()
{
	return true;
}
#endif

void ADVBaseController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (IsLocalController()) {
		UDVBaseGameInstance* GI = Cast<UDVBaseGameInstance>(GetGameInstance());
		if (GI) {
			IDVEventInterface::Execute_SetCurrentRace(PlayerState, GI->GetCurrentRace());
			IDVEventInterface::Execute_UpdatePlayerName(PlayerState, GI->GetPlayerProfile().PlayerName);
		}
	}
}

void ADVBaseController::InitPlayerState()
{
	Super::InitPlayerState();
	OnRep_PlayerState();
}

void ADVBaseController::ServerChat_Implementation(const FString& InMessage, EChatGroup ChatGroup)
{
	Chat(InMessage, ChatGroup);
}

bool ADVBaseController::ServerChat_Validate(const FString& InMessage, EChatGroup ChatGroup)
{
	return true;
}

void ADVBaseController::BeginDestroy()
{
	// IVoiceChat* VoiceChat = IVoiceChat::Get();
	// if (this->VoiceChatUser != nullptr && VoiceChat != nullptr)
	// {
	// 	VoiceChat->ReleaseUser(this->VoiceChatUser);
	// 	this->VoiceChatUser = nullptr;
	// 	UE_LOG(LogTemp, Warning, TEXT("Remove voicechat from server"));
	// }
	Super::BeginDestroy();
}
