// Copyright Cosugames, LLC 2021

#include "DeverseMasterServer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "DVGlobalSettings.h"

IMPLEMENT_MODULE(FDeverseMasterServerModule, DeverseMasterServer);

#define LOCTEXT_NAMESPACE "FDeverseMasterServerModule"

void FDeverseMasterServerModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Deverse Master Server Module started")); 
	RegisterSettings();
}

void FDeverseMasterServerModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Deverse Master Server Module closed"));
	if (UObjectInitialized()) {
		UnregisterSettings();
	}
}

bool FDeverseMasterServerModule::SupportsDynamicReloading()
{
	return true;
}

void FDeverseMasterServerModule::RegisterSettings()
{
	// Registering some settings is just a matter of exposing the default UObject of
	// your desired class, feel free to add here all those settings you want to expose
	// to your LDs or artists.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		// Register the settings
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Engine", "DeverseMasterServer",
			LOCTEXT("RuntimeSettingsName", "Deverse Master Server"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Deverse Master Server parameters"),
			GetMutableDefault<UDVGlobalSettings>()
		);

		// Register the save handler to your settings, you might want to use it to
		// validate those or just act to settings changes.
		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FDeverseMasterServerModule::HandleSettingsSaved);
		}
	}
}

void FDeverseMasterServerModule::UnregisterSettings()
{
	// Ensure to unregister all of your registered settings here, hot-reload would
	// otherwise yield unexpected results.

	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Engine", "DeverseMasterServer");
	}
}

bool FDeverseMasterServerModule::HandleSettingsSaved()
{
	UDVGlobalSettings* Settings = GetMutableDefault<UDVGlobalSettings>();
	bool ResaveSettings = false;

	// You can put any validation code in here and resave the settings in case an invalid
	// value has been entered

	if (ResaveSettings)
	{
		Settings->SaveConfig();
	}

	return true;
}
