// Copyright Cosugames, LLC 2021


#include "Actors/DVBaseNft.h"
#include "Net/UnrealNetwork.h"
#include "Components/DVNftComponent.h"

// Sets default values
ADVBaseNft::ADVBaseNft()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	NftComponent = CreateDefaultSubobject<UDVNftComponent>(TEXT("Nft Component"));
	NftComponent->OnNftUpdate.AddDynamic(this, &ADVBaseNft::HandleNftUpdate);
}

/*void ADVBaseNft::SetNft(const FNft InNft)
{
	OwningNft = InNft;
}

void ADVBaseNft::SetLiftNft(const FLiteNft InLiteNft)
{
	LiteNftData = InLiteNft;
	OnRep_OwningNft();
}*/

void ADVBaseNft::SetCollisionTracing(bool bWantsToTrace)
{
	bCollisionTracing = bWantsToTrace;
	OnRep_CollisionTracing();
}

void ADVBaseNft::HandleNftUpdate()
{
	NftDataUpdated();
}

// Called when the game starts or when spawned
void ADVBaseNft::BeginPlay()
{
	Super::BeginPlay();
	
}

/*void ADVBaseNft::OnRep_OwningNft()
{
	NftDataUpdated();
}*/

void ADVBaseNft::OnRep_CollisionTracing()
{
	
}

// Called every frame
void ADVBaseNft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADVBaseNft::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ADVBaseNft, LiteNftData);
	DOREPLIFETIME(ADVBaseNft, bCollisionTracing);
}
