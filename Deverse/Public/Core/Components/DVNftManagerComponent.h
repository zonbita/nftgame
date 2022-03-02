// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Deverse/Deverse.h"
#include "DVNftManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FFrameEquipment {
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, Category = "Frame Equipment")
	class AActor* Frame;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Equipment")
		FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Equipment")
		FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "Frame Equipment")
		FVector Scale;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEVERSE_API UDVNftManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDVNftManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<class ADVBaseNft> NftClass;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<FFrameEquipment> AttachedFrames;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentNft)
	class AActor* CurrentNftActor;

	UPROPERTY(ReplicatedUsing = OnRep_Avatar)
	bool bHasAvatar = false;

	UFUNCTION()
	void OnRep_CurrentNft();

	UFUNCTION()
	void OnRep_Avatar();

	UFUNCTION(BlueprintCallable, Category = "Nft Manager")
	void SpawnNft(FVector InLocation, FRotator InRotation, FVector InScale3D, FNft InNft, USceneComponent* HitComponent, FName BoneName, bool bCollisionTrace);

	UFUNCTION(BlueprintCallable, Category = "Nft Manager")
		void SpawnNftAvatar(FVector InLocation, FRotator InRotation, FVector InScale3D, FLiteNft InLiteNft, USceneComponent* HitComponent, FName BoneName);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerSpawnAvatarNft(FVector InLocation, FRotator InRotation, FVector InScale3D, FLiteNft InLiteNft, USceneComponent* HitComponent, FName BoneName);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnNft(FVector InLocation, FRotator InRotation, FVector InScale3D, FNft InNft, USceneComponent* HitComponent, FName BoneName, bool bCollisionTrace);

	UFUNCTION(BlueprintPure, Category = "Nft Manager")
	FNft GetCurrentSelectedNft();
	
	UFUNCTION(BlueprintCallable, Category = "Nft Manager")
	void SetCurrentNft(FNft InNft);
protected:
	FNft CurrentSelectedNft;

private:
};
