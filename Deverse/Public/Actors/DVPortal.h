// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "DVBaseActor.h"
#include "DVPortal.generated.h"

UCLASS()
class DEVERSE_API ADVPortal : public ADVBaseActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADVPortal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TeleportVolume;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		class UArrowComponent* LeaveArrow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		class UParticleSystemComponent* PortalParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		class UAudioComponent* PortalAmbientSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
		class USceneComponent* DefaultRootScene;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	class ADVPortal* PairPortal;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	float DelayTime = 2.f;

	/** Use this to set a custom url to portal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
	FString CustomTravelUrl;

	bool bCanTeleport = true;

	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadOnly, Category = "Attributes")
	bool bPortalActive = true;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	class USoundBase* TeleportSFX;

public:
	UPROPERTY()
		TArray<class AActor*> PortalUsers;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void TeleportTrigger(class UPrimitiveComponent* OverlappingComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TeleportEndTrigger(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void TeleportTriggerOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void TeleportTriggerEnd(AActor* OverlappedActor, AActor* OtherActor);

	void GetSpawnLocation(FVector& OutLocation, FRotator& OutRotation);

	ADVPortal* GetPortal(const FString PortalId);

	void UpdateId(const FString InNewId);

	UFUNCTION(BlueprintCallable, Category = "Portal")
	void SetPortalActive(bool bActive);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSetPortalActive(bool bActive);

	UFUNCTION(NetMulticast, Reliable)
	void NetPortalActive(bool bActive);

	// ~IDVEventInterface
	void UpdateActorId_Implementation(const FString& InId);
	void EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent);
private:
};
