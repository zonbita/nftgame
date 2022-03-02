// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DVBaseMapWidget.h"
#include "DVOverviewMapWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVOverviewMapWidget : public UDVBaseMapWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		float MapGridSlotSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
		uint8 NumOfMapColumnsOrRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		bool bIsDragging;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D LastMouseDownLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D LastFocusPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentFocusPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		ESlateVisibility WrapperVisibility = ESlateVisibility::SelfHitTestInvisible;

	UPROPERTY(BlueprintReadOnly, Category = "Config")
		float InitialTopLeftSizeLong = 700.f;

	UPROPERTY(BlueprintReadOnly, Category = "Config")
		float InitialTopLeftSizeShort = 50.f;

	UPROPERTY(BlueprintReadOnly, Category = "Config")
		float InitialTacticalMapSize = 750.f;

	UPROPERTY(BlueprintReadOnly, Category = "Config")
		float InitialNumOfPixelPerMeter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		float CalculatedHalfOfMainMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		float BorderWidthOfMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		float CalculatedHalfOfMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CalculatedTopLeftMapPosition;

	float MinZoomValue = 0.01f;

	float MaxZoomValue = 0.96f;

	float ZoomStep = 0.1f;

	float InitialZoomValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		float CurrentZoomValue = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentTopSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentLeftSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentMainMapSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentTopPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentLeftPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentMainMapPosition;

	// --------------------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Runtime")
		FVector2D CurrentPivotPointOfMap;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeConstruct() override;

	virtual void PrepareData();

	virtual bool CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ConstructTacticalMap(float Size, float GridSlotSize, UMaterialInterface* MapImage, float TotalMapSizePixel, float MainMapSizePixel, float InInitialZoomValue, class ADVBaseCharacter* Target, float InMinZoomValue, float InZoomStep, UCurveFloat* InZoomScaleCurve);

	UFUNCTION(BlueprintCallable)
		void DragMap();

	UFUNCTION(BlueprintCallable)
		virtual void StopDragginMap();

	UFUNCTION(BlueprintCallable)
		virtual void StartDragginMap();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void Zoom(float ZoomValue);

	UFUNCTION(BlueprintCallable)
		virtual void CalculateAllCompsSize();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesSize();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesPosition();

	UFUNCTION(BlueprintCallable)
		void CalculateAllCompsPosition();

	UFUNCTION(BlueprintCallable)
		void MapMousePositionToMapSize(FVector2D MousePosition);

	UFUNCTION(BlueprintCallable)
		const FVector2D CalculateTopLeftMapPosition();

	UFUNCTION(BlueprintCallable)
		const FVector2D CalculatePointAtPerCentage(FVector2D Point);

	UFUNCTION(BlueprintCallable)
		virtual const FVector2D CalculatePivotPointOfMap();

	virtual void AddPOI_Implementation(class UDVBasePOIWidget* POI) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void RemovePOI(class UDVBasePOIWidget* POI);

	virtual float GetCurrentZoomValue() override;

	virtual float GetMapWidthPixel() override;

	UFUNCTION(BlueprintCallable)
		void FocusToPoint(FVector2D Point);
protected:
	void AutoZoomAtInitialState();

private:
	FTimerHandle TimerZoomAtInitialState;
	FTimerHandle TimerDragMap;
};
