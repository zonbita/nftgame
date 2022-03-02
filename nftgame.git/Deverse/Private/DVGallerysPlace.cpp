#include "DVGallerysPlace.h"

#include "GeneratedCodeHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Texture2DDynamic.h"
#include "Actors/DVFrame.h"

// Sets default values
ADVGallerysPlace::ADVGallerysPlace()
{
	bReplicates = true;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = SceneRoot;
	PrimaryActorTick.bCanEverTick = false;
	Direction = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction Arrow Component"));
	Direction->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADVGallerysPlace::BeginPlay()
{
	Super::BeginPlay();
}

template<typename T>
static void Array_Shuffle(const TArray<T>& TargetArray)
{
	int32 LastIndex = TargetArray.Num() - 1;
	for (int32 i = 0; i < LastIndex; ++i)
	{
		int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			const_cast<TArray<T>*>(&TargetArray)->Swap(i, Index);
		}
	}
}

void ADVGallerysPlace::OnFinishQueryNfts_Implementation(const TArray<FNft>& Nfts)
{
	for (FNft Nft : Nfts) {
		
	}
}

void ADVGallerysPlace::FinishDownloadNft_Implementation(FNft InNft)
{
	AddFrame(InNft, nullptr);
}

void ADVGallerysPlace::EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent)
{
	FGalleryFrame FoundFrame;
	//int32 Index;
	//Gallery.GetFrame(HitComponent, FoundFrame, Index);
	//int32 RandomIndex = FMath::RandRange(0, Gallery.Frames.Num() - 1);
	//UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(HitComponent);
	//if (MeshComp) {
		//ChangeFrame(Index, Gallery.Frames[RandomIndex].NftData);
		//UMaterialInstanceDynamic* DynMat = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(0));
		//if (DynMat) {
			//DynMat->SetTextureParameterValue(FName(TEXT("Texture")), Cast<UTexture>(Gallery.Frames[RandomIndex].NftData.Object));
		//}
	//}
	// RPC to change value in frames (params : nftdata (maybe too much, consider smaller data later)

	// Flip switch to on rep the data again (check if there's a persistent static mesh already, then just swap the image)(make sure to download if it's not valid on client)
}

void ADVGallerysPlace::Destroyed()
{
	for (FGalleryFrame Frame : Gallery.Frames) {
		if (Frame.Frame && !Frame.Frame->IsPendingKillPending())
			Frame.Frame->Destroy(true);
	}
	Super::Destroyed();
}

void ADVGallerysPlace::PopulateFrames()
{
	//Array_Shuffle(ArrayGallery);
	/*Array_Shuffle(OwningNfts);
	for (FNft Nft : OwningNfts) {
		FGalleryFrame Frame;
		Frame.NftData = Nft;
		Gallery.Frames.Add(Frame);
	}*/
	OnRep_Gallery();
}

void ADVGallerysPlace::AddFrame(FNft InNft, class UObject* InObject)
{
	FGalleryFrame Frame;
	FLiteNft LiteNft = FLiteNft(InNft.TokenAddress, InNft.OwnerAddress, InNft.Name, InNft.Symbol, InNft.TokenId);
	LiteNft.Metadata.ImageURL = InNft.Metadata.ImageURL;
	Frame.NftData = LiteNft;

	AddFrame(Frame);	
}

void ADVGallerysPlace::AddFrame(FGalleryFrame InFrame)
{
	if (GetLocalRole() < ROLE_Authority) {
		ServerAddFrame(InFrame);
	}
	else {
		Gallery.Frames.Add(InFrame);
		OnRep_Gallery();
		int n = Gallery.Frames.Num();
		if (n < 1) return;
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);

		float Remainder;
		for (int i = 0; i < MaxRow * MaxColumn; i++)
		{
			if (i >= n) return; // Make sure we don't exceed the amount of nfts
			if (Gallery.Frames[i].Frame == nullptr) {
				int Return_value = UKismetMathLibrary::FMod(i, MaxColumn, Remainder);
				FName name = *FString::Printf(TEXT("Mesh %i"), i);
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				ADVFrame* Frame = GetWorld()->SpawnActor<ADVFrame>(FrameClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
				if (Frame)
				{
					FVector Loc;
					Frame->MeshComponent->SetStaticMesh(Mesh);
					Frame->AttachToComponent(RootComponent, AttachmentRules);
					Loc.X = 0.f;
					Loc.Y = Space * Remainder;
					Loc.Z = Space * Return_value;
					Frame->SetActorRelativeLocation(Loc);
					Frame->UpdateNftDisplay(Gallery.Frames[i].NftData);
					Gallery.Frames[i].Frame = Frame;
					CurrentFrameCount++;
				}
			}
			else {

			}
		}
	}
}

void ADVGallerysPlace::ServerAddFrame_Implementation(FGalleryFrame InFrame)
{
	AddFrame(InFrame);
}

bool ADVGallerysPlace::ServerAddFrame_Validate(FGalleryFrame InFrame)
{
	return true;
}

void ADVGallerysPlace::ChangeFrame(int32 FrameIndex, FNft InNftData)
{
	/*if (GetLocalRole() < ROLE_Authority) {
		ServerChangeFrame(FrameIndex, InNftData);
	}
	Gallery.Frames[FrameIndex].NftData = InNftData;
	OnRep_Gallery();*/
}

void ADVGallerysPlace::ServerChangeFrame_Implementation(int32 Frame, FNft InNftData)
{
	ChangeFrame(Frame, InNftData);
}

bool ADVGallerysPlace::ServerChangeFrame_Validate(int32 Frame, FNft InNftData)
{
	return true;
}

void ADVGallerysPlace::OnRep_Gallery()
{

}

void ADVGallerysPlace::GetSpawnLocation(FVector& OutLocation, FRotator& OutRotation)
{
	OutLocation = Direction->GetComponentLocation();
	OutRotation = Direction->GetComponentRotation();
}


bool ADVGallerysPlace::IsFull()
{
	return CurrentFrameCount >= MaxColumn * MaxRow;
}

void ADVGallerysPlace::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADVGallerysPlace, Gallery);
}