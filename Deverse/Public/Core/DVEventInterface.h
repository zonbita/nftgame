// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Deverse/Deverse.h"
#include "DVEventInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDVEventInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEVERSE_API IDVEventInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
	void EventTrigger(FCustomEvent InEvent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventHealthChanged(float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
	void EventInteract(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
	void EventTeleportInitiated(const FVector TeleportLocation, const FRotator TeleportRotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventTeleportInitiatedId(const FString& TeleportId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventTeleportVerseInitiated(const FString& VerseUrl);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventTeleportDone();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		bool IsTeleporting();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void CanBeInteracted(bool& OutInteract);

	/** Call this when ends overlappig portal, signaling teleporting is no longer available */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventEndPortal();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void GetSpawnClass(TSubclassOf<AActor>& SpawnClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void SetCurrentRace(const FString& RaceId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void SetCurrentSkin(const FString& SkinId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventRaceUpdated(AController* InController);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void GetPortalSpawnTransform(FTransform& SpawnTransform);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void RequestRespawn(const FVector SpawnLocation, const FRotator SpawnRotation);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void SetDestination(const FString& TravelURL, const FString& TravelId);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
	void UpdatePortal(const FString& PortalId, const FString& TravelId, const FString& PortalCustomUrl);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void UpdateActorId(const FString& InId);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventSpaceshipSpawned(AActor* SpawnedShip);

	/** This is mostly used by GameState */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventBroadcastChat(const FString& Sender, const FString& Message, EChatGroup ChatGroup);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventChat(const FString& Message, EChatGroup ChatGroup);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventChatUpdate(const FString& SenderId, const FString& Message, EChatGroup ChatGroup);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventToggleChat(bool bOn);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void EventPlayerFirstJoin(AController* NewController);

	// Data events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void UpdatePlayerName(const FString& InName);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DV Event Interface")
		void OnPlayerNameUpdated(const FString& NewName);
};
