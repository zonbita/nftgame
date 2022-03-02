// Copyright Cosugames, LLC 2021


#include "Components/DVNftComponent.h"

// Sets default values for this component's properties
UDVNftComponent::UDVNftComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

// Called when the game starts
void UDVNftComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UDVNftComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDVNftComponent::SetNft(const FNft InNft)
{
	OwningNft = InNft;
}

void UDVNftComponent::SetLiftNft(const FLiteNft InLiteNft)
{
	LiteNftData = InLiteNft;
	OnRep_OwningNft();
}

void UDVNftComponent::OnRep_OwningNft()
{
	OnNftUpdate.Broadcast();
}

void UDVNftComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDVNftComponent, LiteNftData);
}
