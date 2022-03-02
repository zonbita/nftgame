// Copyright Cosugames, LLC 2021

#include "Core/DVBaseGameState.h"
#include "Net/UnrealNetwork.h"
#include "Core/DVBaseController.h"
#include "GameFramework/PlayerState.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/DVEventInterface.h"

void ADVBaseGameState::OnRep_ShipIds()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(this, FoundWidgets, UDVEventInterface::StaticClass(), true);
	for (auto Ship : AllShipPortalIds)
	{
		for (UUserWidget* FoundWidget : FoundWidgets) {
			IDVEventInterface::Execute_UpdatePortal(FoundWidget, Ship, "", "");
		}
	}
}

void ADVBaseGameState::AddSpaceshipId(const FString InId, ADVSpaceShip* InShip)
{
	AllShipPortalIds.AddUnique(InId);
	AllShips.AddUnique(InShip);
	OnRep_ShipIds();
}

void ADVBaseGameState::RemoveSpaceshipId(const FString InId, AActor* InShip)
{
	if (AllShipPortalIds.Contains(InId))
		AllShipPortalIds.Remove(InId);
	if (AllShips.Contains(InShip))
		AllShips.Remove(Cast<ADVSpaceShip>(InShip));
	OnRep_ShipIds();
}

void ADVBaseGameState::RemoveId(int32 Index)
{
	if (AllShipPortalIds.IsValidIndex(Index)) {
		AllShipPortalIds.RemoveAt(Index);
	}
	OnRep_ShipIds();
}

void ADVBaseGameState::Chat(const FString& SenderId, const FString& Message, EChatGroup ChatGroup)
{
	for (APlayerState* PS : PlayerArray) {
		ADVBaseController* OwnerController = Cast<ADVBaseController>(PS->GetOwner());
		if (OwnerController)
			OwnerController->ClientChatUpdate(SenderId, Message, ChatGroup);
	}
	switch (ChatGroup) {
	case EChatGroup::EAll:

		break;
	case EChatGroup::EDAO:

		break;
	case EChatGroup::EPrivate:

		break;
	case EChatGroup::ETeam:

		break;
	}
}

void ADVBaseGameState::EventBroadcastChat_Implementation(const FString& Sender, const FString& Message, EChatGroup ChatGroup)
{
	Chat(Sender, Message, ChatGroup);
}

void ADVBaseGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADVBaseGameState, AllShips);
	DOREPLIFETIME(ADVBaseGameState, AllShipPortalIds);

}