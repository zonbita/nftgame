// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DVDog.generated.h"

UCLASS()
class DEVERSE_API ADVDog : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADVDog();

	void Create();
	
	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="Set Behavior"))
	void SetBehavior();
		
	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="Despawn"))
	void Despawn();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAnimMontage* AnimMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	class UBehaviorTree* BehaviorTree;

	// ~ACharacterInterface
	void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "Deverse|Character")
		void PlayMontage(class UAnimMontage* MontageToPlay);

	UFUNCTION(NetMulticast, Unreliable)
		void NetPlayMontage(class UAnimMontage* MontageToPlay);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPlayMontage(class UAnimMontage* MontageToPlay);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
