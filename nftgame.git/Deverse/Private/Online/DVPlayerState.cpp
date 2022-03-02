// Copyright Cosugames, LLC 2021


#include "Online/DVPlayerState.h"
#include "Core/DVBaseGameInstance.h"

ADVPlayerState::ADVPlayerState()
{

}

void ADVPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ADVPlayerState::UpdatePlayerName_Implementation(const FString& InNewName)
{
	SetPlayerName(InNewName);
}

void ADVPlayerState::SetCurrentRace_Implementation(const FString& RaceId)
{
	SetCurrentRace(RaceId);
}

void ADVPlayerState::GetSpawnClass_Implementation(TSubclassOf<AActor>& SpawnClass)
{
	SpawnClass = UDVFunctionLibrary::GetRaceFromId(CurrentRace).RaceClass;
}

void ADVPlayerState::SetCurrentSkin_Implementation(const FString& SkinId)
{
	SetCurrentSkin(SkinId);
}

void ADVPlayerState::SetPlayerName(const FString& S)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerSetPlayerName(S);
	}
	Super::SetPlayerName(S);
	OnRep_PlayerName();
}

void ADVPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
}

void ADVPlayerState::SetCurrentRace(FString NewRace)
{
	if (GetLocalRole() < ROLE_Authority)
		ServerSetRace(NewRace);

	bool bFirstJoin = CurrentRace.IsEmpty();
	CurrentRace = NewRace;
	if (HasAuthority() && GetWorld()->GetAuthGameMode<AGameMode>()->GetMatchState() == MatchState::InProgress) {
		if (bFirstJoin) {
			IDVEventInterface::Execute_EventPlayerFirstJoin(GetWorld()->GetAuthGameMode(), GetOwner<AController>());
		}
		else {
			IDVEventInterface::Execute_EventRaceUpdated(GetWorld()->GetAuthGameMode(), GetOwner<AController>());
		}
	}		
}

void ADVPlayerState::SetCurrentSkin(const FString& NewSkin)
{
	if (GetLocalRole() < ROLE_Authority)
		ServerSetSkin(NewSkin);

	CurrentSkin = NewSkin;
}

void ADVPlayerState::ServerSetRace_Implementation(const FString& NewRace)
{
	SetCurrentRace(NewRace);
}

bool ADVPlayerState::ServerSetRace_Validate(const FString& NewRace)
{
	return true;
}

void ADVPlayerState::ServerSetSkin_Implementation(const FString& NewSkin)
{
	SetCurrentSkin(NewSkin);
}

bool ADVPlayerState::ServerSetSkin_Validate(const FString& NewSkin)
{
	return true;
}

void ADVPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
}

void ADVPlayerState::ServerSetPlayerName_Implementation(const FString& S)
{
	SetPlayerName(S);
}

bool ADVPlayerState::ServerSetPlayerName_Validate(const FString& S)
{
	return true;
}

void ADVPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ADVPlayerState, CurrentRace, COND_OwnerOnly);
	DOREPLIFETIME(ADVPlayerState, CurrentSkin);
	DOREPLIFETIME(ADVPlayerState, CurrentPartyId);
}