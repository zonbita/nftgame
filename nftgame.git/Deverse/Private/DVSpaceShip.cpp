// Copyright Cosugames, LLC 2021


#include "DVSpaceShip.h"
#include "DVGallerysPlace.h"
#include "Actors/DVPortal.h"
#include "Components/BoxComponent.h"
#include "Core/DVBaseGameState.h"

// Sets default values
ADVSpaceShip::ADVSpaceShip()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root Component"));
	RootComponent = DefaultRoot;

	PortalCoordinate = CreateDefaultSubobject<USceneComponent>(TEXT("Portal Coordinate Component"));
	PortalCoordinate->SetupAttachment(RootComponent);

	OverlapComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	OverlapComponent->SetupAttachment(RootComponent);
} 

// Called when the game starts or when spawned
void ADVSpaceShip::BeginPlay()
{
	Super::BeginPlay();
	SpawnPortal();
	SpawnGallerysPlace();
	if (GetOwner()) {
		// Fix owner issue		
		IDVEventInterface::Execute_EventSpaceshipSpawned(GetOwner(), this);
	}		
}

void ADVSpaceShip::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ADVSpaceShip::SpawnPortal()
{
	if (PortalClass && HasAuthority() && GetOwner()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		FTransform SpawnTransform = PortalCoordinate->GetComponentTransform();
		
		ShipPortal = GetWorld()->SpawnActor<ADVPortal>(PortalClass, SpawnTransform, SpawnParams);
		if (ShipPortal) {
			ShipPortal->UpdateId(FString::Printf(TEXT("portal_%s_ship"), *GetOwner()->GetName())); 
			// ok so apparently controller name is different on server/client
			ADVBaseGameState* GS = Cast<ADVBaseGameState>(GetWorld()->GetGameState());
			if (GS)
				GS->AddSpaceshipId(FString::Printf(TEXT("portal_%s_ship"), *GetOwner()->GetName()), this);
			ShipPortal->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			// Request player to spawn
			FVector SpawnLocation;
			FRotator SpawnRotation;
			ShipPortal->GetSpawnLocation(SpawnLocation, SpawnRotation);
			RequestSpawn(SpawnLocation, SpawnRotation);
		}
	}
}

void ADVSpaceShip::SpawnGallerysPlace()
{
	TArray<USceneComponent*> ChildComponents;
	GetRootComponent()->GetChildrenComponents(false,ChildComponents);
	TArray<USceneComponent*> ArrayGalleries;
	if(!ChildComponents.IsEmpty())
	{
		for (USceneComponent * it : ChildComponents)
		{
			if(it->ComponentHasTag(FName("Frame")))
			{
				ArrayGalleries.Add(it);
			}
		}
	}

	if (GalleryPlaceClass && HasAuthority()) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		for(int i = 0; i < ArrayGalleries.Num();i++)
		{
			FTransform SpawnTransform = ArrayGalleries[i]->GetComponentTransform();
			ADVGallerysPlace* FramePlace = GetWorld()->SpawnActor<ADVGallerysPlace>(GalleryPlaceClass, SpawnTransform, SpawnParams);
			if (FramePlace) {
				FramePlace->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void ADVSpaceShip::RequestSpawn(FVector SpawnLocation, FRotator SpawnRotation)
{
	if (GetOwner()) {
		IDVEventInterface::Execute_RequestRespawn(GetOwner(), SpawnLocation, SpawnRotation);
	}
}

void ADVSpaceShip::GetPortalSpawnTransform_Implementation(FTransform& SpawnTransform)
{
	if (ShipPortal) {
		FVector SpawnLocation;
		FRotator SpawnRotation;
		ShipPortal->GetSpawnLocation(SpawnLocation, SpawnRotation);
		SpawnTransform.SetScale3D(FVector(1.f));
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
	}
}

void ADVSpaceShip::GetGallerySpawnTransform_Implementation(FTransform& SpawnTransform)
{
	/*if (ShipGallerysPlace) {
		FVector SpawnLocation;
		FRotator SpawnRotation;
		ShipGallerysPlace->GetSpawnLocation(SpawnLocation, SpawnRotation);
		SpawnTransform.SetScale3D(FVector(1.f));
		SpawnTransform.SetLocation(SpawnLocation);
		SpawnTransform.SetRotation(SpawnRotation.Quaternion());
	}*/
}

void ADVSpaceShip::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (GetOwner()) {
		EventOwnerReady();
		IDVEventInterface::Execute_EventSpaceshipSpawned(GetOwner(), this);
	}
		
}

void ADVSpaceShip::FinishDownloadNft_Implementation(FNft InNft)
{
	TArray<AActor*> ChildrenActors;
	GetAttachedActors(ChildrenActors);
	for (AActor* ChildrenActor : ChildrenActors) {
		if (ChildrenActor->Implements<UDVWalletInterface>())
			IDVWalletInterface::Execute_FinishDownloadNft(ChildrenActor, InNft);
	}
}

void ADVSpaceShip::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	OnRep_Owner();
}

void ADVSpaceShip::Destroyed()
{
	if (ShipPortal && !ShipPortal->IsPendingKillPending()) {
		ShipPortal->Destroy(true);
	}

	TArray<AActor*> OutActors;
	GetAttachedActors(OutActors);

	for (auto OutActor : OutActors) {
		if (OutActor && !OutActor->IsPendingKillPending())
			OutActor->Destroy(true);
	}
	// Also teleport everyone back to spaceship
	TArray<AActor*> OverlappingActors;
	ADVBaseGameState* GS = Cast<ADVBaseGameState>(GetWorld()->GetGameState());
	OverlapComponent->GetOverlappingActors(OverlappingActors, ADVBaseCharacter::StaticClass());
	for (auto OverlappingActor : OverlappingActors) { // Exclude owner pawn
		for (FString ShipId : GS->AllShipPortalIds) {
			// Why is this not called fuck
			if (OverlappingActor->GetOwner() && ShipId.Contains(OverlappingActor->GetOwner()->GetName())) {
				IDVEventInterface::Execute_EventTeleportInitiatedId(OverlappingActor, ShipId);
				UE_LOG(LogTemp, Warning, TEXT("Teleport called on %s"), *OverlappingActor->GetName());
				break;
			}
		}
	}
	Super::Destroyed();
}
