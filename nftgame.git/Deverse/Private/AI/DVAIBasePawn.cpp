// Copyright Cosugames, LLC 2021


#include "AI/DVAIBasePawn.h"

// Sets default values
ADVAIBasePawn::ADVAIBasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADVAIBasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADVAIBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADVAIBasePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

