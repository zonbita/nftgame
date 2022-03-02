// Copyright Cosugames, LLC 2021


#include "Actors/DVPortal.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Components/DVIdentityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Core/DVBaseGameInstance.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Core/DVBaseGameState.h"

// Sets default values
ADVPortal::ADVPortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DefaultRootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Default Root Scene"));
	RootComponent = DefaultRootScene;
	TeleportVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Teleport Volume Component"));
	TeleportVolume->SetupAttachment(RootComponent);
	LeaveArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Leave Arrow Component"));
	LeaveArrow->SetupAttachment(RootComponent);
	PortalParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Portal Particle Component"));
	PortalParticle->SetupAttachment(RootComponent);
	PortalAmbientSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Portal Audio Component"));
	PortalAmbientSound->SetupAttachment(RootComponent);
	OnActorBeginOverlap.AddUniqueDynamic(this, &ADVPortal::TeleportTriggerOverlap);
	OnActorEndOverlap.AddUniqueDynamic(this, &ADVPortal::TeleportTriggerEnd);
}

// Called when the game starts or when spawned
void ADVPortal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADVPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADVPortal::TeleportTrigger(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ADVPortal::TeleportEndTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bPortalActive) return;
	if (PortalUsers.Contains(OtherActor)) {
		//PortalUsers.Remove(OtherActor);
	}
}

void ADVPortal::TeleportTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bPortalActive) {
		return;
	}
	UDVBaseGameInstance* GI = Cast<UDVBaseGameInstance>(GetGameInstance());
	FString TravelId = GI->TravelId;
	FString TravelURL = GI->TravelURL;

	// First check if the portal is in a different world
	// Travel to that world first, then travel to the corresponding portal
	UE_LOG(LogTemp, Warning, TEXT("travel url %s"), *TravelURL);

	if (!GI->TravelURL.IsEmpty()) { 
		IDVEventInterface::Execute_EventTeleportVerseInitiated(OtherActor, GI->TravelURL);
		GI->TravelURL = ""; // Clear the travel URL so we dont travel again on arriving
	}
	else if (!PortalUsers.Contains(OtherActor) && !TravelId.IsEmpty()) { // TODO check for travel id if it's the same as the current portal
		if (TravelId != GetId()){
			PortalUsers.AddUnique(OtherActor);
			//DestinationPortal->PortalUsers.AddUnique(OtherActor);
			UGameplayStatics::PlaySoundAtLocation(this, TeleportSFX, GetActorLocation());
			//IDVEventInterface::Execute_EventTeleportInitiated(OtherActor, DestinationPortal->LeaveArrow->GetComponentLocation(), DestinationPortal->LeaveArrow->GetComponentRotation());
			if (OtherActor->GetClass()->ImplementsInterface(UDVEventInterface::StaticClass()) && !IDVEventInterface::Execute_IsTeleporting(OtherActor)) {
				IDVEventInterface::Execute_EventTeleportInitiatedId(OtherActor, TravelId);
			}
		}
	}
	else if (PortalUsers.Contains(OtherActor)) {
		//OtherActor->GetInstigatorController()->SetControlRotation(LeaveArrow->GetComponentRotation());
	}
}

void ADVPortal::TeleportTriggerEnd(AActor* OverlappedActor, AActor* OtherActor)
{
	if (PortalUsers.Contains(OtherActor)) {
		PortalUsers.Remove(OtherActor);
	}
	// Why is this called if noone is in portal
	if (OtherActor->GetClass()->ImplementsInterface(UDVEventInterface::StaticClass())) {
		IDVEventInterface::Execute_EventEndPortal(OtherActor);
	}
}

void ADVPortal::GetSpawnLocation(FVector& OutLocation, FRotator& OutRotation)
{
	OutLocation = LeaveArrow->GetComponentLocation();
	OutRotation = LeaveArrow->GetComponentRotation();
}

ADVPortal* ADVPortal::GetPortal(const FString PortalId)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADVPortal::StaticClass(), FoundActors);
	for (AActor* FoundActor : FoundActors) {
		UDVIdentityComponent* IdComp = Cast<UDVIdentityComponent>(FoundActor->GetComponentByClass(UDVIdentityComponent::StaticClass()));
		if (IdComp && IdComp->GetId().Equals(PortalId) && !IdComp->GetId().IsEmpty()) {
			return Cast<ADVPortal>(FoundActor);
		}
	}
	return nullptr;
}

void ADVPortal::UpdateId(const FString InNewId)
{
	IdentityComponent->SetId(InNewId);
}

void ADVPortal::SetPortalActive(bool bActive)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerSetPortalActive(bActive);
	}
	else {
		NetPortalActive(bActive);
		if (bActive && GetWorld()->GetGameState() && GetWorld()->GetGameState()->GetClass()->ImplementsInterface(UDVEventInterface::StaticClass())) {
			IDVEventInterface::Execute_EventBroadcastChat(GetWorld()->GetGameState(), "Verse", "A hidden portal is activated", EChatGroup::EAll);
		}
	}
	if (bActive)
		SetOwner(nullptr);
	bPortalActive = bActive;

}

void ADVPortal::ServerSetPortalActive_Implementation(bool bActive)
{
	SetPortalActive(bActive);
}

bool ADVPortal::ServerSetPortalActive_Validate(bool bActive)
{
	return true;
}

void ADVPortal::NetPortalActive_Implementation(bool bActive)
{
	if (bActive) {
		PortalParticle->Activate(true);
		TArray<UUserWidget*> FoundWidgets;
		UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(this, FoundWidgets, UDVEventInterface::StaticClass(), true);
		for (UUserWidget* FoundWidget : FoundWidgets) {
			IDVEventInterface::Execute_UpdatePortal(FoundWidget, GetId(), "", CustomTravelUrl);
		}
	}
	else {
		PortalParticle->Deactivate();
	}
}

void ADVPortal::UpdateActorId_Implementation(const FString& InId)
{
	/*TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsWithInterface(this, FoundWidgets, UDVEventInterface::StaticClass(), true);
	for (UUserWidget* FoundWidget : FoundWidgets) {
		IDVEventInterface::Execute_UpdatePortal(FoundWidget, InId);
	}*/
}

void ADVPortal::EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent)
{

}

void ADVPortal::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADVPortal, bPortalActive);
}
