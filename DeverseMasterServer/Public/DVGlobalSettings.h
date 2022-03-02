// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DVGlobalSettings.generated.h"

/**
 * 
 */
UCLASS(config=Engine, defaultconfig)
class DEVERSEMASTERSERVER_API UDVGlobalSettings : public UObject
{
	GENERATED_BODY()
public:	
		UPROPERTY(config, EditAnywhere, Category = "Deverse Global Settings")
		FString MasterServerUrl = "www.deverse.world:8081";

	UPROPERTY(config, EditAnywhere, Category = "Deverse Global Settings")
		FString LocalMasterServerUrl = "http:/127.0.0.1:8080";

	UPROPERTY(config, EditAnywhere, Category = "Deverse Global Settings")
		bool bUseLocalServer = false;
};
