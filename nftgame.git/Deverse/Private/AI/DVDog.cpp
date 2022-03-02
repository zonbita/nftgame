// Copyright Cosugames, LLC 2021

#include "AI/DVDog.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ADVDog::ADVDog()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADVDog::BeginPlay()
{
	Super::BeginPlay();
	Create();
}

void ADVDog::Create()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
	
	 if (AnimInstance)
	 {
	 	PlayAnimMontage(AnimMontage, 1.f);
	 	float DelayTime = AnimInstance->UAnimInstance::Montage_Play(AnimMontage,
	 		1.000000,
	 		EMontagePlayReturnType::MontageLength,
	 		0.000000,
	 		true);
	 }
}

void ADVDog::Despawn()
{
	if(GetController())
	{
		GetController()->SetLifeSpan(5.f);
		GetController()->UnPossess();
		FVector Loc = GetActorLocation() + GetActorForwardVector();
		UKismetSystemLibrary::MoveComponentTo(GetRootComponent(),
			FVector(Loc),
			FRotator(0.000000,0.000000,0.000000),
			false, false, 1.000000, false,
			EMoveComponentAction::Move,
			FLatentActionInfo(8, 1882238094, TEXT(""),
			this));
		SetLifeSpan(1);
	}
	else {
		Destroy(true, true);
	}
}

void ADVDog::SetBehavior()
{
	AAIController* AIPC = Cast<AAIController>(GetController());
	if (AIPC)
	{
		if (BehaviorTree) {
			AIPC->RunBehaviorTree(BehaviorTree);
		}
	}
}

// Called to bind functionality to input
void ADVDog::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADVDog::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetBehavior();
}

void ADVDog::PlayMontage(class UAnimMontage* MontageToPlay)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerPlayMontage(MontageToPlay);
	}

	NetPlayMontage(MontageToPlay);
}

void ADVDog::ServerPlayMontage_Implementation(class UAnimMontage* MontageToPlay)
{
	PlayMontage(MontageToPlay);
}

bool ADVDog::ServerPlayMontage_Validate(class UAnimMontage* MontageToPlay)
{
	return true;
}

void ADVDog::NetPlayMontage_Implementation(class UAnimMontage* MontageToPlay)
{
	if (GetMesh()->GetAnimInstance())
		GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay);
}

