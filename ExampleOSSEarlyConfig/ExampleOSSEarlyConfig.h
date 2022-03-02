// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDynamicConfig, All, All);

class FExampleOSSEarlyConfigModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;

    virtual bool IsGameModule() const override
    {
        return true;
    }
};