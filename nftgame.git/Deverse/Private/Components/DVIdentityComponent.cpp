// Copyright Cosugames, LLC 2021


#include "Components/DVIdentityComponent.h"
#include "Net/UnrealNetwork.h"
#include "Core/DVEventInterface.h"

// Sets default values for this component's properties
UDVIdentityComponent::UDVIdentityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}

// Called when the game starts
void UDVIdentityComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UDVIdentityComponent::SetId(const FString NewId)
{
	Id = NewId;
	OnRep_Id();
}

void UDVIdentityComponent::OnRep_Id()
{
	// Should not call this
	IDVEventInterface::Execute_UpdateActorId(GetOwner(), Id);
}

void UDVIdentityComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDVIdentityComponent, Id);
}