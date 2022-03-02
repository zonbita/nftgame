// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DVParty.generated.h"

UCLASS()
class DEVERSE_API ADVParty : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADVParty();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
