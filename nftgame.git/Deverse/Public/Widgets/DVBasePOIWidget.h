// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DVBasePOIWidget.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPOIGeoInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
		FRotator Rotation = FRotator::ZeroRotator;

	FPOIGeoInfo()
	{
	}
};

UCLASS()
class DEVERSE_API UDVBasePOIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		class UDVBasePOIComponent* OwnerComp = nullptr;

	UPROPERTY(BlueprintReadWrite)
		class UDVBaseMapWidget* TargetMap = nullptr;

	UPROPERTY(BlueprintReadWrite)
		class UCurveFloat* ZoomScaleCurve = nullptr;

	UPROPERTY()
		class ADVPlayerState* TargetPlayerState = nullptr;

	virtual FPOIGeoInfo GetPOIGeoInfo();

	virtual void AddPOIToMap(class UDVBaseMapWidget* Map, class UDVBasePOIComponent* OwnerComponent);

	void RemovePOIFromMap();

	void RemovePOIFromMapImmediately();

	virtual void RemovePOIFromMapCompletely();

	virtual void UIUpdatesPOIState();

	UFUNCTION(BlueprintCallable)
		virtual void UpdatePOIOnMaps();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesYaw(float Yaw);

	UFUNCTION(BlueprintCallable)
		float GetAcceptedScaleValue(float CurrentScaleValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesScale(float ZoomValue);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesCutOnMiniMap(float Scale, float X, float Y);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UIUpdatesYawWithGlobalYaw(float Yaw, float GlobalYaw);

	UFUNCTION(BlueprintPure)
		UDVBasePOIComponent* GetOwnerPOIComponent();

	UFUNCTION(BlueprintPure)
		UDVBaseMapWidget* GetTargetMap();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UISetupSize(float WidthPixel);

	void CalculateXYOfMinimapOnMaterial(bool InbIsLeft, float InDegrees, float InDistance, float InObjectRadius, float& OutX, float& OutY);

	UFUNCTION(BlueprintCallable)
		void CodeShowPOI();

	void ActuallyShowPOI();

	UFUNCTION(BlueprintCallable)
		void CodeHidePOI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void UIShowPOI();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void UIHidePOI();

	virtual void HandleEventPlayerRespawn(ADVPlayerState* InPlayerState);
private:
	FTimerHandle TimerUpdatePOIOnMaps;
	FTimerHandle DelayToRemovePOITimer;
	FPOIGeoInfo LastActorGeoInfo;
	FTimerHandle ShowPOITimer;
	bool bIsHidden = true;
};
