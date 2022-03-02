// Copyright June Rhodes. MIT Licensed.

#include "ExampleOSSEarlyConfig.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/JsonSerializerMacros.h"

void FExampleOSSEarlyConfigModule::StartupModule()
{
#if PLATFORM_WINDOWS || PLATFORM_MAC || PLATFORM_LINUX
    // Forcibly set the Steam app ID since this doesn't seem to consistently work in development builds.

    FString AppId = TEXT("");
    GConfig->GetString(TEXT("OnlineSubsystemSteam"), TEXT("SteamDevAppId"), AppId, GEngineIni);

    if (!AppId.IsEmpty())
    {
        auto SteamAppIdFilename = FString::Printf(TEXT("%s%s"), FPlatformProcess::BaseDir(), TEXT("steam_appid.txt"));
        {
            bool bSandboxWasEnabled = IFileManager::Get().IsSandboxEnabled();
            IFileManager::Get().SetSandboxEnabled(false);
            IFileHandle *Handle = IPlatformFile::GetPlatformPhysical().OpenWrite(*SteamAppIdFilename, false, false);
            if (Handle)
            {
                FBufferArchive Archive;
                Archive.Serialize((void *)TCHAR_TO_ANSI(*AppId), AppId.Len());

                Handle->Write(Archive.GetData(), Archive.Num());
                delete Handle;
                Handle = nullptr;
            }
            IFileManager::Get().SetSandboxEnabled(bSandboxWasEnabled);
        }
    }
#endif
}

IMPLEMENT_MODULE(FExampleOSSEarlyConfigModule, ExampleOSSEarlyConfig);