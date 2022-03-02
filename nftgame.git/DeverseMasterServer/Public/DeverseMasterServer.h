#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FDeverseMasterServerModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// Plugin settings

	virtual bool SupportsDynamicReloading() override;

	void RegisterSettings();

	void UnregisterSettings();

	bool HandleSettingsSaved();
};
