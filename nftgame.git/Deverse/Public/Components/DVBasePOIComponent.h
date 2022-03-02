// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DVBasePOIComponent.generated.h"

USTRUCT(BlueprintType)
struct FPOIWidgetOnMap
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "POI")
		class UDVBasePOIWidget* POI = nullptr;

	FPOIWidgetOnMap()
	{
	}

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEVERSE_API UDVBasePOIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDVBasePOIComponent();

	UPROPERTY(BlueprintReadWrite)
		TArray<FPOIWidgetOnMap> ArrayOfWidgetsOnMaps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bScaleWhenZoom = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bNeedCutOnMiniMap = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bExactSizeOnMap = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ObjectRadius = 0.f;

	// Set this if rectangle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ObjectWidth = 0.f;

	// Set this if rectangle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ObjectHeight = 0.f;

	// Set this if rectangle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ShapeLineSize = 0.02f;

	// Set this if rectangle
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FLinearColor ShapeLineColor = FLinearColor(1.f, 0.5f, 0.f, 1.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MinScaleValue = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 POIZOrder = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		bool bOnlyShowInRangeOfHeight = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MaxHeightDistanceFromPivot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float MinHeightDistanceFromPivot;

	virtual void AddPOIsToMaps() {};
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
