// Copyright Cosugames, LLC 2021


#include "DVBaseActor.h"
#include "Components/DVIdentityComponent.h"

// Sets default values
ADVBaseActor::ADVBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	IdentityComponent = CreateDefaultSubobject<UDVIdentityComponent>(TEXT("Identity component"));
}

// Called when the game starts or when spawned
void ADVBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

FString ADVBaseActor::GetId()
{
	return IdentityComponent->GetId();
}

void ADVBaseActor::CanBeInteracted_Implementation(bool& OutInteract)
{
	OutInteract = bCanBeInteract;
}

// Called every frame
void ADVBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

