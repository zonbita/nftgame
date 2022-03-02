// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DVHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangeSignature, UDVHealthComponent*, HealthComp, float, Health, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEVERSE_API UDVHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDVHealthComponent();

	void SetDefaultHealth(float InDefaultHealth);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadWrite, Category = "Health Component")
		float Health;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Health Component")
		float DefaultHealth;

	UFUNCTION()
		void OnRep_Health(float OldHealth);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerApplyDamage(AActor* DamagedActor, float Damage, AActor* DamageCauser);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerApplyDamageWithImpulse(AActor* DamagedActor, float Damage, AActor* DamageCauser, FVector ImpactLocation);

	UFUNCTION(NetMulticast, Reliable)
		void NetApplyImpulse(FVector ImpactLocation, float ImpulseStrength);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RequestApplyDamage(AActor* DamagedActor, float Damage, AActor* DamageCauser);

	void RequestApplyDamageWithImpulse(AActor* DamagedActor, float Damage, AActor* DamageCauser, FVector ImpactLocation);

	UFUNCTION()
		virtual void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SetHealth(float InAmount);

	void RefillHealth(float InAmount);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerRefillHealth(float InAmount);

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangeSignature OnHealthChanged;

	FORCEINLINE float GetHealth() { return Health; }

	FORCEINLINE float GetDefaultHealth() { return DefaultHealth; }

private:
	bool bApplyImpulse = false;

	FVector ImpactImpulse;
};
