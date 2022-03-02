// Copyright Cosugames, LLC 2021


#include "Core/Human/DVHuman.h"
#include "GeneratedCodeHelpers.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Deverse/Deverse.h"
#include "Core/DVWalletInterface.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "AI/DVDog.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ADVHuman::ADVHuman() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// replicate pawn
	bReplicates = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	// Configure character movement
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch=true; // can crounch
	MoveComp->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	MoveComp->JumpZVelocity = 600.f;
	MoveComp->AirControl = 0.2f;
	MoveComp->bCanWalkOffLedgesWhenCrouching = true; 
	MoveComp->MaxWalkSpeedCrouched = 100;

	// Enable crouching
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0, 35, 0);
	CameraBoom->TargetOffset = FVector(0, 0, 55);
	CameraBoom->bUsePawnControlRotation = true; 

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	FPCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FPCamera->SetupAttachment(GetMesh(), "head");
	FPCamera->bUsePawnControlRotation = true;
	FPCamera->bAutoActivate = false;

	// Create a Scene
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Picture Frame"));
	Scene->SetupAttachment(RootComponent);
	Scene->SetRelativeLocation(FVector(437,0.0,55.0));
	Scene->SetRelativeRotation(FRotator(0,90,0));
	
	E_Activity = ECharacterActivity::EWalk;
	GetCharacterMovement()->MaxWalkSpeed = DefaultSpeed;
}

void ADVHuman::BeginPlay()
{
	Super::BeginPlay();
}

void ADVHuman::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ADVHuman::Run);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ADVHuman::StopRun);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ADVHuman::HCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &ADVHuman::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADVHuman::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADVHuman::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADVHuman::LookUpAtRate);
	PlayerInputComponent->BindAxis("Zoom", this, &ADVHuman::Zoom);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ADVHuman::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ADVHuman::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADVHuman::OnResetVR);
	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &ADVHuman::ToggleCamera);
}

void ADVHuman::GetActiveCameraTransform(FVector& OutForwardV, FVector& OutLocation, bool& IsFPActive)
{
	if (FPCamera->IsActive()) {
		OutForwardV = FPCamera->GetForwardVector();
		OutLocation = FPCamera->GetComponentLocation();
		IsFPActive = true;
	}
	else {
		OutForwardV = FollowCamera->GetForwardVector();
		if (GetMesh()->SkeletalMesh) {
			OutLocation = GetMesh()->GetSocketLocation("head");
		}
		else {
			OutLocation = FollowCamera->GetComponentLocation();
		}
		IsFPActive = false;
	}
}

void ADVHuman::StartRightMouseEvent()
{
	if (NftFrame)
		SetLocation_PictureFrame();
}

void ADVHuman::Run()
{
	if (E_Activity == ECharacterActivity::EWalk) {
		UpdateActivity(ECharacterActivity::ERun);
	}	
}

void ADVHuman::StopRun()
{
	if (E_Activity == ECharacterActivity::ERun)
		UpdateActivity(ECharacterActivity::EWalk);
}

void ADVHuman::HCrouch()
{
	if (GetCharacterMovement()->IsFalling()) return;
	if(CanCrouch() && E_Activity != ECharacterActivity::ECrouch)
	{
		UpdateActivity(ECharacterActivity::ECrouch);
		Crouch();
	}
	else
	{
		if (E_Activity == ECharacterActivity::ECrouch) {
			UnCrouch();
			UpdateActivity(ECharacterActivity::EWalk);
		}
	}
}

void ADVHuman::OnRep_Activity()
{
	switch (E_Activity)
	{
	case ECharacterActivity::EIdle:
		break;
	case ECharacterActivity::EWalk:
		break;
	case ECharacterActivity::ERun:
		break;
	default:
		break;
	}
}

void ADVHuman::Spawn_PictureFrame()
{
	if(!NftFrame)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = GetInstigator();
		NftFrame = GetWorld()->SpawnActor<ADVBasePlaceActor>(FrameClass,Scene->K2_GetComponentLocation(), FRotator(0,0,0), SpawnParams);
		if(NftFrame)
		{
			NftFrame->SetCollisionTracing(true);
			ForceNetUpdate();
			OnRep_NftFrame();
		}
	}
}
void ADVHuman::SpawnPet(TSubclassOf<class ADVDog> InClass)
{
	if (!PetRef)
	{
		ServerSpawnPet(InClass);
	}
}
void ADVHuman::DespawnPet()
{	
	if (PetRef)
	{
		OnNftUnSpawn.Broadcast(this, PetRef);
		ServerDespawnPet();
		PetRef = nullptr;
	}
	
}
void ADVHuman::ServerDespawnPet_Implementation()
{
	PetRef->Despawn();
	PetRef = nullptr;
}

bool ADVHuman::ServerDespawnPet_Validate()
{
	return true;
}

void ADVHuman::ServerSpawnPet_Implementation(TSubclassOf<class ADVDog> InClass)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = GetInstigator();
	FVector Location = GetActorLocation();
	PetRef = GetWorld()->SpawnActor<ADVDog>(InClass, Location, FRotator(0, 0, 0), SpawnParams);
}

bool ADVHuman::ServerSpawnPet_Validate(TSubclassOf<class ADVDog> InClass)
{
	return true;
}

void ADVHuman::SetSpeed(float speed)
{
	GetCharacterMovement()->MaxWalkSpeed = speed;
}

void ADVHuman::UpdateActivity(ECharacterActivity InActivity)
{
	if (GetLocalRole() < ROLE_Authority)
		ServerUpdateActivity(InActivity);

	E_Activity = InActivity;
	
	switch (E_Activity) {
	case ECharacterActivity::EIdle:
		break;
	case ECharacterActivity::EWalk:
		SetSpeed(DefaultSpeed);
		break;
	case ECharacterActivity::ERun:
		SetSpeed(RunSpeed);
		break;
	case ECharacterActivity::ECrouch:
		SetSpeed(DefaultSpeed);
		break;
	}

	OnRep_Activity();
}


void ADVHuman::OnRep_PetRef()
{
	if (PetRef) {
		OnNewNftSpawn.Broadcast(this, PetRef);
	}
}

void ADVHuman::OnRep_NftFrame()
{
	if (IsLocallyControlled())
	{
		NftFrame->SetActorTickEnabled(true);
		ForceNetUpdate();
	}
}

void ADVHuman::SetNull_PictureFrame()
{
	if(NftFrame)
	{
		NftFrame->K2_DetachFromActor(EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld,EDetachmentRule::KeepWorld);
		NftFrame = nullptr;
		OnRep_NftFrame();
	}
}

void ADVHuman::OnNftSpawn_Implementation(AActor* SpawnedNft)
{
	Super::OnNftSpawn_Implementation(SpawnedNft);

	NftFrame = Cast<ADVBasePlaceActor>(SpawnedNft);
	if (NftFrame) {
		OnRep_NftFrame();
	}
}

void ADVHuman::RotateYaw_PictureFrame(float value)
{
	if(NftFrame)
		NftFrame->RotateYaw(value);

	if (value == 0 && NftFrame)
		LockRotation_PictureFrame(NftFrame->GetActorRotation());
}

void ADVHuman::RotatePitch_PictureFrame(float Value)
{
	if (NftFrame)
		NftFrame->RotatePitch(Value);

	if (Value == 0 && NftFrame)
		LockRotation_PictureFrame(NftFrame->GetActorRotation());
}

void ADVHuman::SetLocation_PictureFrame()
{
	if (HasAuthority())
	{
		NftFrame->SetLocation(NftFrame->GetActorTransform());
	} else
	{
		Server_SetLocationPictureFrame(NftFrame->GetActorTransform());
	}
}

void ADVHuman::LockRotation_PictureFrame(const FRotator NewRotation)
{
	if (NftFrame == nullptr) return;
	if (GetLocalRole() < ROLE_Authority)
		ServerLockRotation_PictureFrame(NewRotation);

	NftFrame->SetActorRotation(NewRotation);
}

void ADVHuman::ServerLockRotation_PictureFrame_Implementation(const FRotator NewRotation)
{
	LockRotation_PictureFrame(NewRotation);
}

bool ADVHuman::ServerLockRotation_PictureFrame_Validate(const FRotator NewRotation)
{
	return true;
}

void ADVHuman::Server_SetLocationPictureFrame_Implementation(FTransform Transform)
{
	if(NftFrame)
	{
		NftFrame->SetLocation(Transform);
	} 
}

bool ADVHuman::Server_SetLocationPictureFrame_Validate(FTransform Transform)
{
	return true;
}

FHitResult ADVHuman::PerformLineTrace(float Distance)
{
	FHitResult HitResult;
	FVector StartLocation = GetMesh()->GetSocketLocation("head");
	FVector EndLocation = StartLocation + FollowCamera->GetForwardVector() * Distance;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, QueryParams);
	return HitResult;
}

void ADVHuman::Destroyed()
{
	if (PetRef) {
		ServerDespawnPet();
	}
	Super::Destroyed();
}

void ADVHuman::ServerUpdateActivity_Implementation(ECharacterActivity InActivity)
{
	UpdateActivity(InActivity);
}

bool ADVHuman::ServerUpdateActivity_Validate(ECharacterActivity InActivity)
{
	return true;
}

void ADVHuman::OnResetVR()
{
	// If Deverse is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Deverse.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADVHuman::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ADVHuman::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ADVHuman::Zoom(float Value)
{
	ADVBaseController* PC = Cast<ADVBaseController>(GetController());
	if (PC && PC->IsMenuActive()) return;
	if (CameraBoom)
		CameraBoom->TargetArmLength = FMath::Clamp(FMath::FInterpTo(CameraBoom->TargetArmLength, CameraBoom->TargetArmLength - Value * ZoomSpeed, GetWorld()->GetDeltaSeconds(), 10.f), 150.f, 550.f);
}

void ADVHuman::ToggleCamera()
{
	if (FPCamera->IsActive()) {
		FPCamera->Deactivate();
		CameraBoom->SetActive(true);
		FollowCamera->SetActive(true);
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
	}
	else {
		FPCamera->SetActive(true);
		CameraBoom->Deactivate();
		FollowCamera->Deactivate();
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
}

void ADVHuman::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADVHuman::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ADVHuman::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ADVHuman::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}



void ADVHuman::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADVHuman, E_Activity);
	DOREPLIFETIME(ADVHuman, NftFrame);
	DOREPLIFETIME_CONDITION(ADVHuman, PetRef, COND_OwnerOnly);
}
