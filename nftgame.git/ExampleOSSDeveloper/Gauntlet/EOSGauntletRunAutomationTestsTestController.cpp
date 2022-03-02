// Copyright June Rhodes. MIT Licensed.

#include "EOSGauntletRunAutomationTestsTestController.h"

#include "AssetRegistryModule.h"
#include "EOSGauntletTestLog.h"
#include "IAutomationControllerModule.h"
#include "Misc/App.h"
#include "Misc/FilterCollection.h"
#include "Misc/Parse.h"

UEOSGauntletRunAutomationTestsTestController::UEOSGauntletRunAutomationTestsTestController()
    : UGauntletTestController(FObjectInitializer())
{
    this->Stage = EEOSGauntletTestStage::Init;
    this->AutomationController = nullptr;
}

void UEOSGauntletRunAutomationTestsTestController::OnTestsRefreshed()
{
    this->AutomationController->OnTestsRefreshed().Remove(this->OnTestsRefreshedDelegate);

    if (this->AutomationController->GetNumDeviceClusters() == 0)
    {
        this->MarkHeartbeatActive(TEXT("warning: Can't find device clusters for automation"));
        this->Stage = EEOSGauntletTestStage::AutomationFindWorkers;
        return;
    }

    this->MarkHeartbeatActive(TEXT("Got tests from automation controller."));

    TArray<FString> AllTestNames;
    this->AutomationController->SetFilter(MakeShared<AutomationFilterCollection>());
    this->AutomationController->SetVisibleTestsEnabled(true);
    this->AutomationController->GetEnabledTestNames(AllTestNames);

    TArray<FString> FilteredTestNames;
    for (const auto &TestName : AllTestNames)
    {
        if (TestName.StartsWith(TEXT("OnlineSubsystemEOS")))
        {
            FilteredTestNames.Add(TestName);
        }
    }

    this->AutomationController->StopTests();
    this->AutomationController->SetEnabledTests(FilteredTestNames);

    if (FilteredTestNames.Num() == 0)
    {
        this->MarkHeartbeatActive(TEXT("Error! Unable to locate any tests in automation controller."));
        UGauntletTestController::EndTest(2);
        return;
    }

    this->MarkHeartbeatActive(
        FString::Printf(TEXT("Running %d tests on automation controller..."), FilteredTestNames.Num()));

    this->Stage = EEOSGauntletTestStage::AutomationMonitorTests;
    this->AutomationController->RunTests();
}

void UEOSGauntletRunAutomationTestsTestController::OnTick(float TimeDelta)
{
    if (this->AutomationController.IsValid())
    {
        this->AutomationController->Tick();
    }

    switch (this->Stage)
    {
    case EEOSGauntletTestStage::Init: {
        this->MarkHeartbeatActive(TEXT("Obtaining automation controller..."));
        IAutomationControllerModule &Module = IAutomationControllerModule::Get();
        this->AutomationController = Module.GetAutomationController();
        this->AutomationController->Init();
        this->AutomationController->SetNumPasses(1);
        // At least one non-public platform crashes when trying to capture a stack trace in response to AddError, so we
        // turn it off stack capture here.
        FAutomationTestFramework::Get().SetCaptureStack(false);
        this->Stage = EEOSGauntletTestStage::AutomationInit;
        break;
    }
    case EEOSGauntletTestStage::AutomationInit: {
        FAssetRegistryModule &AssetRegistryModule =
            FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
        if (AssetRegistryModule.Get().IsLoadingAssets() == false)
        {
            this->MarkHeartbeatActive(TEXT("All assets are loaded, searching for automation workers..."));
            this->Stage = EEOSGauntletTestStage::AutomationFindWorkers;
        }
        break;
    }
    case EEOSGauntletTestStage::AutomationFindWorkers: {
        this->MarkHeartbeatActive(TEXT("Requesting available automation workers..."));
        this->OnTestsRefreshedDelegate = this->AutomationController->OnTestsRefreshed().AddUObject(
            this,
            &UEOSGauntletRunAutomationTestsTestController::OnTestsRefreshed);
        this->AutomationController->RequestAvailableWorkers(FApp::GetSessionId());
        this->FindWorkersTimeout = 2.0f;
        this->Stage = EEOSGauntletTestStage::AutomationRequestTests;
        break;
    }
    case EEOSGauntletTestStage::AutomationRequestTests: {
        // Wait for OnTestsRefreshed event.
        this->FindWorkersTimeout -= TimeDelta;
        if (this->FindWorkersTimeout < 0.0f)
        {
            this->OnTestsRefreshed();
        }
        break;
    }
    case EEOSGauntletTestStage::AutomationMonitorTests: {
        if (this->AutomationController->GetTestState() != EAutomationControllerModuleState::Running)
        {
            // We have finished the testing, and results are available
            this->MarkHeartbeatActive(TEXT("Automation tests have finished."));
            this->Stage = EEOSGauntletTestStage::End;
        }
        break;
    }
    case EEOSGauntletTestStage::End: {
        UGauntletTestController::EndTest(AutomationController->ReportsHaveErrors() ? 1 : 0);
        break;
    }
    default:
        checkf(false, TEXT("Unknown gauntlet test stage for EOS"));
    }
}