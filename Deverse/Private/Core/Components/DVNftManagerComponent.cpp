// Copyright Cosugames, LLC 2021


#include "Core/Components/DVNftManagerComponent.h"
#include "Net/UnrealNetwork.h"
#include "Core/DVWalletInterface.h"
#include "Actors/DVBaseNft.h"
#include "Core/DVBaseController.h"

// Sets default values for this component's properties
UDVNftManagerComponent::UDVNftManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}

// Called when the game starts
void UDVNftManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UDVNftManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDVNftManagerComponent::SpawnNft(FVector InLocation, FRotator InRotation, FVector InScale3D, FNft InNft, USceneComponent* HitComponent, FName BoneName, bool bCollisionTrace)
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) {
		ServerSpawnNft(InLocation, InRotation, InScale3D, InNft, HitComponent, BoneName, bCollisionTrace);
	}
	else {
		FTransform SpawnTransform;
		
		APawn* ControlledPawn = Cast<ADVBaseController>(GetOwner())->GetPawn();
		if ((ControlledPawn && ControlledPawn->IsPendingKill()) || !ControlledPawn) return;
		IDVWalletInterface::Execute_GetNftSpawnTransform(ControlledPawn, SpawnTransform);
		if (InLocation != FVector::ZeroVector) {
			SpawnTransform.SetLocation(InLocation);
		}

		if (InRotation != FRotator::ZeroRotator) {
			SpawnTransform.SetRotation(InRotation.Quaternion());
		}

		if (InScale3D != FVector::ZeroVector) {
			SpawnTransform.SetScale3D(InScale3D);
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<ADVBaseController>(GetOwner())->GetPawn();
		ADVBaseNft* Frame = GetWorld()->SpawnActor<ADVBaseNft>(NftClass, SpawnTransform, SpawnParams);
		if (Frame) {
			UDVNftComponent* NftComp = Cast<UDVNftComponent>(Frame->GetComponentByClass(UDVNftComponent::StaticClass()));
			FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, false);
			if (HitComponent) {
				Frame->AttachToComponent(HitComponent, AttachRules, BoneName);
			}
			else {
				USceneComponent* AttachComp = nullptr;
				IDVWalletInterface::Execute_GetNftAttachComponent(ControlledPawn, AttachComp);
				Frame->AttachToComponent(AttachComp, AttachRules, NAME_None);
			}
			Frame->bCollisionTracing = bCollisionTrace;
			CurrentNftActor = Frame;
			if (NftComp)
				NftComp->SetNft(InNft);
			FLiteNft LiteNft;
			LiteNft.Metadata = InNft.Metadata;
			LiteNft.Name = InNft.Name;
			LiteNft.OwnerAddress = InNft.OwnerAddress;
			LiteNft.Symbol = InNft.Symbol;
			LiteNft.TokenAddress = InNft.TokenAddress;
			LiteNft.TokenId = InNft.TokenId;
			if (NftComp) {
				NftComp->SetLiftNft(LiteNft);
				OnRep_CurrentNft();
			}
		}
	}
}

void UDVNftManagerComponent::SpawnNftAvatar(FVector InLocation, FRotator InRotation, FVector InScale3D, FLiteNft InLiteNft, USceneComponent* HitComponent, FName BoneName)
{
	if (bHasAvatar) {
		// We only need to update the avatar
		AActor* FoundActor = nullptr;
		IDVWalletInterface::Execute_GetCurrentAvatar(Cast<ADVBaseController>(GetOwner())->GetPawn(), FoundActor);
		if (ADVBaseNft* Avatar = Cast<ADVBaseNft>(FoundActor)) {
			if (UDVNftComponent* NftComp = Cast<UDVNftComponent>(Avatar->GetComponentByClass(UDVNftComponent::StaticClass()))) {
				NftComp->SetLiftNft(InLiteNft);
			}
		}
	}
	else {
		if (GetOwner()->GetLocalRole() < ROLE_Authority) {
			ServerSpawnAvatarNft(InLocation, InRotation, InScale3D, InLiteNft, HitComponent, BoneName);
		}
		else {
			APawn* ControlledPawn = Cast<ADVBaseController>(GetOwner())->GetPawn();
			if ((ControlledPawn && ControlledPawn->IsPendingKill()) || !ControlledPawn) return;
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(InLocation);
			SpawnTransform.SetRotation(InRotation.Quaternion());
			SpawnTransform.SetScale3D(InScale3D);

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = Cast<ADVBaseController>(GetOwner())->GetPawn();
			ADVBaseNft* Frame = GetWorld()->SpawnActor<ADVBaseNft>(NftClass, SpawnTransform, SpawnParams);
			if (Frame) {
				FAttachmentTransformRules AttachRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false);
				if (HitComponent) {
					Frame->AttachToComponent(HitComponent, AttachRules, BoneName);
				}
				Frame->bCollisionTracing = false;
				Frame->Tags.Add(TEXT("avatar"));
				CurrentNftActor = Frame;
				if (UDVNftComponent* NftComp = Cast<UDVNftComponent>(Frame->GetComponentByClass(UDVNftComponent::StaticClass()))) {
					NftComp->SetLiftNft(InLiteNft);
					OnRep_CurrentNft();
				}
				bHasAvatar = true;
				OnRep_Avatar();
			}
		}
	}
}

void UDVNftManagerComponent::ServerSpawnAvatarNft_Implementation(FVector InLocation, FRotator InRotation, FVector InScale3D, FLiteNft InLiteNft, USceneComponent* HitComponent, FName BoneName)
{
	SpawnNftAvatar(InLocation, InRotation, InScale3D, InLiteNft, HitComponent, BoneName);
}

bool UDVNftManagerComponent::ServerSpawnAvatarNft_Validate(FVector InLocation, FRotator InRotation, FVector InScale3D, FLiteNft InLiteNft, USceneComponent* HitComponent, FName BoneName)
{
	return true;
}

FNft UDVNftManagerComponent::GetCurrentSelectedNft()
{
	return CurrentSelectedNft;
}

void UDVNftManagerComponent::SetCurrentNft(FNft InNft)
{
	CurrentSelectedNft = InNft;
}

void UDVNftManagerComponent::ServerSpawnNft_Implementation(FVector InLocation, FRotator InRotation, FVector InScale3D, FNft InNft, USceneComponent* HitComponent, FName BoneName, bool bCollisionTrace)
{
	SpawnNft(InLocation, InRotation, InScale3D, InNft, HitComponent, BoneName, bCollisionTrace);
}

bool UDVNftManagerComponent::ServerSpawnNft_Validate(FVector InLocation, FRotator InRotation, FVector InScale3D, FNft InNft, USceneComponent* HitComponent, FName BoneName, bool bCollisionTrace)
{
	return true;
}

void UDVNftManagerComponent::OnRep_CurrentNft()
{
	IDVWalletInterface::Execute_OnNftSpawn(GetOwner(), CurrentNftActor);
}

void UDVNftManagerComponent::OnRep_Avatar()
{
	// TODO this will not be called on other clients
	IDVWalletInterface::Execute_UpdateAvatar(GetOwner(), bHasAvatar);
}

void UDVNftManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDVNftManagerComponent, CurrentNftActor);
	DOREPLIFETIME(UDVNftManagerComponent, bHasAvatar);
}