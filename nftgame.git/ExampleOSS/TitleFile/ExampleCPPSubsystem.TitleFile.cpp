// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"

#include "OnlineSubsystemUtils.h"

UCPPFileData *UCPPFileData::FromNative(const TArray<uint8> &InData)
{
    auto Ref = NewObject<UCPPFileData>();
    Ref->Data = InData;
    return Ref;
}

void UExampleCPPSubsystem::ReadTitleFile(const FString &FileName, FExampleCPPSubsystemReadTitleFileComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
    if (Title == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    Title->AddOnReadFileCompleteDelegate_Handle(FOnReadFileComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleReadTitleFileComplete,
        OnDone));

    Title->ReadFile(FileName);
}

void UExampleCPPSubsystem::EnumerateTitleFiles(FExampleCPPSubsystemEnumerateTitleFilesComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Subsystem is null"));
        return;
    }

    IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
    if (Title == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Unable to get title interface from subsystem"));
        return;
    }

    Title->AddOnEnumerateFilesCompleteDelegate_Handle(FOnEnumerateFilesComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleEnumerateTitleFilesCompelte,
        OnDone));

    // EOS does not support anything other than the default value being passed in here (unless you use the EOS
    // specific types for pagination).
    FPagedQuery PagedQuery = FPagedQuery();
    Title->EnumerateFiles(PagedQuery);
}

FString UExampleCPPSubsystem::GetFileContents(const FString &FileName)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        return FString();
    }

    IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
    if (Title == nullptr)
    {
        return FString();
    }

    TArray<uint8> FileContents;
    Title->GetFileContents(FileName, FileContents);

    UCPPFileData *FileData = UCPPFileData::FromNative(FileContents);
    return ReadFileDataAsString(FileData);
}

void UExampleCPPSubsystem::HandleEnumerateTitleFilesCompelte(
    bool WasSuccessful,
    const FString &Error,
    FExampleCPPSubsystemEnumerateTitleFilesComplete OnDone)
{
    if (WasSuccessful == false)
    {
        OnDone.ExecuteIfBound(false, TArray<FString>(), Error);
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Subsystem is null"));
        return;
    }

    IOnlineTitleFilePtr Title = Subsystem->GetTitleFileInterface();
    if (Title == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FString>(), TEXT("Unable to get title interface from subsystem"));
        return;
    }

    TArray<FCloudFileHeader> Files;
    Title->GetFileList(Files);
    TArray<FString> FileNames;
    FileNames.Reserve(Files.Num());

    for (auto &&It : Files)
    {
        FileNames.Add(It.FileName);
    }

    OnDone.ExecuteIfBound(true, FileNames, FString());
}

FString UExampleCPPSubsystem::ReadFileDataAsString(UCPPFileData *FileData)
{
    if (!IsValid(FileData))
    {
        return TEXT("");
    }
    FUTF8ToTCHAR Data((const ANSICHAR *)FileData->GetData().GetData(), FileData->GetData().Num());
    return FString(Data.Length(), Data.Get());
}

void UExampleCPPSubsystem::HandleReadTitleFileComplete(
    bool WasSuccessful,
    const FString &FileName,
    FExampleCPPSubsystemReadTitleFileComplete OnDone)
{
    OnDone.ExecuteIfBound(WasSuccessful, FileName);
}