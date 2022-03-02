// Copyright Cosugames, LLC 2021


#include "Actors/DVActorMovement.h"
#include "Core/Human/DVHuman.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/TransformCalculus3D.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
ADVActorMovement::ADVActorMovement()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(RootComponent);
}

void ADVActorMovement::OnRep_ActorTransform()
{
	UE_LOG(LogTemp, Warning, TEXT("Some warning message") );
	SetActorTransform(ActorTransforms);
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
	Owner->SetOwner(this);
}

// Called when the game starts or when spawned
void ADVActorMovement::BeginPlay()
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
void ADVActorMovement::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AddRotate(DeltaTime);
	FTimerHandle Handle;
}

void ADVActorMovement::AddRotate(float Delta)
{
	if(Yaw != 0)
		MeshComponent->AddLocalRotation(FRotator(0,0,(Yaw + Delta)),false,NULL, ETeleportType::None);
	if(Pitch != 0)
		MeshComponent->AddLocalRotation(FRotator(0,(Pitch + Delta),0),false,NULL, ETeleportType::None);
}

void ADVActorMovement::RotateYaw(float value)
{
	Yaw = value;
}

void ADVActorMovement::RotatePitch(float value)
{
	Pitch = value;
}

void ADVActorMovement::Trace(FVector Start, FVector End)
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

void ADVActorMovement::ServerSetLocation(FTransform const& Transform, FTransform& OutTransform, bool& Place)
{
	Objects.SetNum(3, true);
	Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
	Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
	Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
	FVector Min, Max, Scale;
	MeshComponent->GetLocalBounds( Min, Max);
	FVector HalfSize = Max * (Scale = MeshComponent->GetComponentScale());
	bool HasResult;
	HasResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, StartTrace, EndTrace, HalfSize * 1.1,
		Transform.Rotator(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
	if(HasResult)
	{
		bool C;
		C = UKismetSystemLibrary::BoxTraceMultiForObjects(this, StartTrace, EndTrace, HalfSize * 0.9,
		Transform.Rotator(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
		if (C)
		{
			OutTransform = Transform;
			Place = true;
		}else Place = false;
	}
}

void ADVActorMovement::SetLocation(FTransform Transform)
{
	FTransform NewTransform;
	bool set;
	ServerSetLocation(Transform, NewTransform, set);
	if(set)
	{
		SetReplicates(true);
		ActorTransforms = NewTransform;
		OnRep_ActorTransform();
	}
}

void ADVActorMovement::CheckmultiCollision(FVector const& Target, bool& Overlap)
{
	Objects.SetNum(3, true);
	Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
	Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
	Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
	bool HasResult;
	HasResult = UKismetSystemLibrary::BoxTraceMultiForObjects(this, Target, Location, (BoxExtent/1.05),
		GetActorRotation(), Objects, false, Actor, EDrawDebugTrace::None,Hits,
		true, FLinearColor(1.000000,0.000000,0.000000,1.000000),
		FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
	if(HasResult)
		Overlap = true;
	else
		Overlap = false;
}

void ADVActorMovement::CheckOwnerOverlap(FVector const& Target, bool& Overlap)
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

void ADVActorMovement::SetObjectLocation(FVector Target)
{
	OldLocation = FMath::VInterpTo(OldLocation, Target, GetWorld()->GetDeltaSeconds(),20);
	SetActorLocationAndRotation(OldLocation, GetActorRotation(), false, nullptr, ETeleportType::None);
}

void ADVActorMovement::Calculator()
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
			FRotator NormalZ = UKismetMathLibrary::MakeRotFromX(Normal);
			FRotator NewRotator = FRotator(GetActorRotation().Roll,GetActorRotation().Pitch, NormalZ.Yaw +90);
			bool BeHon;
			BoxExtent.X < BoxExtent.Y ? BeHon = true : BeHon = false ;
			FVector Vector; 
			if(BeHon) Vector = FVector(BoxExtent.X,BoxExtent.X,BoxExtent.Z);
			else Vector = FVector(BoxExtent.Y,BoxExtent.Y,BoxExtent.Z);
			FVector NewLocation = Location + ( Vector * Normal);
			Objects.SetNum(3, true);
			Objects[0] = EObjectTypeQuery::ObjectTypeQuery1;
			Objects[1] = EObjectTypeQuery::ObjectTypeQuery2;
			Objects[2] = EObjectTypeQuery::ObjectTypeQuery6;
			bool C;
			C = UKismetSystemLibrary::BoxTraceMultiForObjects(this, NewLocation, NewLocation, (BoxExtent/1.05),NewRotator, Objects, false, Actor, EDrawDebugTrace::None,Hits,true, FLinearColor(1.000000,0.000000,0.000000,1.000000),FLinearColor(0.000000,1.000000,0.000000,1.000000), 5.000000);
			if(C)
			{
				SetObjectLocation(Location);
				SetColor(false);
			}else
			{
				OldLocation = FMath::VInterpTo(OldLocation, NewLocation, GetWorld()->GetDeltaSeconds(),20);
				SetActorLocationAndRotation(OldLocation, NewRotator, false, nullptr, ETeleportType::None);
				SetColor(true);
			}
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

void ADVActorMovement::SetDynamicInstance()
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

void ADVActorMovement::SetColor(bool Green)
{
	for(auto Materials : MeshComponent->GetMaterials())
	{
		UMaterialInstanceDynamic* M = Cast<UMaterialInstanceDynamic>(Materials);
		if(M)
		{
			M->SetScalarParameterValue(FName("Collision_Color"), Green = true ? 1 : 0);
			M->SetScalarParameterValue(FName("CheckingCollision"), 1);
		}
	}	
}

void ADVActorMovement::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADVActorMovement, ActorTransforms);
}