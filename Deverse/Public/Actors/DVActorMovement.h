// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DVActorMovement.generated.h"

UCLASS()
class ADVActorMovement : public AActor
{
	GENERATED_BODY()
	void CheckmultiCollision(FVector const& Target, bool& Overlap);
	void CheckOwnerOverlap(FVector const& Target, bool& Overlap);
	void SetObjectLocation(FVector Target);
	void Calculator();
	void SetDynamicInstance();
	void SetColor(bool Green);
	
	bool CollisionTracing;
	float Yaw, Pitch;
	FVector Location, OldLocation, Normal, BoxExtent, StartTrace, EndTrace;
	TArray<FHitResult> Hits;
	TArray<AActor*> Actor;
	TArray<TEnumAsByte<EObjectTypeQuery> > Objects;
	
public:	
	// Sets default values for this actor's properties
	ADVActorMovement();

	UPROPERTY(Editanywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* MaterialInstanceDynamic;
	
	UPROPERTY(Editanywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

	UFUNCTION(BlueprintCallable, meta=(Category="Default", OverrideNativeName="Set Rotate"))
	void AddRotate(float Delta);
	
	UFUNCTION(BlueprintCallable, meta=(Category="Default", OverrideNativeName="Trace"))
	void Trace(FVector Start, FVector End);

	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="Rotate Yaw"))
	void RotateYaw(float value);
	
	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="Rotate Pitch"))
	void RotatePitch(float value);
	
	UFUNCTION(BlueprintCallable, meta=(Category="Default", OverrideNativeName="Server_setLocation"))
	virtual void ServerSetLocation(FTransform const& Transform, FTransform& OutTransform, bool& Place);
	
	UFUNCTION(BlueprintCallable)
	void SetLocation(FTransform Transform);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, ReplicatedUsing="OnRep_ActorTransform")
	FTransform ActorTransforms;

	UFUNCTION(BlueprintCallable, meta=(Category, OverrideNativeName="OnRep_ActorTransform"))
	virtual void OnRep_ActorTransform();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
