// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DVBaseMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVBaseMapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		class ADVBaseCharacter* TargetCharacter = nullptr;

	UPROPERTY(BlueprintReadWrite)
		class ADVBaseController* TargetPC = nullptr;

	UPROPERTY(BlueprintReadWrite)
		class UCurveFloat* ZoomScaleCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float MapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		bool bNeedObjectCut = false;

public:
	virtual bool CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UIUpdatesPOI(class UDVBasePOIWidget* POI, FVector2D Location, float Yaw, bool bIsVisibleOnMap, float CurrentZoomValue);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void AddPOI(class UDVBasePOIWidget* POI);

	UFUNCTION()
		void SetTargetCharacter(class ADVBaseCharacter* InTargetCharacter);

	UFUNCTION()
		virtual void HandleTargetCharacterDestroyed(AActor* Actor);

	virtual void StartUpdateMap() {};

	virtual void StopUpdateMap() {};

	virtual float GetCurrentZoomValue() {
		return 1.f;
	}

	virtual float GetMapWidthPixel() { return 500.f; }
};
