// Copyright Cosugames, LLC 2021


#include "Widgets/DVMinimapWidget.h"

void UDVMinimapWidget::ConstructMiniMap_Implementation(float Size, float GridSlotSize, UMaterialInterface* MapImage, float TotalMapSizePixel, ADVBaseCharacter* Target, float InInitialZoomValue, float InMinZoomValue, float InZoomStep, UCurveFloat* InZoomScaleCurve)
{
	//
	this->bNeedObjectCut = true;

	//
	this->ZoomScaleCurve = InZoomScaleCurve;
	this->MapSize = Size;
	this->MapGridSlotSize = GridSlotSize;

	this->InitialTacticalMapSize = TotalMapSizePixel;
	this->InitialTopLeftSizeLong = TotalMapSizePixel;
	this->InitialTopLeftSizeShort = 0.f;

	this->TargetCharacter = Target;

	this->MinZoomValue = InMinZoomValue;
	this->ZoomStep = InZoomStep;
	this->InitialZoomValue = FMath::Clamp(InInitialZoomValue, MinZoomValue, MaxZoomValue);

	PrepareData();

	GetWorld()->GetTimerManager().SetTimer(TimerZoomAtInitialState, this, &UDVMinimapWidget::AutoZoomAtInitialState, 1.f, false);
}

void UDVMinimapWidget::Zoom_Implementation(float ZoomValue)
{

}

void UDVMinimapWidget::PrepareData()
{
	this->NumOfMapColumnsOrRows = this->MapSize / this->MapGridSlotSize;

	CalculatedHalfOfMainMapSize = InitialTacticalMapSize / 2;
	CalculatedTopLeftMapPosition = CalculateTopLeftMapPosition();

	BorderWidthOfMapSize = CalculatedHalfOfMainMapSize * 0.05f;

	CurrentTopSize = FVector2D(0.f, 0.f);
	CurrentLeftSize = FVector2D(0.f, 0.f);
	CurrentMainMapSize = FVector2D(InitialTopLeftSizeLong / CurrentZoomValue, InitialTopLeftSizeLong / CurrentZoomValue);

	CurrentTopPosition = FVector2D(0.f, 0.f);
	CurrentLeftPosition = FVector2D(0.f, 0.f);
	CurrentMainMapPosition = FVector2D(0.f, 0.f);

	CalculatedHalfOfMapSize = MapSize / 2.f;
	LastFocusPoint = FVector2D(CalculatedHalfOfMapSize, CalculatedHalfOfMapSize);
	CurrentFocusPoint = LastFocusPoint;

	CurrentPivotPointOfMap = CalculatePivotPointOfMap();

	InitialNumOfPixelPerMeter = InitialTopLeftSizeLong / MapSize;
}

bool UDVMinimapWidget::CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw)
{
	if (POI && TargetCharacter && TargetCharacter->IsPendingKill() == false)
	{
		FPOIGeoInfo POIGeoInfo = POI->GetPOIGeoInfo();

		Location.X = POIGeoInfo.Location.Y / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.X;
		Location.Y = -POIGeoInfo.Location.X / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue + CurrentPivotPointOfMap.Y;

		Yaw = POIGeoInfo.Rotation.Yaw;

		// Calculate height to hide
		if (POI->GetOwnerPOIComponent() &&
			POI->GetOwnerPOIComponent()->GetOwner() &&
			POI->GetOwnerPOIComponent()->GetOwner()->IsPendingKill() == false)
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

			// Calculate location of map
			float DistanceToTargetCharacter = (FVector::Dist2D(TargetCharacter->GetActorLocation(), POIGeoInfo.Location) - POI->GetOwnerPOIComponent()->ObjectRadius) / 100.f * InitialNumOfPixelPerMeter / CurrentZoomValue;
			if (DistanceToTargetCharacter + BorderWidthOfMapSize > CalculatedHalfOfMainMapSize)
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

const FVector2D UDVMinimapWidget::CalculatePivotPointOfMap()
{
	FVector2D ReturnVector;

	ReturnVector.X = CalculatedHalfOfMainMapSize + (CurrentMainMapSize.X / 2 - CurrentFocusPoint.X / MapSize * CurrentMainMapSize.X);
	ReturnVector.Y = CalculatedHalfOfMainMapSize + (CurrentMainMapSize.Y / 2 - CurrentFocusPoint.Y / MapSize * CurrentMainMapSize.Y);

	return ReturnVector;
}

void UDVMinimapWidget::UpdateMapBaseOnTargetCharacter()
{
	if (TargetCharacter)
	{
		FVector Location = TargetCharacter->GetActorLocation();
		FRotator Rotation = TargetCharacter->GetActorRotation();

		CurrentFocusPoint = FVector2D(Location.Y / 100.f + CalculatedHalfOfMapSize, -Location.X / 100.f + CalculatedHalfOfMapSize);

		MiniMapYaw = 360 - Rotation.Yaw;
		UIUpdatesMapBaseOnTargetCharacter();

		CurrentPivotPointOfMap = CalculatePivotPointOfMap();

		this->GridSlotX = CurrentFocusPoint.X / this->MapGridSlotSize + 1;
		this->GridSlotY = CurrentFocusPoint.Y / this->MapGridSlotSize + 1;
	}
}

void UDVMinimapWidget::AddPOI_Implementation(class UDVBasePOIWidget* POI)
{

}

void UDVMinimapWidget::CalculateAllCompsSize()
{
	float CalculatedSize = (1.f / CurrentZoomValue) * InitialTopLeftSizeLong;
	CurrentMainMapSize = FVector2D(CalculatedSize, CalculatedSize);
}

void UDVMinimapWidget::StopDragginMap()
{

}

void UDVMinimapWidget::StartDragginMap()
{

}

void UDVMinimapWidget::HandleTargetCharacterDestroyed(AActor* Actor)
{
	Super::HandleTargetCharacterDestroyed(Actor);
	this->RemoveFromParent();
	StopUpdateMap();
}

void UDVMinimapWidget::StartUpdateMap()
{
	GetWorld()->GetTimerManager().SetTimer(TimerUpdateMap, this, &UDVMinimapWidget::UpdateMapBaseOnTargetCharacter, 0.02f, true);
}

void UDVMinimapWidget::StopUpdateMap()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerUpdateMap);
}

FReply UDVMinimapWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UDVMinimapWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UDVMinimapWidget::AutoZoomAtInitialState()
{
	this->CurrentZoomValue = this->InitialZoomValue;
	CalculateAllCompsSize();
	UIUpdatesSize();
}
