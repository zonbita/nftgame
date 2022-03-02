// Copyright June Rhodes. MIT Licensed.

#include "../ExampleCPPSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "VoiceChat.h"
#include "VoiceChatServices.h"

void FVoiceChatChannel3dPropertiesCPP::ConvertAttenuationModelToBlueprintableResult(
    const EVoiceChatAttenuationModel &AttenuationModel,
    EVoiceChatAttenuationModelCPP &OutAttenuationModel)
{
    switch (AttenuationModel)
    {
    case EVoiceChatAttenuationModel::InverseByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModelCPP::InverseByDistance;
        break;
    case EVoiceChatAttenuationModel::LinearByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModelCPP::LinearByDistance;
        break;
    case EVoiceChatAttenuationModel::ExponentialByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModelCPP::ExponentialByDistance;
        break;
    case EVoiceChatAttenuationModel::None:
    default:
        OutAttenuationModel = EVoiceChatAttenuationModelCPP::None;
    }
}

void FVoiceChatChannel3dPropertiesCPP::ConvertBlueprintableAttenuationModelToAttenuationModel(
    const EVoiceChatAttenuationModelCPP &AttenuationModel,
    EVoiceChatAttenuationModel &OutAttenuationModel)
{
    switch (AttenuationModel)
    {
    case EVoiceChatAttenuationModelCPP::InverseByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModel::InverseByDistance;
        break;
    case EVoiceChatAttenuationModelCPP::LinearByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModel::LinearByDistance;
        break;
    case EVoiceChatAttenuationModelCPP::ExponentialByDistance:
        OutAttenuationModel = EVoiceChatAttenuationModel::ExponentialByDistance;
        break;
    case EVoiceChatAttenuationModelCPP::None:
    default:
        OutAttenuationModel = EVoiceChatAttenuationModel::None;
    }
}

void FVoiceChatChannel3dPropertiesCPP::ConvertBlueprintableChannelTypeToChannelType(
    const EVoiceChatChannelTypeCPP &ChannelType,
    EVoiceChatChannelType &OutChannelType)
{
    switch (ChannelType)
    {
    case EVoiceChatChannelTypeCPP::NonPositional:
        OutChannelType = EVoiceChatChannelType::NonPositional;
        break;
    case EVoiceChatChannelTypeCPP::Positional:
        OutChannelType = EVoiceChatChannelType::Positional;
        break;
    case EVoiceChatChannelTypeCPP::Echo:;
    default:
        OutChannelType = EVoiceChatChannelType::Echo;
    }
}

FVoiceChatChannel3dProperties FVoiceChatChannel3dPropertiesCPP::
    ConvertBlueprintableChatChannelPropertiesToChatChannelProperties(
        const FVoiceChatChannel3dPropertiesCPP &ChatChannel3dProperties)
{
    FVoiceChatChannel3dProperties VoiceChatChannel3dProperties;
    VoiceChatChannel3dProperties.Rolloff = ChatChannel3dProperties.Rolloff;
    VoiceChatChannel3dProperties.MinDistance = ChatChannel3dProperties.MinDistance;
    VoiceChatChannel3dProperties.MaxDistance = ChatChannel3dProperties.MaxDistance;
    ConvertBlueprintableAttenuationModelToAttenuationModel(
        ChatChannel3dProperties.AttenuationModel,
        VoiceChatChannel3dProperties.AttenuationModel);

    return VoiceChatChannel3dProperties;
}

void FVoiceChatResultCPP::ConvertVoiceChatResultToBlueprintableResult(
    const EVoiceChatResult &VoiceChatResult,
    EVoiceChatResultCPP &OutResult)
{
    switch (VoiceChatResult)
    {
    case EVoiceChatResult::Success:
        OutResult = EVoiceChatResultCPP::Success;
        break;
    case EVoiceChatResult::InvalidState:
        OutResult = EVoiceChatResultCPP::InvalidState;
        break;
    case EVoiceChatResult::NotInitialized:
        OutResult = EVoiceChatResultCPP::NotInitialized;
        break;
    case EVoiceChatResult::NotConnected:
        OutResult = EVoiceChatResultCPP::NotConnected;
        break;
    case EVoiceChatResult::NotLoggedIn:
        OutResult = EVoiceChatResultCPP::NotLoggedIn;
        break;
    case EVoiceChatResult::NotPermitted:
        OutResult = EVoiceChatResultCPP::NotPermitted;
        break;
    case EVoiceChatResult::Throttled:
        OutResult = EVoiceChatResultCPP::Throttled;
        break;
    case EVoiceChatResult::InvalidArgument:
        OutResult = EVoiceChatResultCPP::InvalidArgument;
        break;
    case EVoiceChatResult::CredentialsInvalid:
        OutResult = EVoiceChatResultCPP::CredentialsInvalid;
        break;
    case EVoiceChatResult::CredentialsExpired:
        OutResult = EVoiceChatResultCPP::CredentialsExpired;
        break;
    case EVoiceChatResult::ClientTimeout:
        OutResult = EVoiceChatResultCPP::ClientTimeout;
        break;
    case EVoiceChatResult::ServerTimeout:
        OutResult = EVoiceChatResultCPP::ServerTimeout;
        break;
    case EVoiceChatResult::DnsFailure:
        OutResult = EVoiceChatResultCPP::DnsFailure;
        break;
    case EVoiceChatResult::ConnectionFailure:
        OutResult = EVoiceChatResultCPP::ConnectionFailure;
        break;
    case EVoiceChatResult::ImplementationError:
    default:
        OutResult = EVoiceChatResultCPP::ImplementationError;
    }
}

void UExampleCPPSubsystem::CreateVoiceChatUser()
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        return;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        return;
    }

    IVoiceChatUser *VoiceChatUser = VoiceChat->CreateUser();
    if (VoiceChatUser == nullptr)
    {
        // Add logging here for handling
        return;
    }

    if (PrimaryVoiceUser == nullptr)
    {
        PrimaryVoiceUser = VoiceChatUser;
    }
}

void UExampleCPPSubsystem::LoginToVoice(
    AVoiceChatServices *VoiceChatService,
    FExampleCPPSubsystemOnVoiceChatLoginComplete OnDone)
{
    FVoiceChatResultCPP VoiceChatResult;

    if (VoiceChatService == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChat Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    const FPlatformUserId PlatformUserId = Identity->GetPlatformUserIdFromUniqueNetId(*Identity->GetUniquePlayerId(0));

    VoiceChatService->GetVoiceChatUser()->Login(
        PlatformUserId,
        Identity->GetUniquePlayerId(0)->ToString(),
        TEXT(""),
        FOnVoiceChatLoginCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleVoiceChatLoginComplete,
            OnDone));
}

void UExampleCPPSubsystem::LogoutOfVoice(
    AVoiceChatServices *VoiceChatService,
    FExampleCPPSubsystemOnVoiceChatLogoutComplete OnDone)
{
    FVoiceChatResultCPP VoiceChatResult;

    if (VoiceChatService == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    VoiceChatService->GetVoiceChatUser()->Logout(FOnVoiceChatLogoutCompleteDelegate::CreateUObject(
        this,
        &UExampleCPPSubsystem::HandleVoiceChatLogoutComplete,
        VoiceChatService,
        OnDone));
}

void UExampleCPPSubsystem::JoinVoiceChannel(
    AVoiceChatServices *VoiceChatService,
    const FString &ChannelName,
    const FString &ChannelCredentials,
    const EVoiceChatChannelTypeCPP ChannelType,
    const FVoiceChatChannel3dPropertiesCPP Channel3dProperties,
    FExampleCPPSubsystemOnVoiceChatChannelJoinedComplete OnDone)
{
    FVoiceChatResultCPP VoiceChatResult;

    if (VoiceChatService == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatService Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    if (VoiceChatService->GetVoiceChatUser() == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChatUser was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
    }

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Subsystem Pointer was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
    if (!Identity)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("Identity Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    if (Identity->GetUniquePlayerId(0).IsValid() == false)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::CredentialsInvalid;
        VoiceChatResult.ErrorDescription = TEXT("UniqueNetId of player 0 was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    if (VoiceChat == nullptr)
    {
        VoiceChatResult.bWasSuccessful = false;
        VoiceChatResult.VoiceChatResult = EVoiceChatResultCPP::ImplementationError;
        VoiceChatResult.ErrorDescription = TEXT("VoiceChat Interface was not valid");
        OnDone.ExecuteIfBound(FString(), VoiceChatResult);
        return;
    }

    EVoiceChatChannelType VoiceChatChannelType;
    FVoiceChatChannel3dPropertiesCPP::ConvertBlueprintableChannelTypeToChannelType(ChannelType, VoiceChatChannelType);
    const TOptional<FVoiceChatChannel3dProperties> ChatChannel3dProperties =
        FVoiceChatChannel3dPropertiesCPP::ConvertBlueprintableChatChannelPropertiesToChatChannelProperties(
            Channel3dProperties);

    VoiceChat->JoinChannel(
        ChannelName,
        ChannelCredentials,
        VoiceChatChannelType,
        FOnVoiceChatChannelJoinCompleteDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::HandleVoiceChatJoinChannelComplete,
            OnDone),
        ChatChannel3dProperties);
}

bool UExampleCPPSubsystem::IsConnected()
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        return false;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    return VoiceChat ? VoiceChat->IsConnected() : false;
}

bool UExampleCPPSubsystem::IsConnecting()
{
    IOnlineSubsystem *Subsystem = Online::GetSubsystem(GetWorld());
    if (Subsystem == nullptr)
    {
        return false;
    }

    IVoiceChat *VoiceChat = IVoiceChat::Get();
    return VoiceChat ? VoiceChat->IsConnecting() : false;
}

void UExampleCPPSubsystem::HandleVoiceChatLoginComplete(
    const FString &PlayerName,
    const FVoiceChatResult &Result,
    FExampleCPPSubsystemOnVoiceChatLoginComplete OnDone)
{
    const FVoiceChatResultCPP VoiceChatResult = FVoiceChatResultCPP(Result);
    OnDone.ExecuteIfBound(PlayerName, VoiceChatResult);
}

void UExampleCPPSubsystem::HandleVoiceChatLogoutComplete(
    const FString &PlayerName,
    const FVoiceChatResult &Result,
    AVoiceChatServices *VoiceChatService,
    FExampleCPPSubsystemOnVoiceChatLogoutComplete OnDone)
{
    if (PrimaryVoiceUser == VoiceChatService->GetVoiceChatUser())
    {
        PrimaryVoiceUser = nullptr;
    }

    VoiceChatService->Destroy();
    const FVoiceChatResultCPP VoiceChatResult = FVoiceChatResultCPP(Result);
    OnDone.ExecuteIfBound(PlayerName, VoiceChatResult);
}

void UExampleCPPSubsystem::HandleVoiceChatJoinChannelComplete(
    const FString &ChannelName,
    const FVoiceChatResult &Result,
    FExampleCPPSubsystemOnVoiceChatChannelJoinedComplete OnDone)
{
    const FVoiceChatResultCPP VoiceChatResult = FVoiceChatResultCPP(Result);
    OnDone.ExecuteIfBound(ChannelName, VoiceChatResult);
}