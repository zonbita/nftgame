// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DVBaseActor.h"
#include "Deverse/Deverse.h"
#include "Core/DVWalletInterface.h"
#include "DVBaseNft.generated.h"

UCLASS()
class DEVERSE_API ADVBaseNft : public ADVBaseActor, public IDVWalletInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADVBaseNft();

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CollisionTracing)
		bool bCollisionTracing = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UDVNftComponent* NftComponent;

	//void SetNft(const FNft InNft);

	//void SetLiftNft(const FLiteNft InLiteNft);

	UFUNCTION(BlueprintImplementableEvent)
	void NftDataUpdated();

	void SetCollisionTracing(bool bWantsToTrace);

	UFUNCTION()
	void HandleNftUpdate();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintReadWrite)
	//FNft OwningNft;
	
	//UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_OwningNft)
	//FLiteNft LiteNftData;

	//UFUNCTION()
	//virtual void OnRep_OwningNft();

	UFUNCTION()
	virtual void OnRep_CollisionTracing();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
