// Copyright Cosugames, LLC 2021


#include "Actors/DVFrame.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DVNftComponent.h"
#include "Core/Human/DVBaseCharacter.h"
#include "Net/UnrealNetwork.h"

ADVFrame::ADVFrame()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADVFrame::BeginPlay()
{
	Super::BeginPlay();
	OnRep_CollisionTracing();
}

void ADVFrame::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCollisionTracing) {
		AddRotate(DeltaTime);
		if (ADVBaseCharacter* OwningChar = Cast<ADVBaseCharacter>(GetInstigator())) {
			FVector CameraFV;
			FVector CameraLoc;
			bool bFPActive;
			OwningChar->GetActiveCameraTransform(CameraFV, CameraLoc, bFPActive);
			if (bFPActive) {
				Trace(CameraLoc, CameraLoc + CameraFV * 500.f);
			}
			else {
				Trace(CameraLoc, CameraLoc + CameraFV * 800.f);
			}
		}
	}
}

void ADVFrame::EventTrigger_Implementation(FCustomEvent InEvent)
{

}

void ADVFrame::EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent)
{

}

void ADVFrame::RequestUpdateNftDisplay_Implementation(FLiteNft InNft)
{
	UpdateNftDisplay(InNft);
}

void ADVFrame::UpdateNftDisplay(FLiteNft InLiteNft)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerUpdateNftDisplay(InLiteNft);
	}
	NftComponent->SetLiftNft(InLiteNft);
	//LiteNftData = InLiteNft;
	//OnRep_OwningNft();
}

void ADVFrame::OnRep_CollisionTracing()
{
	if (bCollisionTracing) {
	}
	else {
		MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void ADVFrame::ServerUpdateNftDisplay_Implementation(FLiteNft InLiteNft)
{
	UpdateNftDisplay(InLiteNft);
}

bool ADVFrame::ServerUpdateNftDisplay_Validate(FLiteNft InLiteNft)
{
	return true;
}