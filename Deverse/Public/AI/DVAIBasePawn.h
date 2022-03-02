// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DVAIBasePawn.generated.h"

UCLASS()
class DEVERSE_API ADVAIBasePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADVAIBasePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
