// Copyright Cosugames, LLC 2021


#include "DVUserSettings.h"
#include "Engine/GameEngine.h"
#include "Android/AndroidPlatformFramePacer.h"
#include "Kismet/KismetSystemLibrary.h"


const UDVUserSettings* UDVUserSettings::GetDVGameUserSettings()
{
	return Cast<UDVUserSettings>(GEngine->GetGameUserSettings());
}

float UDVUserSettings::GetMasterValue() const
{
	return MasterValue;
}

void UDVUserSettings::SetMasterValue(float Value)
{
	MasterValue = Value;
}

float UDVUserSettings::GetMusicValue() const
{
	return MusicValue;
}

void UDVUserSettings::SetMusicValue(float Value)
{
	MusicValue = Value;
}

float UDVUserSettings::GetSoundFXValue() const
{
	return SoundFXValue;
}

void UDVUserSettings::SetSoundFXValue(float Value)
{
	SoundFXValue = Value;
}

float UDVUserSettings::GetRadioValue() const
{
	return RadioValue;
}

void UDVUserSettings::SetRadioValue(float Value)
{
	RadioValue = Value;
}

float UDVUserSettings::GetVoiceValue() const
{
	return VoiceValue;
}

void UDVUserSettings::SetVoiceValue(float Value)
{
	VoiceValue = Value;
}
