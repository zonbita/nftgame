// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"

#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystemUtils.h"

FCloudFileDataCPP FCloudFileDataCPP::FromNative(const FCloudFileHeader &FileHeader)
{
    FCloudFileDataCPP Result;
    Result.Hash = FileHeader.Hash;
    Result.HashType = FileHeader.HashType;
    Result.DLName = FileHeader.DLName;
    Result.FileName = FileHeader.FileName;
    Result.FileSize = FileHeader.FileSize;
    Result.URL = FileHeader.URL;
    Result.ChunkID = FileHeader.ChunkID;
    return Result;
}

void UExampleCPPSubsystem::WriteUserFile(
    const FString &FileName,
    TArray<uint8> &FileData,
    FExampleCPPSubsystemWriteUserFileComplete OnDone)
{
    // note: this implementation expects the data to already be serialized before calling it.
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false);
        return;
    }

    UserCloud->AddOnWriteUserFileCompleteDelegate_Handle(FOnWriteUserFileComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleWriteUserFileComplete,
        OnDone));

    const int32 TotalFileSize = FileData.Num();
    UserCloud->AddOnWriteUserFileProgressDelegate_Handle(FOnWriteUserFileProgress::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleWriteUserFileProgress,
        TotalFileSize));

    UserCloud->AddOnWriteUserFileCanceledDelegate_Handle(
        FOnWriteUserFileCanceled::FDelegate::CreateUObject(this, &UExampleCPPSubsystem::HandleWriteUserFileCancelled));

    UserCloud->WriteUserFile(*Identity->GetUniquePlayerId(0), FileName, FileData);
}

void UExampleCPPSubsystem::WriteUserFileFromString(
    const FString &FileName,
    const FString &FileData,
    FExampleCPPSubsystemWriteUserFileComplete OnDone)
{
    if (FileData.IsEmpty())
    {
        OnDone.ExecuteIfBound(false);
        return;
    }

    // note: this implementation expects the data to already be in a string format
    FTCHARToUTF8 Data(*FileData);
    TArray<uint8> FileDataArray((uint8 *)Data.Get(), Data.Length());

    WriteUserFile(FileName, FileDataArray, OnDone);
}

void UExampleCPPSubsystem::WriteUserFileFromSaveGame(
    const FString &FileName,
    USaveGame *SaveGame,
    FExampleCPPSubsystemWriteUserFileComplete OnDone)
{
    TArray<uint8> FileData;
    UGameplayStatics::SaveGameToMemory(SaveGame, FileData);

    WriteUserFile(FileName, FileData, OnDone);
}

void UExampleCPPSubsystem::ReadUserFileAsString(
    const FString &FileName,
    FExampleCPPSubsystemReadUserFileAsStringComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    UserCloud->AddOnReadUserFileCompleteDelegate_Handle(FOnReadUserFileComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleReadUserFileComplete,
        OnDone));

    UserCloud->ReadUserFile(*Identity->GetUniquePlayerId(0), FileName);
}

void UExampleCPPSubsystem::ReadUserFileAsSaveGame(
    const FString &FileName,
    FExampleCPPSubsystemReadUserFileAsSaveGameComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    UserCloud->AddOnReadUserFileCompleteDelegate_Handle(FOnReadUserFileComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleReadUserFileComplete,
        OnDone));

    UserCloud->ReadUserFile(*Identity->GetUniquePlayerId(0), FileName);
}

void UExampleCPPSubsystem::EnumerateUserFiles(FExampleCPPSubsystemEnumerateUserFilesComplete OnDone)
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (Identity == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    UserCloud->AddOnEnumerateUserFilesCompleteDelegate_Handle(FOnEnumerateUserFilesComplete::FDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleEnumerateUserFilesComplete,
        OnDone));

    UserCloud->EnumerateUserFiles(*Identity->GetUniquePlayerId(0));
}

void UExampleCPPSubsystem::HandleWriteUserFileComplete(
    bool WasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName,
    FExampleCPPSubsystemWriteUserFileComplete OnDone)
{
    OnDone.ExecuteIfBound(WasSuccessful);
}

void UExampleCPPSubsystem::HandleWriteUserFileCancelled(
    bool WasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName)
{
    ExampleCPPSubsystemWriteUserFileCancelled.Broadcast(WasSuccessful);
}

void UExampleCPPSubsystem::HandleWriteUserFileProgress(
    int32 BytesWritten,
    const FUniqueNetId &UserId,
    const FString &FileName,
    int32 TotalFileSize)
{
    if (TotalFileSize > 0)
    {
        const float PercentageTransferred = BytesWritten / TotalFileSize;
        this->ExampleCPPSubsystemWriteUserFileProgress.Broadcast(PercentageTransferred, FileName);
    }
}

void UExampleCPPSubsystem::HandleReadUserFileComplete(
    bool WasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName,
    FExampleCPPSubsystemReadUserFileAsStringComplete OnDone)
{
    if (WasSuccessful == false)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, FString());
        return;
    }

    TArray<uint8> FileContents;
    if (UserCloud->GetFileContents(UserId, FileName, FileContents))
    {
        FUTF8ToTCHAR Data((const ANSICHAR *)FileContents.GetData(), FileContents.Num());
        OnDone.ExecuteIfBound(true, FString(Data.Length(), Data.Get()));
        return;
    }

    OnDone.ExecuteIfBound(false, FString());
}

void UExampleCPPSubsystem::HandleReadUserFileComplete(
    bool WasSuccessful,
    const FUniqueNetId &UserId,
    const FString &FileName,
    FExampleCPPSubsystemReadUserFileAsSaveGameComplete OnDone)
{
    if (WasSuccessful == false)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, nullptr);
        return;
    }

    TArray<uint8> FileContents;
    if (UserCloud->GetFileContents(UserId, FileName, FileContents))
    {
        USaveGame *Data = UGameplayStatics::LoadGameFromMemory(FileContents);
        OnDone.ExecuteIfBound(true, Data);
        return;
    }

    OnDone.ExecuteIfBound(false, nullptr);
}

void UExampleCPPSubsystem::HandleEnumerateUserFilesComplete(
    bool WasSuccessful,
    const FUniqueNetId &UserId,
    FExampleCPPSubsystemEnumerateUserFilesComplete OnDone)
{
    if (WasSuccessful == false)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
    if (UserCloud == nullptr)
    {
        OnDone.ExecuteIfBound(false, TArray<FCloudFileDataCPP>());
        return;
    }

    TArray<FCloudFileHeader> Files;
    UserCloud->GetUserFileList(UserId, Files);
    TArray<FCloudFileDataCPP> FileData;
    FileData.Reserve(Files.Num() - 1);
    for (auto &&It : Files)
    {
        FCloudFileDataCPP Data = FCloudFileDataCPP::FromNative(It);
        FileData.Add(Data);
    }

    OnDone.ExecuteIfBound(true, FileData);
}