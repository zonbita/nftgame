// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/DVEventInterface.h"
#include "DVBaseActor.generated.h"

UCLASS()
class DEVERSE_API ADVBaseActor : public AActor, public IDVEventInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADVBaseActor();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UDVIdentityComponent* IdentityComponent;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "DV Actor")
	FString GetId();

	UPROPERTY(EditAnywhere, Category = "Attributes")
	bool bCanBeInteract;

	// IDVEventInterface
	void CanBeInteracted_Implementation(bool& OutInteract);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
