// Copyright Cosugames, LLC 2021


#include "Actors/DVBasePlaceActor.h"
#include "Core/Human/DVHuman.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/TransformCalculus3D.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
ADVBasePlaceActor::ADVBasePlaceActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ADVBasePlaceActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(true);
	SetDynamicInstance();
	if(GetInstigator() && !GetInstigator()->IsLocallyControlled())
	{
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called every frame
void ADVBasePlaceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddRotate(DeltaTime);
}

void ADVBasePlaceActor::AddRotate(float Delta)
{
	if (Yaw != 0) {
		MeshComponent->AddLocalRotation(FRotator(0, 0, (Yaw + Delta)), false, NULL, ETeleportType::None);
	}
		
	if (Pitch != 0) {
		MeshComponent->AddLocalRotation(FRotator(0, (Pitch + Delta), 0), false, NULL, ETeleportType::None);
	}
}

void ADVBasePlaceActor::RotateYaw(float value)
{
	Yaw = value;
}

void ADVBasePlaceActor::RotatePitch(float value)
{
	Pitch = value;
}

void ADVBasePlaceActor::Trace(FVector Start, FVector End)
{
	if(GetInstigator())
	{
		FVector Min, Max, Scale;
		MeshComponent->GetLocalBounds( Min, Max);
		Scale = MeshComponent->GetComponentScale();
		BoxExtent = Scale * Max;
		FHitResult Hit;
		Objects.SetNum(3, true);
		Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
		Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
		Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
		TArray<AActor*> Actors;
		Actors.Add( GetInstigator());
		bool A;
		A = UKismetSystemLibrary::LineTraceSingleForObjects(this, Start, End, Objects, false, Actors,
			EDrawDebugTrace::None, Hit, true,
			FLinearColor(1.000000,0.000000,0.000000,1.000000),
			FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
		if(A)
		{
			Location  = Hit.Location;
			Normal = Hit.Normal ;
			Calculator();
		}else
		{
			bool B;
			Location = FVector(Hit.TraceEnd.X, Hit.TraceEnd.Y, Hit.TraceEnd.Z + BoxExtent.Z);
			FVector NewLocation = FVector(Hit.TraceEnd.X, Hit.TraceEnd.Y, Hit.TraceEnd.Z - (BoxExtent.Z/1.1));
			
			CheckmultiCollision(NewLocation, B);
			if(B)
			{
				//SetObjectLocation(NewLocation);
				bool C;
				CheckOwnerOverlap(NewLocation, C);
				if(C)
				{
					bool D;
					CheckmultiCollision(NewLocation, D);
					if(D)
					{
						SetObjectLocation(Location);
						SetColor(false);
					}else
					{
						SetObjectLocation(Location);
						SetColor(true);
					}
				}else SetObjectLocation(Location);
			}else
			{
				SetObjectLocation(Location);
			}
		}
	}
}

void ADVBasePlaceActor::ServerSetLocation(FTransform const& Transform, FTransform& OutTransform, bool& Place)
{
	Objects.SetNum(3, true);
	Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
	Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
	Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
	FVector Min, Max, Scale;
	MeshComponent->GetLocalBounds( Min, Max);
	FVector HalfSize = Max * (Scale = MeshComponent->GetComponentScale());
	bool HasResult;
	HasResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, Transform.GetLocation(), Transform.GetLocation(), HalfSize * 1.1,
		Transform.Rotator(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
	if(HasResult)
	{
		bool C;
		C = UKismetSystemLibrary::BoxTraceMultiForObjects(this, Transform.GetLocation(), Transform.GetLocation(), HalfSize * 0.9,
		Transform.Rotator(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
		if (!C)
		{
			OutTransform = Transform;
			Place = true;
		}else Place = false;
	}
}

void ADVBasePlaceActor::SetLocation(FTransform Transform)
{
	FTransform NewTransform;
	bool bSuccesful = false;
	ServerSetLocation(Transform, NewTransform, bSuccesful);
	if(bSuccesful)
	{
		SetReplicates(true);
		ActorTransforms = NewTransform;
		OnRep_ActorTransform();
	}
}

void ADVBasePlaceActor::OnRep_ActorTransform()
{
	SetActorTickEnabled(false);
	for(auto Materials : MeshComponent->GetMaterials())
	{
		UMaterialInstanceDynamic* M = Cast<UMaterialInstanceDynamic>(Materials);
		if(M)
		{
			M->SetScalarParameterValue(FName("Collision_Color"), 0);
			M->SetScalarParameterValue(FName("CheckingCollision"), 0);
		}
	}
	MeshComponent->SetCollisionObjectType(ECC_WorldStatic);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	MeshComponent->SetVisibility(true);
	MeshComponent->SetOnlyOwnerSee(false);
	SetActorTransform(ActorTransforms);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void ADVBasePlaceActor::CheckmultiCollision(FVector const& Target, bool& Overlap)
{
	Objects.SetNum(3, true);
	Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
	Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
	Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
	bool HasResult;
	HasResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, Target, Target, (BoxExtent/1.05),
		GetActorRotation(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
	if(HasResult)
		Overlap = true;
	else
		Overlap = false;
}

void ADVBasePlaceActor::CheckOwnerOverlap(FVector const& Target, bool& Overlap)
{
	Objects.SetNum(3, true);
	Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
	Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
	Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
	FVector Min, Max, Scale;
	MeshComponent->GetLocalBounds( Min, Max);
	Scale = MeshComponent->GetComponentScale();
	Scale = (Scale * Max) / 0.95;
	bool HasResult;
	HasResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, Target, Location, Scale,
		GetActorRotation(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
	if(HasResult)
		Overlap = true;
	else
		Overlap = false;
}

void ADVBasePlaceActor::SetObjectLocation(FVector Target)
{
	OldLocation = FMath::VInterpTo(OldLocation, Target, GetWorld()->GetDeltaSeconds(),8);
	SetActorLocationAndRotation(OldLocation, GetActorRotation(), false, nullptr, ETeleportType::None);
}

void ADVBasePlaceActor::Calculator()
{
	bool A;
	CheckmultiCollision(Location, A);
	if(A)
	{
		Location = FVector(Location.X, Location.Y, Location.Z + BoxExtent.Z);
		bool B;
		CheckmultiCollision(Location, B);
		if(B)
		{
			FVector Vector; 
			if(BoxExtent.X < BoxExtent.Y)	Vector = FVector(BoxExtent.X,BoxExtent.X,BoxExtent.Z);
			else							Vector = FVector(BoxExtent.Y,BoxExtent.Y,BoxExtent.Z);
			FVector Location2 = Location + ( Vector * Normal);
			FRotator Rotator2 = FRotator( GetActorRotation().Pitch, UKismetMathLibrary::MakeRotFromX(Normal).Yaw , GetActorRotation().Roll  );
			OldLocation = FMath::VInterpTo(OldLocation, Location2, GetWorld()->GetDeltaSeconds(),20);
			SetActorLocationAndRotation(OldLocation, Rotator2, false, nullptr, ETeleportType::None);
			SetColor(true);
		}else
		{
			SetObjectLocation(Location);
			SetColor(true);
		} 
	}else {
		SetObjectLocation(Location);
		SetColor(true);
	} 
}

void ADVBasePlaceActor::SetDynamicInstance()
{
	for(int i = 0; i < MeshComponent->GetNumMaterials();i++){
		UMaterialInstanceDynamic* M = MeshComponent->CreateDynamicMaterialInstance(i, MaterialInstanceDynamic, FName("Collision_Color"));
		if(M)
		{
			M->SetScalarParameterValue(FName("Collision_Color"), 0);
			M->SetScalarParameterValue(FName("CheckingCollision"), 0);	
		}
	}
}

void ADVBasePlaceActor::SetColor(bool Green)
{
	for(auto Materials : MeshComponent->GetMaterials())
	{
		UMaterialInstanceDynamic* M = Cast<UMaterialInstanceDynamic>(Materials);
		if(M)
		{
			M->SetScalarParameterValue(FName("Collision_Color"), Green? 1 : 0);
			M->SetScalarParameterValue(FName("CheckingCollision"), 1);
		}
	}	
}

void ADVBasePlaceActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADVBasePlaceActor, ActorTransforms);
}
