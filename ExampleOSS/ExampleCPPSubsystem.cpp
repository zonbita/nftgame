// Copyright June Rhodes. MIT Licensed.

#include "ExampleCPPSubsystem.h"

#include "Engine/LocalPlayer.h"
#include "Engine/NetConnection.h"
#include "ExampleCPPConfigLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UExampleCPPSubsystem::PostInitProperties()
{
    Super::PostInitProperties();

    if (this->HasAnyFlags(RF_ClassDefaultObject))
    {
        return;
    }

    check(this->GetWorld() != nullptr);

    IOnlineSubsystem *Subsystem = Online::GetSubsystem(this->GetWorld());
    if (Subsystem != nullptr && Subsystem->GetSubsystemName() == FName(TEXT("RedpointEOS")))
    {
        IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
        IOnlinePartyPtr Party = Subsystem->GetPartyInterface();
        IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
        IOnlinePresencePtr Presence = Subsystem->GetPresenceInterface();
        if (Party.IsValid())
        {
            Party->AddOnPartyInvitesChangedDelegate_Handle(
                FOnPartyInvitesChangedDelegate::CreateUObject(this, &UExampleCPPSubsystem::OnPartyInvitesChanged));
            Party->AddOnPartyDataReceivedDelegate_Handle(
                FOnPartyDataReceivedDelegate::CreateUObject(this, &UExampleCPPSubsystem::OnPartyDataReceived));
        }
        Session->AddOnSessionUserInviteAcceptedDelegate_Handle(FOnSessionUserInviteAcceptedDelegate::CreateUObject(
            this,
            &UExampleCPPSubsystem::OnSessionUserInviteAccepted));
        Session->AddOnJoinSessionCompleteDelegate_Handle(
            FOnJoinSessionCompleteDelegate::CreateUObject(this, &UExampleCPPSubsystem::OnSessionJoinedViaOverlay));
        if (Presence.IsValid())
        {
            Presence->AddOnPresenceReceivedDelegate_Handle(
                FOnPresenceReceivedDelegate::CreateUObject(this, &UExampleCPPSubsystem::OnPresenceReceived));
        }
    }
}