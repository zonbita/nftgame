// Copyright Cosugames, LLC 2021

#pragma once
#include "Net/UnrealNetwork.h"
#include "CoreMinimal.h"
#include "DVBaseActor.h"
#include "Actors/DVBasePlaceActor.h"
#include "Core/Human/DVBaseCharacter.h"
#include "Deverse/Deverse.h"
#include "Core/DVWalletInterface.h"
#include "AI/DVDog.h"
#include "DVHuman.generated.h"


UCLASS()
class DEVERSE_API ADVHuman : public ADVBaseCharacter
{
	GENERATED_BODY()
	/** Spring Arm */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FPCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* Scene;

public:
	ADVHuman();

	/** Base turn rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

protected:
	virtual void BeginPlay() override;

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handle spring arm zoom */
	void Zoom(float Value);

	/** Toggle TP and FP camera */
	UFUNCTION(BlueprintCallable)
	void ToggleCamera();

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetActiveCameraTransform(FVector& OutForwardV, FVector& OutLocation, bool& IsFPActive) override;

	virtual void StartRightMouseEvent() override;
	// Modify speed
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float DefaultSpeed = 200.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
		float RunSpeed = 600.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	float ZoomSpeed = 100.f;

	void Run();

	void StopRun();

	void HCrouch();
	
	void SetSpeed(float speed);
	
	void UpdateActivity(ECharacterActivity InActivity);
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateActivity(ECharacterActivity InActivity);

	UFUNCTION(BlueprintImplementableEvent)
	void OnControllerReady();
	
	virtual void OnNftSpawn_Implementation(AActor* SpawnedNft) override;;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SetLocationPictureFrame(FTransform Transform);
	
	// ADVBaseCharacter interface
	virtual FHitResult PerformLineTrace(float Distance) override;

	// AActor interface
	virtual void Destroyed() override;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

    UPROPERTY(ReplicatedUsing=OnRep_Activity, EditAnywhere, BlueprintReadWrite)
    ECharacterActivity E_Activity;
	
	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="OnRep_Activity"))
	virtual void OnRep_Activity();

	// Picture Frame
	UFUNCTION(BlueprintCallable)
	void Spawn_PictureFrame();	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADVBasePlaceActor> FrameClass;
	
	UFUNCTION(BlueprintCallable)
	void SpawnPet(TSubclassOf<class ADVDog> InClass );
	UFUNCTION(BlueprintCallable)
	void DespawnPet();

	UFUNCTION(Server,Reliable, WithValidation)
	void ServerSpawnPet(TSubclassOf<class ADVDog> InClass);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDespawnPet();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ADVDog> PetToSpawn ;
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PetRef)
	ADVDog* PetRef;

	UFUNCTION()
	void OnRep_PetRef();

	UPROPERTY(ReplicatedUsing=OnRep_NftFrame, EditAnywhere, BlueprintReadWrite)
	ADVBasePlaceActor* NftFrame;

	UFUNCTION(BlueprintCallable)
	virtual void OnRep_NftFrame();

	void SetNull_PictureFrame();

	UFUNCTION(BlueprintCallable)
	void RotateYaw_PictureFrame(float value);

	UFUNCTION(BlueprintCallable)
		void RotatePitch_PictureFrame(float Value);

	UFUNCTION(BlueprintCallable)
	void SetLocation_PictureFrame();
	
	void LockRotation_PictureFrame(const FRotator NewRotation);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLockRotation_PictureFrame(const FRotator NewRotation);
	
};
