// Copyright Cosugames, LLC 2021


#include "Widgets/DVBasePOIWidget.h"
#include "Widgets/DVMinimapWidget.h"
#include "Online/DVPlayerState.h"
#include "Components/DVBasePOIComponent.h"

FPOIGeoInfo UDVBasePOIWidget::GetPOIGeoInfo()
{
	FPOIGeoInfo POIInfo;
	return POIInfo;
}

void UDVBasePOIWidget::AddPOIToMap(class UDVBaseMapWidget* Map, class UDVBasePOIComponent* OwnerComponent)
{
	CodeHidePOI();

	this->OwnerComp = OwnerComponent;
	this->TargetMap = Map;
	this->ZoomScaleCurve = TargetMap->ZoomScaleCurve;
	this->TargetMap->AddPOI(this);

	if (this->OwnerComp)
	{
		if (this->OwnerComp->bExactSizeOnMap == true)
		{
			float Offset = this->TargetMap->GetMapWidthPixel() / 850.f;
			float WidthPixel = (((this->OwnerComp->ObjectRadius / 100.f) / this->TargetMap->MapSize) / 0.02f) * 34.f * Offset;
			UISetupSize(WidthPixel);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(TimerUpdatePOIOnMaps, this, &UDVBasePOIWidget::UpdatePOIOnMaps, 0.02f, true);
	CodeShowPOI();
}

void UDVBasePOIWidget::RemovePOIFromMap()
{
	GetWorld()->GetTimerManager().SetTimer(DelayToRemovePOITimer, this, &UDVBasePOIWidget::RemovePOIFromMapCompletely, 5.f, false);
}

void UDVBasePOIWidget::RemovePOIFromMapImmediately()
{
	RemovePOIFromMapCompletely();
}

void UDVBasePOIWidget::RemovePOIFromMapCompletely()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerUpdatePOIOnMaps);
		RemoveFromParent();
	}
}

void UDVBasePOIWidget::UIUpdatesPOIState()
{

}

void UDVBasePOIWidget::UpdatePOIOnMaps()
{
	if (TargetMap && TargetMap->IsVisible())
	{
		//V_LOG(LogTemp, Error, TargetMap->GetName());

		FVector2D CalculatedLoc = FVector2D(LastActorGeoInfo.Location.X, LastActorGeoInfo.Location.Y);
		float CalcualtedYaw = LastActorGeoInfo.Rotation.Yaw;

		bool bIsVisibleOnMap = TargetMap->CalculatePOIGeoOnMap(this, CalculatedLoc, CalcualtedYaw);

		// regarding scale, location...
		this->TargetMap->UIUpdatesPOI(this, CalculatedLoc, CalcualtedYaw, bIsVisibleOnMap, TargetMap->GetCurrentZoomValue());

		if (bIsVisibleOnMap)
		{
			// regarding name, team, class...
			this->UIUpdatesPOIState();
		}

		// special cut
		if (TargetMap->bNeedObjectCut)
		{
			if (OwnerComp != nullptr)
			{
				if (OwnerComp->bNeedCutOnMiniMap == true)
				{
					if (OwnerComp != nullptr && OwnerComp->GetOwner() && this->TargetMap->TargetCharacter)
					{
						FVector OwnerLocation = OwnerComp->GetOwner()->GetActorLocation();
						FVector TargetLocation = this->TargetMap->TargetCharacter->GetActorLocation();

						OwnerLocation = FVector(OwnerLocation.X, OwnerLocation.Y, 0);
						TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0);

						float Distance = FVector::Dist2D(OwnerLocation, TargetLocation);

						FVector ForwardVector = OwnerComp->GetOwner()->GetActorForwardVector();
						FVector TargetVector = TargetLocation - OwnerLocation;

						// Left vector
						FRotator OwnerRotation = OwnerComp->GetOwner()->GetActorRotation();
						FVector LeftVector = FRotator(OwnerRotation.Pitch, OwnerRotation.Yaw - 90, OwnerRotation.Roll).Vector();

						// What we need
						float Degrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVector, TargetVector.GetSafeNormal())));
						bool bIsLeft = FVector::DotProduct(LeftVector.GetSafeNormal(), TargetVector.GetSafeNormal()) > 0 ? true : false;

						float MinimapScaleCompareWithObject = (this->TargetMap->MapSize * this->TargetMap->GetCurrentZoomValue()) / (OwnerComp->ObjectRadius / 50.f);
						float OutX, OutY;

						CalculateXYOfMinimapOnMaterial(bIsLeft, Degrees, Distance, OwnerComp->ObjectRadius, OutX, OutY);

						this->UIUpdatesCutOnMiniMap(MinimapScaleCompareWithObject * 0.5f, OutX, OutY);
					}
				}
			}
		}
	}
	else
	{
		//V_LOG(LogTemp, Error, TargetMap->GetName());

		if (TargetMap->IsA(UDVMinimapWidget::StaticClass()) == false)
		{
			// Still calculate last GEO info
			if (TargetMap && TargetMap->IsVisible() == false)
			{
				FVector2D CalculatedLoc = FVector2D(LastActorGeoInfo.Location.X, LastActorGeoInfo.Location.Y);
				float CalcualtedYaw = LastActorGeoInfo.Rotation.Yaw;
				bool bIsVisibleOnMap = TargetMap->CalculatePOIGeoOnMap(this, CalculatedLoc, CalcualtedYaw);

				// regarding scale, location...
				this->TargetMap->UIUpdatesPOI(this, CalculatedLoc, CalcualtedYaw, bIsVisibleOnMap, TargetMap->GetCurrentZoomValue());

				// regarding name, team, class...
				this->UIUpdatesPOIState();
			}
		}
	}
}

float UDVBasePOIWidget::GetAcceptedScaleValue(float CurrentScaleValue)
{
	if (OwnerComp)
	{
		if (CurrentScaleValue < OwnerComp->MinScaleValue)
		{
			return OwnerComp->MinScaleValue;
		}
		else
		{
			return CurrentScaleValue;
		}
	}
	else
	{
		return 1.0f;
	}
}

UDVBasePOIComponent* UDVBasePOIWidget::GetOwnerPOIComponent()
{
	return OwnerComp;
}

UDVBaseMapWidget* UDVBasePOIWidget::GetTargetMap()
{
	return TargetMap;
}

void UDVBasePOIWidget::CalculateXYOfMinimapOnMaterial(bool InbIsLeft, float InDegrees, float InDistance, float InObjectRadius, float& OutX, float& OutY)
{
	// Calculate target degrees
	float TargetDegress;
	if (InDegrees > 90)
	{
		TargetDegress = 180.f - InDegrees;
	}
	else
	{
		TargetDegress = InDegrees;
	}

	float PreCalDistance = InDistance * 0.5f / InObjectRadius;

	OutX = UKismetMathLibrary::DegSin(TargetDegress) * PreCalDistance;
	OutY = UKismetMathLibrary::DegCos(TargetDegress) * PreCalDistance;

	// X
	if (InbIsLeft)
	{
		OutX = -OutX;
	}

	// Y
	if (InDegrees <= 90)
	{
		OutY = -OutY;
	}

	OutX += 0.5f;
	OutY += 0.5f; //
}

void UDVBasePOIWidget::CodeShowPOI()
{
	if (bIsHidden == true)
	{
		GetWorld()->GetTimerManager().SetTimer(ShowPOITimer, this, &UDVBasePOIWidget::ActuallyShowPOI, 0.02f, false);
		bIsHidden = false;
	}
}

void UDVBasePOIWidget::ActuallyShowPOI()
{
	UIShowPOI();
}

void UDVBasePOIWidget::CodeHidePOI()
{
	GetWorld()->GetTimerManager().ClearTimer(ShowPOITimer);
	UIHidePOI();
	bIsHidden = true;
}

void UDVBasePOIWidget::HandleEventPlayerRespawn(ADVPlayerState* InPlayerState)
{
	TargetPlayerState = InPlayerState;
}

void UDVBasePOIWidget::UIHidePOI_Implementation()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UDVBasePOIWidget::UIShowPOI_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);
}

