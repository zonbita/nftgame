// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletTestLog.h"

DEFINE_LOG_CATEGORY_STATIC(LogAutomationController, Log, All)
DEFINE_LOG_CATEGORY(LogEOSGauntlet)

void EmitTestStart(FString TestName)
{
    UE_LOG(LogAutomationController, Display, TEXT("Test Started. Name={%s}"), *TestName);
}

void EmitTestFail(FString TestName)
{
    UE_LOG(
        LogAutomationController,
        Display,
        TEXT("Test Completed. Result={Failed} Name={%s} Path={%s}"),
        *TestName,
        *TestName);
}

void EmitTestPass(FString TestName)
{
    UE_LOG(
        LogAutomationController,
        Display,
        TEXT("Test Completed. Result={Passed} Name={%s} Path={%s}"),
        *TestName,
        *TestName);
}