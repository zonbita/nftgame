// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/DVEventInterface.h"
#include "Core/DVWalletInterface.h"
#include "DVSpaceShip.generated.h"

UCLASS()
class DEVERSE_API ADVSpaceShip : public AActor, public IDVEventInterface, public IDVWalletInterface
{
	GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ADVSpaceShip();
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<class ADVPortal> PortalClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<class ADVGallerysPlace> GalleryPlaceClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UBoxComponent* OverlapComponent;

	UPROPERTY(EditAnywhere, Category = "Components")
		class USceneComponent* DefaultRoot;

	UPROPERTY(EditAnywhere, Category = "Components")
	class USceneComponent* PortalCoordinate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void SpawnPortal();

	void SpawnGallerysPlace();

	void RequestSpawn(FVector SpawnLocation, FRotator SpawnRotation);

	UPROPERTY(BlueprintReadOnly)
	class ADVPortal* ShipPortal;
	
	// ~IEventInterface
	void GetPortalSpawnTransform_Implementation(FTransform& SpawnTransform);

	void GetGallerySpawnTransform_Implementation(FTransform& SpawnTransform);

	// ~AActor Interface
	void OnRep_Owner() override;

	// ~IWallet Interface
	void FinishDownloadNft_Implementation(FNft InNft);

	virtual void SetOwner(AActor* NewOwner) override;

	UFUNCTION(BlueprintImplementableEvent)
	void EventOwnerReady();

	virtual void Destroyed() override;
public:	

private:
};
