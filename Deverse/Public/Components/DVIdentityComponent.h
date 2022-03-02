// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Deverse/Deverse.h"
#include "DVIdentityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEVERSE_API UDVIdentityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDVIdentityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Id, Category = "Identity")
		FString Id;
public:
	void SetId(const FString NewId);

	FORCEINLINE const FString GetId() { return Id; }

	UFUNCTION()
	void OnRep_Id();
};
