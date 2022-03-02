// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Deverse/Deverse.h"
#include "DVBlueprintImageLoader.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVBlueprintImageLoader : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static void LoadImage(FImageData ImageData);

	void StartLoad(FImageData ImageData);
};
