// Copyright Cosugames, LLC 2021

#pragma once
#include "Interfaces/OnlineIdentityInterface.h"
#include "CoreMinimal.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "GameFramework/Character.h"
#include "Core/DVEventInterface.h"
#include "Core/DVWalletInterface.h"
#include "Deverse/Deverse.h"
#include "DVBaseCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNewNftSpawn, class ADVBaseCharacter*, class ADVDog*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnNftUnSpawn, class ADVBaseCharacter*, class ADVDog*);

UCLASS()
class DEVERSE_API ADVBaseCharacter : public ACharacter, public IDVEventInterface, public IDVWalletInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADVBaseCharacter();

	// VoiceChat
	virtual void BeginDestroy() override;

	static FOnNewNftSpawn OnNewNftSpawn;
	
	static FOnNftUnSpawn OnNftUnSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	bool bDieOnFall;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_HasAvatar)
	bool bHasAvatar = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UDVHealthComponent* HealthComp;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAudioComponent* FootstepsAC;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAudioComponent* JumpLandAC;

	UFUNCTION(BlueprintCallable, Category = "Deverse|Character")
	void PlaceFrame();

	UFUNCTION(BlueprintCallable, Category = "Deverse|Character")
		void SelectFrame(FNft InNft);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Player State Ready"))
		void K2_OnRep_PlayerState();

	// ~AActor interface
	virtual void OnRep_PlayerState() override;
	virtual void NotifyControllerChanged() override;
	virtual void NotifyActorOnClicked(FKey ButtonPressed = EKeys::LeftMouseButton) override;
	virtual void Destroyed() override;
	virtual void PossessedBy(AController* NewController) override;

	// ~ACharacter interface

	virtual void Landed(const FHitResult& Hit) override;
	
	// ~IDVWalletInterface
	void OnNftSpawn_Implementation(AActor* SpawnedActor);
	void UpdateAvatar_Implementation(bool HasAvatar);
	void GetCurrentAvatar_Implementation(AActor*& Avatar);

	// ~IDEventInterface
	void EventTeleportInitiated_Implementation(const FVector TeleportLocation, const FRotator TeleportRotation);
	void EventTeleportInitiatedId_Implementation(const FString& TeleportId);
	void EventTeleportVerseInitiated_Implementation(const FString& VerseUrl);
	bool IsTeleporting_Implementation();
	void EventEndPortal_Implementation();
	////////////////////

	void Teleport(const FVector TeleportLocation, const FRotator TeleportRotation);

	UFUNCTION(Client, Reliable)
	void ClientTeleport(ADVPortal* Portal, FVector TeleportLocation, const FRotator TeleportRotation);

	void TeleportWithId(const FString& InId);

	UFUNCTION()
	void TeleportVerse(const FString& VerseUrl);

	UFUNCTION()
	void FinishTeleport(const FVector TeleportLocation, const FRotator TeleportRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerTeleport(const FVector TeleportLocation, const FRotator TeleportRotation);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerTeleportWithId(const FString& InId);

	UFUNCTION(NetMulticast, Reliable)
		void OnFinishTeleport(AActor* InActor, const FVector TeleportLocation, const FRotator TeleportRotation);

	virtual void SimulateMesh();

	virtual void StopSimulateMesh();

	virtual void Interact();
	
	//virtual void OnLanded(const FHitResult& Hit) override ;

	void OwnActor(AActor* InActor, AActor* NewOwner);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOwnActor(AActor* InActor, AActor* NewOwner);

	UFUNCTION(BlueprintPure, Category = "Deverse|Character")
	virtual void GetActiveCameraTransform(FVector& OutForwardV, FVector& OutLocation, bool& IsFPActive) {};
protected:
	virtual void StartLeftMouseEvent();

	virtual void StopLeftMouseEvent() {};

	virtual void StartRightMouseEvent() {};

	virtual void StopRightMouseEvent() {};

	UFUNCTION()
	void OnRep_HasAvatar();

	UFUNCTION()
	void OnCharacterHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void EventHealthChanged_Implementation(float Health, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void LandingDie();

	virtual void ResetAtPlayerStart();
	virtual void ResetPlay();

	virtual FHitResult PerformLineTrace(float Distance);

	// ~Wallet interface
	void GetAvatarSpawnTransform_Implementation(FVector& SpawnLocation, FRotator& SpawnRotation, USceneComponent*& AttachComp, FName& BoneName);
	
	UFUNCTION(BlueprintCallable, Category = "Deverse|Character")
	void PlayMontage(class UAnimMontage* MontageToPlay);

	UFUNCTION(NetMulticast, Unreliable)
	void NetPlayMontage(class UAnimMontage* MontageToPlay);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPlayMontage(class UAnimMontage* MontageToPlay);

	UFUNCTION(BlueprintCallable)
		int32 PlayFootstepSound(FVector& HitLocation, FRotator& HitRotation);

	UFUNCTION(BlueprintCallable)
	int32 PlayJumpLand(FVector& HitLocation, FRotator& HitRotation);
private:
	FVector DefaultMeshLocation;

	FRotator DefaultMeshRotation;

	UPROPERTY(Replicated)
		bool bTeleporting = false;
};
