// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "DVWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API ADVWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	ADVWorldSettings();

	UPROPERTY(EditAnywhere, Category = "World")
	bool bSpawnSpaceship = false;
};
