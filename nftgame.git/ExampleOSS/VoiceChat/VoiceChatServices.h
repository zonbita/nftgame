// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VoiceTypes.h"

#include "VoiceChatServices.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinedChannelsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnParticipantsChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAudioDevicesChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoiceChatChannelJoined, const FString &, ChannelName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnVoiceChatChannelExited,
    const FString &,
    ChannelName,
    const FVoiceChatResultCPP &,
    Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnVoiceChatPlayerAdded,
    const FString &,
    ChannelName,
    const FString &,
    PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnVoiceChatPlayerRemoved,
    const FString &,
    ChannelName,
    const FString &,
    PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnVoiceChatPlayerMuteUpdated,
    const FString &,
    ChannelName,
    const FString &,
    PlayerName,
    bool,
    bIsMuted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
    FOnVoiceChatPlayerTalkingUpdated,
    const FString &,
    ChannelName,
    const FString &,
    PlayerName,
    bool,
    bIsTalking);

UCLASS()
class EXAMPLEOSS_API AVoiceChatServices : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AVoiceChatServices();

    // ~ begin Actor overrides
    virtual void BeginPlay() override;
    virtual void BeginDestroy() override;
    // ~ end Actor overrides

    void SetupVoiceDelegates();

    class IVoiceChatUser *CreateUser();

    IVoiceChatUser *GetVoiceChatUser();

    // ~ begin VoiceChat interactions

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    TArray<FString> GetAvailableInputDevices();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    TArray<FString> GetAvailableOutputDevices();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    FString GetCurrentInputDevice();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    FString GetCurrentOutputDevice();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetInputDevice(const FString &Device);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetOutputDevice(const FString &Device);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    bool IsLoggedIn();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    FString GetLoggedInPlayerName();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    bool IsConnecting();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    bool IsConnected();

    UFUNCTION(BlueprintCallable, Category = "Voice")
    TArray<FString> GetJoinedChannels();

    UFUNCTION(BlueprintImplementableEvent, Category = "Voice", meta = (DisplayName = "GetJoinedChannels"))
    void BP_GetJoinedChannels(const TArray<FString> &JoinedChannels);

    UFUNCTION(BlueprintCallable, Category = "Voice")
    TArray<FString> GetPlayersInChannel(const FString &ChannelName);

    UFUNCTION(BlueprintImplementableEvent, Category = "Voice", meta = (DisplayName = "GetPlayersInChannel"))
    void BP_GetPlayersInChannel(const TArray<FString> &Players);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    float GetInputVolume();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetInputVolume(const float &Volume);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    float GetOutputVolume();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetOutputVolume(const float &Volume);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    bool GetInputMuted();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetInputMuted(bool bMuted);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    bool GetOutputMuted();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetOutputMuted(bool bMuted);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "Voice")
    EVoiceChatTransmitModeCPP GetTransmitMode();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Voice")
    void SetTransmitMode(const enum EVoiceChatTransmitModeCPP &TransmitMode);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice")
    bool IsPlayerMuted(const FString &PlayerName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Voice")
    bool IsPlayerTalking(const FString &PlayerName);

    // ~ begin VoiceChat interactions

    // ~ begin Delegate Bound Functions
    void HandleAudioDevicesChanged();

    void HandleVoiceChatChannelJoined(const FString &ChannelName);

    void HandleVoiceChatChannelExited(const FString &ChannelName, const struct FVoiceChatResult &Result);

    void HandleVoiceChatPlayerAdded(const FString &ChannelName, const FString &PlayerName);

    void HandleVoiceChatPlayerRemoved(const FString &ChannelName, const FString &PlayerName);

    void HandleVoiceChatPlayerMuteUpdated(const FString &ChannelName, const FString &PlayerName, const bool bIsMuted);

    void HandleVoiceChatPlayerTalkingUpdated(
        const FString &ChannelName,
        const FString &PlayerName,
        const bool bIsTalking);

    // ~ end Delegate Bound Functions

protected:
    class IVoiceChatUser *VoiceChatUser;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnJoinedChannelsChanged OnJoinedChannelsChanged;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnParticipantsChanged OnParticipantsChanged;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnAudioDevicesChanged OnAudioDevicesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatChannelJoined OnVoiceChatChannelJoined;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatChannelExited OnVoiceChatChannelExited;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatPlayerAdded OnVoiceChatPlayerAdded;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatPlayerRemoved OnVoiceChatPlayerRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatPlayerMuteUpdated OnVoiceChatPlayerMuteUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Voice")
    FOnVoiceChatPlayerTalkingUpdated OnVoiceChatPlayerTalkingUpdated;
};