// Copyright Cosugames, LLC 2021

#include "Core/Human/DVBaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Core/DVBaseController.h"
#include "Components/CapsuleComponent.h"
#include "Components/DVHealthComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/DVPortal.h"
#include "Components/DVIdentityComponent.h"
#include "Core/DVBaseGameState.h"
#include "DVSpaceShip.h"

FOnNewNftSpawn ADVBaseCharacter::OnNewNftSpawn;
FOnNftUnSpawn ADVBaseCharacter::OnNftUnSpawn;

// Sets default values
ADVBaseCharacter::ADVBaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ADVBaseCharacter::OnCharacterHit);
	HealthComp = CreateDefaultSubobject<UDVHealthComponent>(TEXT("Health Component"));

	FootstepsAC = CreateDefaultSubobject<UAudioComponent>(TEXT("Footsteps Audio Component"));
	FootstepsAC->SetupAttachment(RootComponent);

	JumpLandAC = CreateDefaultSubobject<UAudioComponent>(TEXT("Jump Land Audio Component"));
	JumpLandAC->SetupAttachment(RootComponent);
}

void ADVBaseCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called when the game starts or when spawned
void ADVBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultMeshLocation = GetMesh()->GetRelativeLocation();
	DefaultMeshRotation = GetMesh()->GetRelativeRotation();
}

// Called every frame
void ADVBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ADVBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("LeftMouseEvent", IE_Pressed, this, &ADVBaseCharacter::StartLeftMouseEvent);
	PlayerInputComponent->BindAction("LeftMouseEvent", IE_Released, this, &ADVBaseCharacter::StopLeftMouseEvent);
	PlayerInputComponent->BindAction("RightMouseEvent", IE_Pressed, this, &ADVBaseCharacter::StartRightMouseEvent);
	PlayerInputComponent->BindAction("RightMouseEvent", IE_Released, this, &ADVBaseCharacter::StopRightMouseEvent);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADVBaseCharacter::Interact);
}

void ADVBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	K2_OnRep_PlayerState();
}

void ADVBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	if (bDieOnFall) {
		LandingDie();
	}
}

void ADVBaseCharacter::PlaceFrame()
{

}

void ADVBaseCharacter::SelectFrame(FNft InNft)
{

}

void ADVBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	K2_OnRep_PlayerState();
}

void ADVBaseCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();
}

void ADVBaseCharacter::NotifyActorOnClicked(FKey ButtonPressed /*= EKeys::LeftMouseButton*/)
{
	ADVBaseController* PC = Cast<ADVBaseController>(GetController());

	if (PC && !PC->IsMenuActive()) {
		return;
	}

	if (PC) {
		PC->OnControlledPawnClick();
	}
}

void ADVBaseCharacter::Destroyed()
{
	Super::Destroyed();
	TArray<AActor*> OutActors;
	GetAttachedActors(OutActors);

	for (auto OutActor : OutActors) {
		if (OutActor && !OutActor->IsPendingKillPending())
			OutActor->Destroy(true);
	}
}

void ADVBaseCharacter::OnNftSpawn_Implementation(AActor* SpawnedActor)
{
	GetMesh()->IgnoreActorWhenMoving(SpawnedActor, true);
	GetCapsuleComponent()->IgnoreActorWhenMoving(SpawnedActor, true);
}

void ADVBaseCharacter::EventTeleportInitiated_Implementation(const FVector TeleportLocation, const FRotator TeleportRotation)
{
	Teleport(TeleportLocation, TeleportRotation);
}

void ADVBaseCharacter::EventTeleportInitiatedId_Implementation(const FString& TeleportId)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	TeleportWithId(TeleportId);
	bTeleporting = true; 
}

void ADVBaseCharacter::EventTeleportVerseInitiated_Implementation(const FString& VerseUrl)
{
	FTimerDelegate TeleportDelegate;
	FTimerHandle TeleportHandle;
	TeleportDelegate.BindUFunction(this, FName("TeleportVerse"), VerseUrl);
	GetWorldTimerManager().SetTimer(TeleportHandle, TeleportDelegate, 2.f, false);
}

void ADVBaseCharacter::StartLeftMouseEvent()
{
	FHitResult TraceResult = PerformLineTrace(400.f);
	if (TraceResult.bBlockingHit && TraceResult.GetActor()->Implements<UDVEventInterface>()) {
		IDVEventInterface::Execute_EventInteract(TraceResult.GetActor(), this, TraceResult.GetComponent());
	}
}

void ADVBaseCharacter::OnRep_HasAvatar()
{
	if (bHasAvatar) {
		GetMesh()->HideBoneByName(TEXT("head"), PBO_None);
	}
	else {
		GetMesh()->UnHideBoneByName(TEXT("head"));
	}
}

void ADVBaseCharacter::OnCharacterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (HealthComp->GetHealth() > 0) {
		if (IsLocallyControlled() && Cast<ADVBaseCharacter>(OtherActor)) {
			HealthComp->RequestApplyDamage(this, 100.f, this);
			//ResetPlay
			FTimerHandle RespawnHandle;
			GetWorldTimerManager().SetTimer(RespawnHandle, this, &ADVBaseCharacter::ResetPlay, 3.f);
		}
	}
}
void ADVBaseCharacter::LandingDie()
{
	if (HealthComp->GetHealth() > 0) {
		if (FMath::Clamp(UKismetMathLibrary::NormalizeToRange(UKismetMathLibrary::Abs(GetVelocity().Z), 490.f, 1600.f), 0.f, 1.f) >= 0.4f) {
			if (HasAuthority()) {
				HealthComp->RequestApplyDamage(this, 100.f, this);
				//ResetPlay
				FTimerHandle ResLocationHandle;
				GetWorldTimerManager().SetTimer(ResLocationHandle, this, &ADVBaseCharacter::ResetAtPlayerStart, 2.65f);
				FTimerHandle RespawnHandle;
				GetWorldTimerManager().SetTimer(RespawnHandle, this, &ADVBaseCharacter::ResetPlay, 3.f);
			}
		}
	}
}
void  ADVBaseCharacter::ResetAtPlayerStart() 
{
	TArray<AActor*> AllPlayerStart;
	AllPlayerStart.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllPlayerStart);
	for (AActor* IndexPlayerStart : AllPlayerStart)
	{
		auto index = UKismetMathLibrary::RandomIntegerInRange(0, AllPlayerStart.Num() - 1);
		APlayerStart* PlayerLoca = Cast<APlayerStart>(AllPlayerStart[index]);
		if (PlayerLoca != nullptr)
		{
			SetActorLocation(PlayerLoca->GetActorLocation());
		}
	}
}
void ADVBaseCharacter::EventHealthChanged_Implementation(float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health > 0) {
		StopSimulateMesh();
	}
	else {
		SimulateMesh();
	}
}

void ADVBaseCharacter::ResetPlay()
{
	HealthComp->RefillHealth(100.f);
	StopSimulateMesh();
}

FHitResult ADVBaseCharacter::PerformLineTrace(float Distance)
{
	FHitResult HitResult;
	return HitResult;
}

void ADVBaseCharacter::GetAvatarSpawnTransform_Implementation(FVector& SpawnLocation, FRotator& SpawnRotation, USceneComponent*& AttachComp, FName& BoneName)
{
	SpawnLocation = GetMesh()->GetSocketLocation("avatar");
	SpawnRotation = GetMesh()->GetSocketRotation("avatar");
	AttachComp = GetMesh();
	BoneName = TEXT("head");
}

void ADVBaseCharacter::PlayMontage(class UAnimMontage* MontageToPlay)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerPlayMontage(MontageToPlay);
	}

	NetPlayMontage(MontageToPlay);
}

int32 ADVBaseCharacter::PlayFootstepSound(FVector& HitLocation, FRotator& HitRotation)
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.f, 0.f, 200.f);
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
	int32 SurfaceIndex = 0;
	EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
	switch (SurfaceType) {
	case SURFACE_GRASS:
		SurfaceIndex = 0;
		break;
	case SURFACE_CONCRETE:
		SurfaceIndex = 1;
		break;
	case SURFACE_DIRT:
		SurfaceIndex = 2;
		break;
	case SURFACE_GRAVEL:
		SurfaceIndex = 3;
		break;
	case SURFACE_SOLIDMETAL:
		SurfaceIndex = 4;
		break;
	case SURFACE_THINMETAL:
		SurfaceIndex = 5;
		break;
	case SURFACE_GLASS:
		SurfaceIndex = 6;
		break;
	case SURFACE_WOOD:
		SurfaceIndex = 7;
		break;
	case SURFACE_WATER:
		SurfaceIndex = 8;
		break;
	case SURFACE_ROCK:
		SurfaceIndex = 9;
		break;
	}

	FootstepsAC->SetIntParameter(FName("Surface"), SurfaceIndex);
	FootstepsAC->Play();
	HitLocation = HitResult.Location;
	HitRotation = HitResult.Normal.Rotation();
	return SurfaceIndex;
}

int32 ADVBaseCharacter::PlayJumpLand(FVector& HitLocation, FRotator& HitRotation)
{
	FVector StartLocation = GetActorLocation();
	FVector EndLocation = StartLocation - FVector(0.f, 0.f, 200.f);
	FHitResult HitResult;
	UKismetSystemLibrary::LineTraceSingle(this, StartLocation, EndLocation, ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true);
	int32 SurfaceIndex = 0;
	EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(HitResult);
	switch (SurfaceType) {
	case SURFACE_GRASS:
		SurfaceIndex = 0;
		break;
	case SURFACE_CONCRETE:
		SurfaceIndex = 1;
		break;
	case SURFACE_DIRT:
		SurfaceIndex = 2;
		break;
	case SURFACE_GRAVEL:
		SurfaceIndex = 3;
		break;
	case SURFACE_SOLIDMETAL:
		SurfaceIndex = 4;
		break;
	case SURFACE_THINMETAL:
		SurfaceIndex = 5;
		break;
	case SURFACE_GLASS:
		SurfaceIndex = 6;
		break;
	case SURFACE_WOOD:
		SurfaceIndex = 7;
		break;
	case SURFACE_WATER:
		SurfaceIndex = 8;
		break;
	case SURFACE_ROCK:
		SurfaceIndex = 9;
		break;
	}

	JumpLandAC->SetIntParameter(FName("Surface"), SurfaceIndex);
	JumpLandAC->Play();
	HitLocation = HitResult.Location;
	HitRotation = HitResult.Normal.Rotation();
	return SurfaceIndex;
}

void ADVBaseCharacter::ServerPlayMontage_Implementation(class UAnimMontage* MontageToPlay)
{
	PlayMontage(MontageToPlay);
}

bool ADVBaseCharacter::ServerPlayMontage_Validate(class UAnimMontage* MontageToPlay)
{
	return true;
}

void ADVBaseCharacter::NetPlayMontage_Implementation(class UAnimMontage* MontageToPlay)
{
	if (GetMesh()->GetAnimInstance())
		GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
}

void ADVBaseCharacter::Teleport(const FVector TeleportLocation, const FRotator TeleportRotation)
{
	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled()) {
		ServerTeleport(TeleportLocation, TeleportRotation);
	}
	else {
		FTimerDelegate TeleportDelegate;
		FTimerHandle TeleportHandle;
		TeleportDelegate.BindUFunction(this, TEXT("FinishTeleport"), TeleportLocation, TeleportRotation);
		GetWorldTimerManager().SetTimer(TeleportHandle, TeleportDelegate, 2.f, false);
	}
}

void ADVBaseCharacter::TeleportWithId(const FString& InId)
{
	if (GetLocalRole() < ROLE_Authority && IsLocallyControlled()) {
		ServerTeleportWithId(InId);
	}
	else {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADVPortal::StaticClass(), FoundActors); 
		for (AActor* FoundActor : FoundActors) {
			UDVIdentityComponent* IdComp = Cast<UDVIdentityComponent>(FoundActor->GetComponentByClass(UDVIdentityComponent::StaticClass()));
			if (IdComp && IdComp->GetId().Equals(InId) && !IdComp->GetId().IsEmpty()) {
				ADVPortal* DestinationPortal = Cast<ADVPortal>(FoundActor);
				if (DestinationPortal) {
					FVector SpawnLocation;
					FRotator SpawnRotation;
					DestinationPortal->GetSpawnLocation(SpawnLocation, SpawnRotation);
					GetCharacterMovement()->SetMovementMode(MOVE_Flying);
					GetCharacterMovement()->StopMovementImmediately();
					Teleport(SpawnLocation, SpawnRotation);
					return;
				}
			}
		}
	}
}

void ADVBaseCharacter::TeleportVerse(const FString& VerseUrl)
{
	FString FinalUrl = FString::Printf(TEXT("open %s"), *VerseUrl);
	UE_LOG(LogTemp, Warning, TEXT("url %s"), *FinalUrl); 
	GetController<APlayerController>()->ConsoleCommand(FinalUrl, true);
}

void ADVBaseCharacter::FinishTeleport(const FVector TeleportLocation, const FRotator TeleportRotation)
{
	SetActorLocation(TeleportLocation, false, nullptr, ETeleportType::TeleportPhysics);
	SetActorRotation(TeleportRotation);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	OnFinishTeleport(this, TeleportLocation, TeleportRotation);
}

void ADVBaseCharacter::ServerTeleportWithId_Implementation(const FString& InId)
{
	TeleportWithId(InId);
}

bool ADVBaseCharacter::ServerTeleportWithId_Validate(const FString& InId)
{
	return true;
}

void ADVBaseCharacter::ServerTeleport_Implementation(const FVector TeleportLocation, const FRotator TeleportRotation)
{
	Teleport(TeleportLocation, TeleportRotation);
}

bool ADVBaseCharacter::ServerTeleport_Validate(const FVector TeleportLocation, const FRotator TeleportRotation)
{
	return true;
}

void ADVBaseCharacter::ClientTeleport_Implementation(ADVPortal* Portal, FVector TeleportLocation, const FRotator TeleportRotation)
{
	SetActorLocation(TeleportLocation);
	SetActorRotation(TeleportRotation);
}

void ADVBaseCharacter::OnFinishTeleport_Implementation(AActor* InActor, const FVector TeleportLocation, const FRotator TeleportRotation)
{
	IDVEventInterface::Execute_EventTeleportDone(InActor);
	if (GetInstigatorController())
		GetInstigatorController()->SetControlRotation(TeleportRotation);
	SetActorRotation(TeleportRotation);
}

bool ADVBaseCharacter::IsTeleporting_Implementation()
{
	return bTeleporting;
}

void ADVBaseCharacter::EventEndPortal_Implementation()
{
	bTeleporting = false;
}

void ADVBaseCharacter::UpdateAvatar_Implementation(bool HasAvatar)
{
	this->bHasAvatar = HasAvatar;
	OnRep_HasAvatar();
}

void ADVBaseCharacter::GetCurrentAvatar_Implementation(AActor*& Avatar)
{
	TArray<AActor*> FoundActors;
	GetAttachedActors(FoundActors, true);
	for (AActor* FoundActor : FoundActors) {
		if (FoundActor->ActorHasTag(TEXT("avatar"))) {
			Avatar = FoundActor;
			break;
		}
	}
}

void ADVBaseCharacter::SimulateMesh()
{
	if (GetMesh()->IsSimulatingPhysics()) return;
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->StopMovementImmediately();
	GetMesh()->WakeAllRigidBodies();
}

void ADVBaseCharacter::StopSimulateMesh()
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->ResetAllBodiesSimulatePhysics();
	GetMesh()->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(DefaultMeshLocation, DefaultMeshRotation);
}

void ADVBaseCharacter::Interact()
{
	FVector StartLocationV;
	FVector ForwardV;
	bool bFPActive = false;
	GetActiveCameraTransform(ForwardV, StartLocationV, bFPActive);
	FHitResult TraceResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	float TraceDistance = bFPActive ? 200.f : 600.f;
	GetWorld()->LineTraceSingleByChannel(TraceResult, StartLocationV, StartLocationV + ForwardV * TraceDistance, ECC_Camera, QueryParams);
	if (TraceResult.GetActor() && (!TraceResult.GetActor()->GetOwner() || TraceResult.GetActor()->GetOwner() == this)) {
		bool bCanInteract = false;
		if (TraceResult.GetActor()->GetClass()->ImplementsInterface(UDVEventInterface::StaticClass()))
			IDVEventInterface::Execute_CanBeInteracted(TraceResult.GetActor(), bCanInteract);
		if (bCanInteract) {
			if (!TraceResult.GetActor()->GetOwner()) {
				OwnActor(TraceResult.GetActor(), this);
			}
			else if (TraceResult.GetActor()->GetOwner() == this){ // Stop interacting
				OwnActor(TraceResult.GetActor(), nullptr);
			}
			IDVEventInterface::Execute_EventInteract(TraceResult.GetActor(), this, TraceResult.GetComponent());
		}
	}
}

void ADVBaseCharacter::OwnActor(AActor* InActor, AActor* NewOwner)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerOwnActor(InActor, NewOwner);
	}
	if (Cast<APawn>(NewOwner)) // For now instigator is the same as owner
		InActor->SetInstigator(Cast<APawn>(NewOwner));
	InActor->SetOwner(NewOwner);
}

void ADVBaseCharacter::ServerOwnActor_Implementation(AActor* InActor, AActor* NewOwner)
{
	OwnActor(InActor, NewOwner);
}

bool ADVBaseCharacter::ServerOwnActor_Validate(AActor* InActor, AActor* NewOwner)
{
	return true;
}

void ADVBaseCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADVBaseCharacter, bHasAvatar);
	DOREPLIFETIME_CONDITION(ADVBaseCharacter, bTeleporting, COND_OwnerOnly);

}
