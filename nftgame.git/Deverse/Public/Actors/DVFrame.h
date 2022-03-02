// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Actors/DVBasePlaceActor.h"
#include "Deverse/Deverse.h"
#include "DVFrame.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API ADVFrame : public ADVBasePlaceActor
{
	GENERATED_BODY()
	
public:
	ADVFrame();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite)
	class ADVBaseCharacter* OwningCharacter;

	void EventTrigger_Implementation(FCustomEvent InEvent);

	void EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent);

	void RequestUpdateNftDisplay_Implementation(FLiteNft InNft);

	void UpdateNftDisplay(FLiteNft InLiteNft);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateNftDisplay(FLiteNft InNft);

	virtual void OnRep_CollisionTracing() override;
};
