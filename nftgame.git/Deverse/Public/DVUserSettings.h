// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "DVUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class DEVERSE_API UDVUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

	float MasterValue, MusicValue, SoundFXValue, RadioValue, VoiceValue;
	
public:
	
	UFUNCTION(BlueprintCallable, Category=Settings)
	static const UDVUserSettings* GetDVGameUserSettings();
	
	UFUNCTION(BlueprintCallable, Category=Settings)
	float GetMasterValue() const;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMasterValue(float Value);

	UFUNCTION(BlueprintCallable, Category=Settings)
	float GetMusicValue() const;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMusicValue(float Value);

	UFUNCTION(BlueprintCallable, Category=Settings)
	float GetSoundFXValue() const;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetSoundFXValue(float Value);

	UFUNCTION(BlueprintCallable, Category=Settings)
	float GetRadioValue() const;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetRadioValue(float Value);

	UFUNCTION(BlueprintCallable, Category=Settings)
	float GetVoiceValue() const;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetVoiceValue(float Value);
};
