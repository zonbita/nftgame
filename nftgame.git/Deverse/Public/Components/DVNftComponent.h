// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Deverse/Deverse.h"
#include "DVNftComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNftUpdate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEVERSE_API UDVNftComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDVNftComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetNft(const FNft InNft);

	void SetLiftNft(const FLiteNft InLiteNft);

	UPROPERTY(BlueprintAssignable)
	FOnNftUpdate OnNftUpdate;

	UPROPERTY(BlueprintReadWrite)
		FNft OwningNft;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing = OnRep_OwningNft)
		FLiteNft LiteNftData;

	UFUNCTION()
		virtual void OnRep_OwningNft();
};
