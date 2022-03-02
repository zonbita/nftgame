// Copyright June Rhodes. MIT Licensed.

#include "VoiceChatServices.h"
#include "ExampleOSS/ExampleCPPSubsystem.h"
#include "VoiceChat.h"

AVoiceChatServices::AVoiceChatServices()
{
}

void AVoiceChatServices::BeginPlay()
{
    Super::BeginPlay();

    IVoiceChatUser *VoiceUser = CreateUser();
    if (VoiceUser == nullptr)
    {
        VoiceChatUser = VoiceUser;
    }
    else
    {
        UExampleCPPSubsystem *CPPSubsystem =
            GetGameInstance() ? GetGameInstance()->GetSubsystem<UExampleCPPSubsystem>() : nullptr;
        if (CPPSubsystem)
        {
            VoiceChatUser = CPPSubsystem->PrimaryVoiceUser;
        }
    }

    SetupVoiceDelegates();
}

void AVoiceChatServices::BeginDestroy()
{
    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat && VoiceChatUser)
    {
        VoiceChat->ReleaseUser(VoiceChatUser);
        VoiceChatUser = nullptr;
    }

    Super::BeginDestroy();
}

void AVoiceChatServices::SetupVoiceDelegates()
{
    if (VoiceChatUser == nullptr)
    {
        return;
    }

    VoiceChatUser->OnVoiceChatAvailableAudioDevicesChanged().AddUObject(
        this,
        &AVoiceChatServices::HandleAudioDevicesChanged);
    VoiceChatUser->OnVoiceChatChannelJoined().AddUObject(this, &AVoiceChatServices::HandleVoiceChatChannelJoined);
    VoiceChatUser->OnVoiceChatChannelExited().AddUObject(this, &AVoiceChatServices::HandleVoiceChatChannelExited);
    VoiceChatUser->OnVoiceChatPlayerAdded().AddUObject(this, &AVoiceChatServices::HandleVoiceChatPlayerAdded);
    VoiceChatUser->OnVoiceChatPlayerRemoved().AddUObject(this, &AVoiceChatServices::HandleVoiceChatPlayerRemoved);
    VoiceChatUser->OnVoiceChatPlayerMuteUpdated().AddUObject(
        this,
        &AVoiceChatServices::HandleVoiceChatPlayerMuteUpdated);
    VoiceChatUser->OnVoiceChatPlayerTalkingUpdated().AddUObject(
        this,
        &AVoiceChatServices::HandleVoiceChatPlayerTalkingUpdated);
}

IVoiceChatUser *AVoiceChatServices::CreateUser()
{
    UExampleCPPSubsystem *CPPSubsystem =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<UExampleCPPSubsystem>() : nullptr;
    if (CPPSubsystem == nullptr)
    {
        return nullptr;
    }

    CPPSubsystem->CreateVoiceChatUser();
    return CPPSubsystem->PrimaryVoiceUser;
}

IVoiceChatUser *AVoiceChatServices::GetVoiceChatUser()
{
    return VoiceChatUser;
}

TArray<FString> AVoiceChatServices::GetAvailableInputDevices_Implementation()
{
    TArray<FVoiceChatDeviceInfo> VoiceChatDevices = VoiceChatUser->GetAvailableInputDeviceInfos();
    TArray<FString> InputDevices;
    InputDevices.Reserve(VoiceChatDevices.Num());
    for (auto &&Device : VoiceChatDevices)
    {
        InputDevices.Add(Device.DisplayName + TEXT("--") + Device.Id);
    }

    return InputDevices;
}

TArray<FString> AVoiceChatServices::GetAvailableOutputDevices_Implementation()
{
    TArray<FVoiceChatDeviceInfo> VoiceChatDevices = VoiceChatUser->GetAvailableOutputDeviceInfos();
    TArray<FString> OutputDevices;
    OutputDevices.Reserve(VoiceChatDevices.Num());
    for (auto &&Device : VoiceChatDevices)
    {
        OutputDevices.Add(Device.DisplayName + TEXT("--") + Device.Id);
    }

    return OutputDevices;
}

FString AVoiceChatServices::GetCurrentInputDevice_Implementation()
{
    const FVoiceChatDeviceInfo VoiceChatDevice = VoiceChatUser->GetInputDeviceInfo();
    FString DeviceInfo = VoiceChatDevice.DisplayName + TEXT("--") + VoiceChatDevice.Id;
    return DeviceInfo;
}

FString AVoiceChatServices::GetCurrentOutputDevice_Implementation()
{
    const FVoiceChatDeviceInfo VoiceChatDevice = VoiceChatUser->GetOutputDeviceInfo();
    FString DeviceInfo = VoiceChatDevice.DisplayName + TEXT("--") + VoiceChatDevice.Id;
    return DeviceInfo;
}

void AVoiceChatServices::SetInputDevice_Implementation(const FString &Device)
{
    FString DeviceName;
    FString DeviceId;
    Device.Split(TEXT("--"), &DeviceName, &DeviceId);
    VoiceChatUser->SetInputDeviceId(DeviceId);
}

void AVoiceChatServices::SetOutputDevice_Implementation(const FString &Device)
{
    FString DeviceName;
    FString DeviceId;
    Device.Split(TEXT("--"), &DeviceName, &DeviceId);
    VoiceChatUser->SetOutputDeviceId(DeviceId);
}

bool AVoiceChatServices::IsLoggedIn_Implementation()
{
    return VoiceChatUser->IsLoggedIn();
}

FString AVoiceChatServices::GetLoggedInPlayerName_Implementation()
{
    return VoiceChatUser->GetLoggedInPlayerName();
}

bool AVoiceChatServices::IsConnecting_Implementation()
{
    UExampleCPPSubsystem *CPPSubsystem =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<UExampleCPPSubsystem>() : nullptr;
    return CPPSubsystem ? CPPSubsystem->IsConnecting() : false;
}

bool AVoiceChatServices::IsConnected_Implementation()
{
    UExampleCPPSubsystem *CPPSubsystem =
        GetGameInstance() ? GetGameInstance()->GetSubsystem<UExampleCPPSubsystem>() : nullptr;
    return CPPSubsystem ? CPPSubsystem->IsConnected() : false;
}

TArray<FString> AVoiceChatServices::GetJoinedChannels()
{
    const TArray<FString> JoinedChannels = VoiceChatUser->GetChannels();
    BP_GetJoinedChannels(JoinedChannels);
    return JoinedChannels;
}

TArray<FString> AVoiceChatServices::GetPlayersInChannel(const FString &ChannelName)
{
    const TArray<FString> Players = VoiceChatUser->GetPlayersInChannel(ChannelName);
    BP_GetPlayersInChannel(Players);
    return Players;
}

float AVoiceChatServices::GetInputVolume_Implementation()
{
    return VoiceChatUser->GetAudioInputVolume();
}

void AVoiceChatServices::SetInputVolume_Implementation(const float &Volume)
{
    VoiceChatUser->SetAudioInputVolume(Volume);
}

float AVoiceChatServices::GetOutputVolume_Implementation()
{
    return VoiceChatUser->GetAudioOutputVolume();
}

void AVoiceChatServices::SetOutputVolume_Implementation(const float &Volume)
{
    VoiceChatUser->SetAudioOutputVolume(Volume);
}

bool AVoiceChatServices::GetInputMuted_Implementation()
{
    return VoiceChatUser->GetAudioInputDeviceMuted();
}

void AVoiceChatServices::SetInputMuted_Implementation(bool bMuted)
{
    VoiceChatUser->SetAudioInputDeviceMuted(bMuted);
}

bool AVoiceChatServices::GetOutputMuted_Implementation()
{
    return VoiceChatUser->GetAudioOutputDeviceMuted();
}

void AVoiceChatServices::SetOutputMuted_Implementation(bool bMuted)
{
    VoiceChatUser->SetAudioOutputDeviceMuted(bMuted);
}

EVoiceChatTransmitModeCPP AVoiceChatServices::GetTransmitMode_Implementation()
{
    const EVoiceChatTransmitMode TransmitMode = VoiceChatUser->GetTransmitMode();
    EVoiceChatTransmitModeCPP OutTransmitMode;
    switch (TransmitMode)
    {
    case EVoiceChatTransmitMode::All:
        OutTransmitMode = EVoiceChatTransmitModeCPP::All;
        break;
    case EVoiceChatTransmitMode::Channel:
        OutTransmitMode = EVoiceChatTransmitModeCPP::Channel;
        break;
    case EVoiceChatTransmitMode::None:
    default:
        OutTransmitMode = EVoiceChatTransmitModeCPP::None;
    }

    return OutTransmitMode;
}

void AVoiceChatServices::SetTransmitMode_Implementation(const EVoiceChatTransmitModeCPP &TransmitMode)
{
    switch (TransmitMode)
    {
    case EVoiceChatTransmitModeCPP::All: {
        VoiceChatUser->TransmitToAllChannels();
        break;
    }
    case EVoiceChatTransmitModeCPP::Channel: {
        // Default to transmitting to the first channel the user is in
        TArray<FString> ChannelNames = GetJoinedChannels();
        if (ChannelNames.IsValidIndex(0))
        {
            /**
             * @note EOS Can transmit to several specific channels at once. This example is just using the first
             * channel the user is in. If you want a user to transmit to multiple voice channels at once, you'll
             * need to pick the specific channel names you want the user to transmit to and then call
             * TransmitToSpecificChannel for each channel.
             *
             * Additionally, you need to call TransmitToNoChannels first to clear the slate before you specify a channel
             * to transmit to.
             */
            VoiceChatUser->TransmitToNoChannels();
            VoiceChatUser->TransmitToSpecificChannel(ChannelNames[0]);
        }
        break;
    }
    case EVoiceChatTransmitModeCPP::None:
    default: {
        VoiceChatUser->TransmitToNoChannels();
    }
    }
}

bool AVoiceChatServices::IsPlayerMuted(const FString &PlayerName)
{
    return VoiceChatUser->IsPlayerMuted(PlayerName);
}

bool AVoiceChatServices::IsPlayerTalking(const FString &PlayerName)
{
    return VoiceChatUser->IsPlayerTalking(PlayerName);
}

void AVoiceChatServices::HandleAudioDevicesChanged()
{
    OnAudioDevicesChanged.Broadcast();
}

void AVoiceChatServices::HandleVoiceChatChannelJoined(const FString &ChannelName)
{
    OnVoiceChatChannelJoined.Broadcast(ChannelName);
}

void AVoiceChatServices::HandleVoiceChatChannelExited(const FString &ChannelName, const FVoiceChatResult &Result)
{
    FVoiceChatResultCPP VoiceChatResultCPP = FVoiceChatResult(Result);
    OnVoiceChatChannelExited.Broadcast(ChannelName, VoiceChatResultCPP);
}

void AVoiceChatServices::HandleVoiceChatPlayerAdded(const FString &ChannelName, const FString &PlayerName)
{
    OnVoiceChatPlayerAdded.Broadcast(ChannelName, PlayerName);
}

void AVoiceChatServices::HandleVoiceChatPlayerRemoved(const FString &ChannelName, const FString &PlayerName)
{
    OnVoiceChatPlayerRemoved.Broadcast(ChannelName, PlayerName);
}

void AVoiceChatServices::HandleVoiceChatPlayerMuteUpdated(
    const FString &ChannelName,
    const FString &PlayerName,
    bool bIsMuted)
{
    OnVoiceChatPlayerMuteUpdated.Broadcast(ChannelName, PlayerName, bIsMuted);
}

void AVoiceChatServices::HandleVoiceChatPlayerTalkingUpdated(
    const FString &ChannelName,
    const FString &PlayerName,
    bool bIsTalking)
{
    OnVoiceChatPlayerTalkingUpdated.Broadcast(ChannelName, PlayerName, bIsTalking);
}