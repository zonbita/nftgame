// Copyright Cosugames, LLC 2021

#include "Components/DVHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Core/Human/DVBaseCharacter.h"

// Sets default values for this component's properties
UDVHealthComponent::UDVHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DefaultHealth = 100.f;
	Health = DefaultHealth;
	SetIsReplicatedByDefault(true);
	// ...
}

void UDVHealthComponent::SetDefaultHealth(float InDefaultHealth)
{
	DefaultHealth = InDefaultHealth;
	Health = DefaultHealth;
	OnRep_Health(Health);
}

// Called when the game starts
void UDVHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner) {
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UDVHealthComponent::HandleTakeAnyDamage);
	}
}

void UDVHealthComponent::OnRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	IDVEventInterface::Execute_EventHealthChanged(GetOwner(), Health, Damage, nullptr, nullptr, nullptr);
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UDVHealthComponent::RequestApplyDamage(AActor* DamagedActor, float Damage, AActor* DamageCauser)
{
	ServerApplyDamage(DamagedActor, Damage, DamageCauser);
}

void UDVHealthComponent::RequestApplyDamageWithImpulse(AActor* DamagedActor, float Damage, AActor* DamageCauser, FVector ImpactLocation)
{
	ServerApplyDamageWithImpulse(DamagedActor, Damage, DamageCauser, ImpactLocation);
}

void UDVHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || DamagedActor->IsPendingKill() || Health <= 0.f) {
		return;
	}
	//AKogBaseGameMode* const MyGameMode = GetWorld()->GetAuthGameMode<AKogBaseGameMode>();
	//float OldHealth = Health;
	//if (Damage != 9999.f) // Basically just a cheat to know if it is a force kill
		//Damage = MyGameMode ? MyGameMode->ModifyDamage(Damage, GetOwner(), InstigatedBy, DamageCauser) : 0.f; // Damage is modified based on gamemode
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	OnRep_Health(Health + Damage);
	if (Health <= 0 && bApplyImpulse) {
		NetApplyImpulse(ImpactImpulse, -0.01f);
		bApplyImpulse = false;
	}
}

void UDVHealthComponent::SetHealth(float InAmount)
{
	Health += InAmount;
	Health = FMath::Clamp(Health, 0.f, DefaultHealth);
	OnRep_Health(Health - InAmount);
}

void UDVHealthComponent::RefillHealth(float InAmount)
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) {
		ServerRefillHealth(InAmount);
	}

	Health += InAmount;
	Health = FMath::Clamp(Health, 0.f, DefaultHealth);
	OnRep_Health(Health - InAmount);
}

void UDVHealthComponent::ServerRefillHealth_Implementation(float InAmount)
{
	RefillHealth(InAmount);
}

bool UDVHealthComponent::ServerRefillHealth_Validate(float InAmount)
{
	return true;
}

void UDVHealthComponent::ServerApplyDamage_Implementation(AActor* DamagedActor, float Damage, AActor* DamageCauser)
{
	UGameplayStatics::ApplyDamage(DamagedActor, Damage, nullptr, DamageCauser, UDamageType::StaticClass());
}

bool UDVHealthComponent::ServerApplyDamage_Validate(AActor* DamagedActor, float Damage, AActor* DamageCauser)
{
	return true;
}

void UDVHealthComponent::ServerApplyDamageWithImpulse_Implementation(AActor* DamagedActor, float Damage, AActor* DamageCauser, FVector ImpactLocation)
{
	bApplyImpulse = true;
	ImpactImpulse = ImpactLocation;
	UGameplayStatics::ApplyDamage(DamagedActor, Damage, nullptr, DamageCauser, UDamageType::StaticClass());
}

bool UDVHealthComponent::ServerApplyDamageWithImpulse_Validate(AActor* DamagedActor, float Damage, AActor* DamageCauser, FVector ImpactLocation)
{
	return true;
}

void UDVHealthComponent::NetApplyImpulse_Implementation(FVector ImpactLocation, float ImpulseStrength)
{
	ADVBaseCharacter* Character = Cast<ADVBaseCharacter>(GetOwner());
	if (Character) {
		Character->SimulateMesh();
		Character->GetMesh()->AddImpulse(ImpactLocation*ImpulseStrength, NAME_None, true);
	}
}

// Called every frame
void UDVHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDVHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDVHealthComponent, Health);
	DOREPLIFETIME(UDVHealthComponent, DefaultHealth);
}

