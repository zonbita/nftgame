// Copyright Cosugames, LLC 2021


#include "Widgets/DVBaseMapWidget.h"
#include "Core/Human/DVBaseCharacter.h"
#include "Core/DVBaseController.h"
#include "Widgets/DVBasePOIWidget.h"

bool UDVBaseMapWidget::CalculatePOIGeoOnMap(class UDVBasePOIWidget* POI, FVector2D& Location, float& Yaw)
{
	return true;
}

void UDVBaseMapWidget::SetTargetCharacter(class ADVBaseCharacter* InTargetCharacter)
{
	TargetCharacter = InTargetCharacter;
	if (InTargetCharacter) {
		InTargetCharacter->OnDestroyed.AddDynamic(this, &UDVBaseMapWidget::HandleTargetCharacterDestroyed);
		TargetPC = Cast<ADVBaseController>(TargetCharacter->GetController());
	}
}

void UDVBaseMapWidget::HandleTargetCharacterDestroyed(AActor* Actor)
{
	SetTargetCharacter(nullptr);
}

void UDVBaseMapWidget::AddPOI_Implementation(class UDVBasePOIWidget* POI)
{

}

