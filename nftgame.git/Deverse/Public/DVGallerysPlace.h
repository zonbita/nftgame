// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/DVWalletInterface.h"
#include "Core/DVEventInterface.h"
#include "DVGallerysPlace.generated.h"

USTRUCT(BlueprintType)
struct FGalleryFrame {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "DV|Nfts") // This may be too much but for now
	FLiteNft NftData;

	UPROPERTY(BlueprintReadOnly, Category = "DV|Nfts")
		class ADVFrame* Frame = nullptr;

	FGalleryFrame() {};
};

USTRUCT(BlueprintType)
struct FGallery { 
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "DV|Nfts")
	TArray<FGalleryFrame> Frames;

	/*bool GetFrame(const FString TokenAddress, FGalleryFrame& FoundFrame) {
		for (FGalleryFrame Frame : Frames) {
			if (Frame.NftData.TokenAddress.Equals(TokenAddress)) {
				FoundFrame = Frame;
				return true;
			}
		}
		return false;
	}*/

	/*bool GetFrame(const UPrimitiveComponent* HitMesh, FGalleryFrame& FoundFrame, int32& Index) {
		for (int32 i = 0; i < Frames.Num(); i++) {
			if (Frames[i].FrameMesh == HitMesh) {
				FoundFrame = Frames[i];
				Index = i;
				return true;
			}
		}
		return false;
	}*/
};

UCLASS()
class DEVERSE_API ADVGallerysPlace : public AActor, public IDVWalletInterface, public IDVEventInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADVGallerysPlace();

	UPROPERTY(BlueprintReadWrite, NonTransactional, meta=(Category="Default", OverrideNativeName="DefaultSceneRoot"))
		USceneComponent* SceneRoot;
	
	UPROPERTY(EditAnywhere, Category = "Components")
		class UArrowComponent* Direction;

	UPROPERTY(EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<class ADVFrame> FrameClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "Attributes")
	TArray<FNft> OwningNfts;

	UPROPERTY(EditAnywhere, Category = "Attributes")
		int32 MaxRow = 2;

	UPROPERTY(EditAnywhere, Category = "Attributes")
		int32 MaxColumn = 2;

	UPROPERTY(EditAnywhere, Category = "Attributes")
		float Space = 300.f;

	UFUNCTION(BlueprintCallable, meta = (Category, OverrideNativeName = "OnRep_Gallery"))
		void OnRep_Gallery();
	
	UPROPERTY(ReplicatedUsing = "OnRep_Gallery")
		FGallery Gallery;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UStaticMesh* Mesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
    	UMaterialInterface* StoredMaterial;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UMaterialInterface* FrameMaterial;

	UFUNCTION(BlueprintCallable)
	void PopulateFrames();

	UFUNCTION(BlueprintCallable)
	void AddFrame(FNft InNft, class UObject* InObject);

	void AddFrame(FGalleryFrame InFrame);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddFrame(FGalleryFrame InFrame);

	void ChangeFrame(int32 FrameIndex, FNft InNftData);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeFrame(int32 Frame, FNft InNftData);

	void GetSpawnLocation(FVector& OutLocation, FRotator& OutRotation);

	bool IsFull();
protected:
	virtual void BeginPlay() override;
	
	// ~Wallet interface
	void OnFinishQueryNfts_Implementation(const TArray<FNft>& Nfts);

	void FinishDownloadNft_Implementation(FNft InNft);

	// ~Event interface
	void EventInteract_Implementation(class ADVBaseCharacter* Interactor, class UPrimitiveComponent* HitComponent);

	virtual void Destroyed() override;
	int32 CurrentFrameCount = 0;
private:
	UTexture2D* Texture;
};
