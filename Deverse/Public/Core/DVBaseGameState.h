// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Deverse/Deverse.h"
#include "Core/DVEventInterface.h"
#include "DVBaseGameState.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API ADVBaseGameState : public AGameState, public IDVEventInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	TArray<class ADVSpaceShip*> AllShips;

	UPROPERTY(ReplicatedUsing = OnRep_ShipIds, BlueprintReadOnly)
		TArray<FString> AllShipPortalIds;

	UFUNCTION()
	void OnRep_ShipIds();

	void AddSpaceshipId(const FString InId, ADVSpaceShip* InShip);

	void RemoveSpaceshipId(const FString InId, AActor* InShip);

	UFUNCTION(BlueprintCallable)
	void RemoveId(int32 Index);

	void Chat(const FString& SenderId, const FString& Message, EChatGroup ChatGroup);

	// ~IDVEventInterface
	void EventBroadcastChat_Implementation(const FString& Sender, const FString& Message, EChatGroup ChatGroup);

};
