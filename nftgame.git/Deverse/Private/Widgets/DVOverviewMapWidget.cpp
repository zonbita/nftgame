// Copyright Cosugames, LLC 2021


#include "Widgets/DVOverviewMapWidget.h"
#include "Widgets/DVBasePOIWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

FReply UDVOverviewMapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton"))
	{
		StartDragginMap();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDVOverviewMapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton") && bIsDragging)
	{
		StopDragginMap();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UDVOverviewMapWidget::NativeOnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Zoom(InMouseEvent.GetWheelDelta());

	return Super::NativeOnMouseWheel(InGeometry, InMouseEvent);
}

void UDVOverviewMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDVOverviewMapWidget::PrepareData()
{
	this->NumOfMapColumnsOrRows = this->MapSize / this->MapGridSlotSize;

	CalculatedHalfOfMainMapSize = (InitialTacticalMapSize / 2) - InitialTopLeftSizeShort;
	CalculatedTopLeftMapPosition = CalculateTopLeftMapPosition();

	CurrentTopSize = FVector2D(InitialTopLeftSizeShort, InitialTopLeftSizeLong);
	CurrentLeftSize = FVector2D(InitialTopLeftSizeLong, InitialTopLeftSizeShort);
	CurrentMainMapSize = FVector2D(InitialTopLeftSizeLong, InitialTopLeftSizeLong);

	CurrentTopPosition = FVector2D(0.f, 0.f);
	CurrentLeftPosition = FVector2D(0.f, 0.f);
	CurrentMainMapPosition = FVector2D(0.f, 0.f);

	CalculatedHalfOfMapSize = MapSize / 2.f;
	LastFocusPoint = FVector2D(CalculatedHalfOfMapSize, CalculatedHalfOfMapSize);
	CurrentFocusPoint = LastFocusPoint;

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();

	InitialNumOfPixelPerMeter = InitialTopLeftSizeLong / MapSize;
}

bool UDVOverviewMapWidget::CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw)
{
	if (POI &&
		POI->GetOwnerPOIComponent() &&
		POI->GetOwnerPOIComponent()->GetOwner())
	{
		FPOIGeoInfo POIGeoInfo = POI->GetPOIGeoInfo();

		float Extra = POI->GetOwnerPOIComponent()->ObjectRadius / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue;

		Location.X = POIGeoInfo.Location.Y / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.X;
		Location.Y = -POIGeoInfo.Location.X / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.Y;

		Yaw = POIGeoInfo.Rotation.Yaw;

		// Calculate height to hide
		if (TargetCharacter &&
			TargetCharacter->IsPendingKill() == false)
		{
			if (POI->GetOwnerPOIComponent()->bOnlyShowInRangeOfHeight == true)
			{
				float TargetCharacterHeight = TargetCharacter->GetActorLocation().Z;
				float OwnerHeight = POI->GetOwnerPOIComponent()->GetOwner()->GetActorLocation().Z;

				if (TargetCharacterHeight > OwnerHeight + POI->GetOwnerPOIComponent()->MaxHeightDistanceFromPivot ||
					TargetCharacterHeight < OwnerHeight - POI->GetOwnerPOIComponent()->MinHeightDistanceFromPivot)
				{
					return false;
				}
			}
		}

		if (Location.X + Extra < 0 || Location.X - Extra > InitialTopLeftSizeLong || Location.Y + Extra < 0 || Location.Y - Extra > InitialTopLeftSizeLong)
		{
			return false;
		}
	}
	else
	{
		FPOIGeoInfo POIGeoInfo = POI->GetPOIGeoInfo();

		Location.X = POIGeoInfo.Location.Y / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.X;
		Location.Y = -POIGeoInfo.Location.X / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.Y;

		Yaw = POIGeoInfo.Rotation.Yaw;
	}

	return true;
}

void UDVOverviewMapWidget::DragMap()
{
	FVector2D MousePos; GetOwningPlayer()->GetMousePosition(MousePos.X, MousePos.Y);
	FVector2D PreProcessVector = (LastMouseDownLocation - MousePos) / CurrentMainMapSize / (1.f - CurrentZoomValue);

	CurrentFocusPoint.X = FMath::Clamp(PreProcessVector.X * MapSize + LastFocusPoint.X, 0.f, MapSize);
	CurrentFocusPoint.Y = FMath::Clamp(PreProcessVector.Y * MapSize + LastFocusPoint.Y, 0.f, MapSize);

	CalculateAllCompsPosition();

	UIUpdatesPosition();

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();
}

void UDVOverviewMapWidget::StopDragginMap()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerDragMap);
	bIsDragging = false;
	WrapperVisibility = ESlateVisibility::SelfHitTestInvisible;
	LastFocusPoint = CurrentFocusPoint;
}

void UDVOverviewMapWidget::StartDragginMap()
{
	bIsDragging = true;
	WrapperVisibility = ESlateVisibility::Visible;

	if (GetOwningPlayer())
	{
		GetOwningPlayer()->GetMousePosition(LastMouseDownLocation.X, LastMouseDownLocation.Y);
		GetWorld()->GetTimerManager().SetTimer(TimerDragMap, this, &UDVOverviewMapWidget::DragMap, 0.01f, true);
	}
}

void UDVOverviewMapWidget::CalculateAllCompsSize()
{
	float CalculatedSize = (1.f / CurrentZoomValue) * InitialTopLeftSizeLong;

	CurrentTopSize = FVector2D(CalculatedSize, InitialTopLeftSizeShort);
	CurrentLeftSize = FVector2D(InitialTopLeftSizeShort, CalculatedSize);
	CurrentMainMapSize = FVector2D(CalculatedSize, CalculatedSize);
}

void UDVOverviewMapWidget::CalculateAllCompsPosition()
{
	float CurrentZoomValueMinusOne = CurrentZoomValue - 1.f;

	float TopX = (this->CurrentFocusPoint.X / MapSize) * CurrentTopSize.X * CurrentZoomValueMinusOne;
	CurrentTopPosition = FVector2D(TopX, 0.f);

	float LeftY = (this->CurrentFocusPoint.Y / MapSize) * CurrentLeftSize.Y * CurrentZoomValueMinusOne;
	CurrentLeftPosition = FVector2D(0.f, LeftY);

	CurrentMainMapPosition = FVector2D(TopX, LeftY);
}

void UDVOverviewMapWidget::MapMousePositionToMapSize(FVector2D MousePosition)
{
	FVector2D PreProcessVector = (MousePosition - CalculatedTopLeftMapPosition - CurrentMainMapPosition) / CurrentMainMapSize;

	LastFocusPoint.X = FMath::Clamp(PreProcessVector.X * MapSize, 0.f, MapSize);
	LastFocusPoint.Y = FMath::Clamp(PreProcessVector.Y * MapSize, 0.f, MapSize);

	CurrentFocusPoint = LastFocusPoint;

	CalculateAllCompsSize();
	CalculateAllCompsPosition();

	CalculatePointAtPerCentage(LastFocusPoint);

	LastFocusPoint = LastFocusPoint - (MousePosition - CalculatePointAtPerCentage(LastFocusPoint)) / (1.f - CurrentZoomValue) * MapSize / CurrentMainMapSize;
	CurrentFocusPoint = LastFocusPoint;
}

const FVector2D UDVOverviewMapWidget::CalculatePointAtPerCentage(FVector2D Point)
{
	return (CalculatedTopLeftMapPosition + CurrentMainMapPosition) + (Point / MapSize * CurrentMainMapSize);
}

const FVector2D UDVOverviewMapWidget::CalculatePivotPointOfMap()
{
	FVector2D ReturnVector;

	ReturnVector.X = CurrentMainMapPosition.X + CurrentMainMapSize.X / 2;
	ReturnVector.Y = CurrentMainMapPosition.Y + CurrentMainMapSize.Y / 2;

	return ReturnVector;
}

void UDVOverviewMapWidget::AddPOI_Implementation(class UDVBasePOIWidget* POI)
{

}

float UDVOverviewMapWidget::GetCurrentZoomValue()
{
	return CurrentZoomValue;
}

float UDVOverviewMapWidget::GetMapWidthPixel()
{
	return InitialTopLeftSizeLong;
}

void UDVOverviewMapWidget::FocusToPoint(FVector2D Point)
{
	Point = Point / (1.f - CurrentZoomValue);

	CurrentFocusPoint.X = Point.Y / 100.f + CalculatedHalfOfMapSize;
	CurrentFocusPoint.Y = -Point.X / 100.f + CalculatedHalfOfMapSize;

	CalculateAllCompsPosition();

	UIUpdatesPosition();

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();

	LastFocusPoint = CurrentFocusPoint;
}

void UDVOverviewMapWidget::AutoZoomAtInitialState()
{
	this->CurrentZoomValue = this->InitialZoomValue;

	CalculateAllCompsSize();
	CalculateAllCompsPosition();

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();

	UIUpdatesSize();
	UIUpdatesPosition();
}

void UDVOverviewMapWidget::RemovePOI_Implementation(class UDVBasePOIWidget* POI)
{

}

const FVector2D UDVOverviewMapWidget::CalculateTopLeftMapPosition()
{
	// 100 is PosX of GridPanel_Wrapper
	return FVector2D(100.f + InitialTopLeftSizeShort - 5.f, (UWidgetLayoutLibrary::GetViewportSize(GetWorld()) / 2.f - InitialTacticalMapSize / 2).Y + InitialTopLeftSizeShort - 5.f);
}

void UDVOverviewMapWidget::Zoom_Implementation(float ZoomValue)
{
	if (ZoomValue > 0)
	{
		// Zoom in
		CurrentZoomValue = FMath::Clamp(CurrentZoomValue - ZoomStep, this->MinZoomValue, this->MaxZoomValue);

		FVector2D MousePos; GetOwningPlayer()->GetMousePosition(MousePos.X, MousePos.Y);
		MapMousePositionToMapSize(MousePos);

		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::MakeRandomColor(), MousePos.ToString());
	}
	else
	{
		// Zoom out
		CurrentZoomValue = FMath::Clamp(CurrentZoomValue + ZoomStep, this->MinZoomValue, this->MaxZoomValue);
	}

	CalculateAllCompsSize();
	CalculateAllCompsPosition();

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();

	UIUpdatesSize();
	UIUpdatesPosition();
}

void UDVOverviewMapWidget::ConstructTacticalMap_Implementation(float Size, float GridSlotSize, UMaterialInterface* MapImage, float TotalMapSizePixel, float MainMapSizePixel, float InInitialZoomValue, class ADVBaseCharacter* Target, float InMinZoomValue, float InZoomStep, UCurveFloat* InZoomScaleCurve)
{
	//
	this->bNeedObjectCut = false;

	//
	this->ZoomScaleCurve = InZoomScaleCurve;
	this->MapSize = Size;
	this->MapGridSlotSize = GridSlotSize;

	this->InitialTacticalMapSize = TotalMapSizePixel;
	this->InitialTopLeftSizeLong = MainMapSizePixel;
	this->InitialTopLeftSizeShort = InitialTacticalMapSize - InitialTopLeftSizeLong;

	// Finally set initial zoom value
	this->MinZoomValue = InMinZoomValue;
	this->ZoomStep = InZoomStep;
	this->InitialZoomValue = FMath::Clamp(InInitialZoomValue, MinZoomValue, MaxZoomValue);

	this->TargetCharacter = Target;
	PrepareData();

	GetWorld()->GetTimerManager().SetTimer(TimerZoomAtInitialState, this, &UDVOverviewMapWidget::AutoZoomAtInitialState, 1.f, false);
}
