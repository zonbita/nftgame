// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DVOverviewMapWidget.h"
#include "DVMinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVMinimapWidget : public UDVOverviewMapWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		float MiniMapYaw;

	UPROPERTY(BlueprintReadWrite)
		int32 GridSlotX;

	UPROPERTY(BlueprintReadWrite)
		int32 GridSlotY;

	virtual void Zoom_Implementation(float ZoomValue) override;

	virtual void PrepareData() override;

	virtual bool CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw) override;

	virtual const FVector2D CalculatePivotPointOfMap() override;

	void UpdateMapBaseOnTargetCharacter();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UIUpdatesMapBaseOnTargetCharacter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ConstructMiniMap(float Size, float GridSlotSize, UMaterialInterface* MapImage, float TotalMapSizePixel, ADVBaseCharacter* Target, float InInitialZoomValue, float InMinZoomValue, float InZoomStep, UCurveFloat* InZoomScaleCurve);

	virtual void AddPOI_Implementation(class UDVBasePOIWidget* POI) override;

	virtual void CalculateAllCompsSize() override;

	virtual void StopDragginMap() override;

	virtual void StartDragginMap() override;

	virtual void HandleTargetCharacterDestroyed(AActor* Actor) override;

	virtual void StartUpdateMap() override;

	virtual void StopUpdateMap() override;
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
private:
	FTimerHandle TimerUpdateMap;
	FTimerHandle TimerZoomAtInitialState;
	void AutoZoomAtInitialState();
};
