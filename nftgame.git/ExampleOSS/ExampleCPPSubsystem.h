// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "Containers/Set.h"
#include "CoreMinimal.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineLeaderboardInterface.h"
#include "Interfaces/OnlinePartyInterface.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineStatsInterface.h"
#include "Interfaces/OnlineUserInterface.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Achievements/AchievementsTypes.h"
#include "Auth/AuthTypes.h"
#include "Avatar/AvatarTypes.h"
#include "ExampleCPPBeacon.h"
#include "Friends/FriendsTypes.h"
#include "Leaderboards/LeaderboardsTypes.h"
#include "Lobbies/LobbiesTypes.h"
#include "Multiplayer/MultiplayerTypes.h"
#include "Parties/PartiesTypes.h"
#include "Presence/PresenceTypes.h"
#include "Session/SessionTypes.h"
#include "Stats/StatsTypes.h"
#include "TitleFile/TitleFileTypes.h"
#include "UserCloud/UserCloudTypes.h"
#include "UserInfo/UserInfoTypes.h"
#include "VoiceChat/VoiceTypes.h"

#include "ExampleCPPSubsystem.generated.h"

/**
 * This demo project exposes some blueprint nodes so they're accessible from UMG. It's not meant to be an example of how
 * you can access the online subsystem from blueprints.
 *
 * If you're interested in using the online subsystem from blueprints, see the "ExampleBlueprints_EOS_..." project
 * instead.
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemLogoutComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FExampleCPPSubsystemLoginComplete, bool, WasSuccessful, FString, ErrorMessage);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemQueryAchievementsComplete,
    bool,
    bWasSuccessful,
    const TArray<FExampleCPPAchievement> &,
    Achievements);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemReadFriendsComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemInviteFriendComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FExampleCPPSubsystemInvitationsChanged);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemUpdatePresenceComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemQueryPresenceComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FExampleCPPSubsystemPresenceUpdated, FString, UserId, FString, Status);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemQueryUserInfoComplete, FString, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemQueryUserIdMappingComplete, FString, Result);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemQueryExternalIdMappingsComplete, FString, Result);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemQueryStatsComplete,
    bool,
    bWasSuccessful,
    const TArray<FExampleCPPStat> &,
    QueriedStats);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemIngestStatComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemFindSessionsComplete,
    bool,
    WasSuccessful,
    const TArray<UExampleCPPSessionSearchResult *> &,
    Results);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemCreateSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemDestroySessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemJoinSessionComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExampleCPPSubsystemPartyLeaderWasFollowedToSession, FName, SessionName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExampleCPPSubsystemSessionJoinedMapTravelRequired, FName, SessionName);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemCreatePartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemJoinPartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemLeavePartyComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemKickMemberComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemQueryLeaderboardsComplete,
    bool,
    bWasSuccessful,
    const TArray<FExampleCPPLeaderboardEntry> &,
    LeaderboardEntries);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemReadTitleFileComplete,
    bool,
    bWasSuccessful,
    const FString &,
    FileName);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
    FExampleCPPSubsystemEnumerateTitleFilesComplete,
    bool,
    bWasSuccessful,
    const TArray<FString> &,
    FileNames,
    const FString &,
    Error);
DECLARE_DYNAMIC_DELEGATE_OneParam(FExampleCPPSubsystemWriteUserFileComplete, bool, WasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FExampleCPPSubsystemWriteUserFileProgress,
    float,
    Percent,
    const FString &,
    FileName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExampleCPPSubsystemWriteUserFileCancelled, bool, bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemReadUserFileAsStringComplete,
    bool,
    WasSuccessful,
    const FString &,
    FileData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemReadUserFileAsSaveGameComplete,
    bool,
    WasSuccessful,
    class USaveGame *,
    FileData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemEnumerateUserFilesComplete,
    bool,
    WasSuccessful,
    const TArray<FCloudFileDataCPP> &,
    FileData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemGetAvatarComplete,
    bool,
    bWasSuccessful,
    const TSoftObjectPtr<class UTexture2D> &,
    ResultTexture);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemOnVoiceChatLoginComplete,
    const FString &,
    PlayerName,
    const FVoiceChatResultCPP &,
    VoiceChatLogin);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemOnVoiceChatLogoutComplete,
    const FString &,
    PlayerName,
    const FVoiceChatResultCPP &,
    VoiceChatLogin);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemOnVoiceChatChannelJoinedComplete,
    const FString &,
    ChannelName,
    const FVoiceChatResultCPP &,
    VoiceChatResult);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubsystemOnCreateOrConnectLobbyComplete,
    const FExampleCPPCreateOrConnectLobbyResult &,
    LobbyResult,
    bool,
    bWasSucessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubssytemOnSearchLobbyComplete,
    const FExampleCPPLobbySearchResult &,
    SearchResult,
    bool,
    bWasSuccessful);
DECLARE_DYNAMIC_DELEGATE_TwoParams(
    FExampleCPPSubssytemOnUpdateLobbyComplete,
    const FExampleCPPUpdateLobbyResult &,
    SearchResult,
    bool,
    bWasSuccessful);

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void PostInitProperties() override;

    /********** ExampleCPPSubsystem.Achievements.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryAchievements(const UObject *WorldContextObject, FExampleCPPSubsystemQueryAchievementsComplete OnDone);

private:
    void HandleQueryAchievementDescriptions(
        const FUniqueNetId &UserId,
        const bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryAchievementsComplete OnDone);
    void HandleQueryAchievements(
        const FUniqueNetId &UserId,
        const bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryAchievementsComplete OnDone);

    /********** ExampleCPPSubsystem.Auth.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool IsSignedIn(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetLoggedInUsername(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetLoggedInProductUserId(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetLoggedInEpicAccountId(const UObject *WorldContextObject);

    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
    FString GetLoggedInAuthAttribute(const UObject *WorldContextObject, const FString &InAuthAttrName);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FUniqueNetIdRepl GetLoggedInUserId(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartLogin(const UObject *WorldContextObject, int32 LocalUserNum, FExampleCPPSubsystemLoginComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool CanLinkToEpicGamesAccount(const UObject *WorldContextObject);

private:
    FDelegateHandle LoginDelegateHandle;
    void HandleLoginComplete(
        int32 _UnusedLocalUserNum,
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &Error,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemLoginComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartLogout(const UObject *WorldContextObject, FExampleCPPSubsystemLogoutComplete OnDone);

private:
    FDelegateHandle LogoutDelegateHandle;
    void HandleLogoutComplete(
        int32 _UnusedLocalUserNum,
        bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemLogoutComplete OnDone);

    /********** ExampleCPPSubsystem.Avatar.cpp **********/

    UFUNCTION(BlueprintCallable, Category = "Avatar")
    void GetAvatar(
        int32 LocalUserId,
        const FUniqueNetIdRepl &TargetUserId,
        class UTexture2D *DefaultTexture,
        FExampleCPPSubsystemGetAvatarComplete OnDone);

    void HandleGetAvatarComplete(
        bool bWasSucessful,
        TSoftObjectPtr<class UTexture2D> ResultTexture,
        FExampleCPPSubsystemGetAvatarComplete OnDone);

    /********** ExampleCPPSubsystem.Friends.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<UExampleCPPFriend *> GetFriends(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartReadFriends(const UObject *WorldContextObject, FExampleCPPSubsystemReadFriendsComplete OnDone);

private:
    void HandleReadFriendsComplete(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const FString &ListName,
        const FString &ErrorStr,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemReadFriendsComplete OnDone);

    /********** ExampleCPPSubsystem.Leaderboards.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryFriendsLeaderboards(
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryLeaderboardsComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryGlobalLeaderboards(
        const UObject *WorldContextObject,
        const FString &LeaderboardId,
        FExampleCPPSubsystemQueryLeaderboardsComplete OnDone);

private:
    FDelegateHandle QueryLeaderboardsDelegateHandle;
    void HandleLeaderboardResult(
        const bool bWasSuccessful,
        FOnlineLeaderboardReadRef LeaderboardRef,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryLeaderboardsComplete OnDone);

    /********** ExampleCPPSubsystem.Lobbies.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void CreateLobby(const int64 &ExampleAttributeData, FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void ConnectLobby(class UExampleCPPLobbyId *LobbyId, FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void DisconnectLobby(class UExampleCPPLobbyId *LobbyId, FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void SearchLobby(FExampleCPPSubssytemOnSearchLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void UpdateLobby(
        class UExampleCPPLobbyId *LobbyId,
        const int64 &ExampleAttributeData,
        FExampleCPPSubssytemOnUpdateLobbyComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool GetMemberCount(class UExampleCPPLobbyId *LobbyId, int32 &OutMemberCount);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool GetMemberUserId(class UExampleCPPLobbyId *LobbyId, const int32 &MemberIndex, FString &OutMemberId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    class UExampleCPPPartyInvite *ConvertLobbyIdToPartyJoinInfo(class UExampleCPPLobbyId *LobbyId);

protected:
    void HandleCreateLobbyComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby,
        FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    void HandleConnectLobbyComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TSharedPtr<class FOnlineLobby> &Lobby,
        FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    void HandleDisconnectLobbyComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        FExampleCPPSubsystemOnCreateOrConnectLobbyComplete OnDone);

    void HandleSearchLobbyComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        const TArray<TSharedRef<const FOnlineLobbyId>> &LobbyIds,
        FExampleCPPSubssytemOnSearchLobbyComplete OnDone);

    void HandleUpdateLobbyComplete(
        const FOnlineError &Error,
        const FUniqueNetId &UserId,
        FExampleCPPSubssytemOnUpdateLobbyComplete OnDone);

    /********** ExampleCPPSubsystem.Multiplayer.cpp **********/

public:
    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
    void SeamlessTravel(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "WorldContextObject"))
    void NonSeamlessTravel(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable)
    void BeginRecordingReplay(AGameModeBase *GameMode);

    /********** ExampleCPPSubsystem.Parties.cpp **********/

private:
    TSet<FString> SessionFollowConsumedIds;

    void OnPartyDataReceived(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FName &Namespace,
        const FOnlinePartyData &PartyData);

    void OnPartyInvitesChanged(const FUniqueNetId &LocalUserId);

public:
    UPROPERTY(BlueprintAssignable)
    FExampleCPPSubsystemInvitationsChanged OnInvitationsChanged;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<UExampleCPPPartyInvite *> GetInvitations(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<UExampleCPPPartyId *> GetJoinedParties(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void RequestPartyMembersFollowLeader(const UObject *WorldContextObject, FName SessionName);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<UExampleCPPPartyMemberId *> GetPartyMembers(const UObject *WorldContextObject, UExampleCPPPartyId *PartyId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    bool IsPartyLeader(const UObject *WorldContextObject, UExampleCPPPartyId *PartyId);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartCreateParty(
        const UObject *WorldContextObject,
        int PartyTypeId,
        FExampleCPPSubsystemCreatePartyComplete OnDone);

private:
    void HandleCreatePartyComplete(
        const FUniqueNetId &LocalUserId,
        const TSharedPtr<const FOnlinePartyId> &PartyId,
        const ECreatePartyCompletionResult Result,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemCreatePartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartJoinParty(
        const UObject *WorldContextObject,
        UExampleCPPPartyInvite *Invite,
        FExampleCPPSubsystemJoinPartyComplete OnDone);

private:
    void HandleJoinPartyComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const EJoinPartyCompletionResult Result,
        const int32 NotApprovedReason,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemJoinPartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartLeaveParty(
        const UObject *WorldContextObject,
        UExampleCPPPartyId *PartyId,
        FExampleCPPSubsystemLeavePartyComplete OnDone);

private:
    void HandleLeavePartyComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const ELeavePartyCompletionResult Result,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemLeavePartyComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartKickMember(
        const UObject *WorldContextObject,
        UExampleCPPPartyId *PartyId,
        UExampleCPPPartyMemberId *MemberId,
        FExampleCPPSubsystemKickMemberComplete OnDone);

private:
    void HandleKickMemberComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &MemberId,
        const EKickMemberCompletionResult Result,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemKickMemberComplete OnDone);

    
public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartInviteFriend(
        const UObject *WorldContextObject,
        UExampleCPPPartyId *PartyId,
        UExampleCPPFriend *Friend,
        FExampleCPPSubsystemInviteFriendComplete OnDone);

    ///////
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void CallInviteFriend(
        const UObject *WorldContextObject,
        UExampleCPPPartyId *PartyId,
        UExampleCPPPartyMemberId *MemberId,
        FExampleCPPSubsystemInviteFriendComplete OnDone);

private:
    void HandleInviteFriendComplete(
        const FUniqueNetId &LocalUserId,
        const FOnlinePartyId &PartyId,
        const FUniqueNetId &RecipientId,
        const ESendPartyInvitationCompletionResult Result,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemInviteFriendComplete OnDone);

    /********** ExampleCPPSubsystem.Presence.cpp **********/

private:
    void OnPresenceReceived(const class FUniqueNetId &UserId, const TSharedRef<FOnlineUserPresence> &Presence);

public:
    UPROPERTY(BlueprintAssignable)
    FExampleCPPSubsystemPresenceUpdated PresenceUpdated;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartUpdatePresence(
        const UObject *WorldContextObject,
        const FString &NewPresenceString,
        FExampleCPPSubsystemUpdatePresenceComplete OnDone);

private:
    void HandleUpdatePresenceComplete(
        const class FUniqueNetId &UserId,
        const bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemUpdatePresenceComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryPresence(const UObject *WorldContextObject, FExampleCPPSubsystemQueryPresenceComplete OnDone);

private:
    void HandleQueryPresenceComplete(const class FUniqueNetId &UserId, const bool bWasSuccessful);

    /********** ExampleCPPSubsystem.Session.cpp **********/

private:
    void OnFindSessionForLeaderFollow(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const FOnlineSessionSearchResult &SearchResult,
        FString SessionFollowString);
    FName JoinSessionForLeaderSessionName;
    FDelegateHandle JoinSessionForLeaderFollowDelegateHandle;
    void OnJoinSessionForLeaderFollow(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

    void OnSessionUserInviteAccepted(
        const bool bWasSuccessful,
        const int32 LocalUserNum,
        TSharedPtr<const FUniqueNetId> UserId,
        const FOnlineSessionSearchResult &InviteResult);
    void OnSessionJoinedViaOverlay(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);
    FName JoinSessionForInviteSessionName;
    FDelegateHandle JoinSessionForInviteDelegateHandle;
    void OnJoinSessionForInviteFollow(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult);

public:
    UPROPERTY(BlueprintAssignable)
    FExampleCPPSubsystemPartyLeaderWasFollowedToSession PartyLeaderWasFollowedToSession;

    UPROPERTY(BlueprintAssignable)
    FExampleCPPSubsystemSessionJoinedMapTravelRequired SessionJoinedMapTravelRequired;

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetCurrentSessionId(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    TArray<FString> GetPlayersInSession(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void RegisterExistingPlayers(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FUniqueNetIdRepl RegisterPlayer(APlayerController *InPlayerController);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void UnregisterPlayer(APlayerController *InPlayerController, FUniqueNetIdRepl UniqueNetIdRepl);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void SendBeaconPingToSearchResult(
        const UObject *WorldContextObject,
        UExampleCPPSessionSearchResult *SearchResult,
        FExampleCPPSubsystemBeaconPingComplete OnDone);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetResolvedConnectString(const UObject *WorldContextObject);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    FString GetResolvedConnectStringForSession(const UObject *WorldContextObject, FName SessionName);

    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartCreateSession(
        const UObject *WorldContextObject,
        bool bOverridePorts,
        int32 InGamePort,
        int32 InBeaconPort,
        FExampleCPPSubsystemCreateSessionComplete OnDone);

private:
    FDelegateHandle CreateSessionDelegateHandle;
    void HandleCreateSessionComplete(
        FName SessionName,
        bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemCreateSessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartFindSessions(const UObject *WorldContextObject, FExampleCPPSubsystemFindSessionsComplete OnDone);

private:
    FDelegateHandle FindSessionsDelegateHandle;
    void HandleFindSessionsComplete(
        bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemFindSessionsComplete OnDone,
        TSharedRef<FOnlineSessionSearch> Search);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartDestroySession(
        const UObject *WorldContextObject,
        FName SessionName,
        FExampleCPPSubsystemDestroySessionComplete OnDone);

private:
    FDelegateHandle DestroySessionDelegateHandle;
    void HandleDestroySessionComplete(
        FName SessionName,
        bool bWasSuccessful,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemDestroySessionComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void StartJoinSession(
        const UObject *WorldContextObject,
        UExampleCPPSessionSearchResult *SearchResult,
        FExampleCPPSubsystemJoinSessionComplete OnDone);

private:
    FDelegateHandle JoinSessionDelegateHandle;
    void HandleJoinSessionComplete(
        FName SessionName,
        EOnJoinSessionCompleteResult::Type JoinResult,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemJoinSessionComplete OnDone);

    /********** ExampleCPPSubsystem.Stats.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryStats(const UObject *WorldContextObject, FExampleCPPSubsystemQueryStatsComplete OnDone);

private:
    void HandleQueryStats(
        const FOnlineError &ResultState,
        const TArray<TSharedRef<const FOnlineStatsUserStats>> &UsersStatsResult,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryStatsComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void IngestStat(
        const UObject *WorldContextObject,
        FString StatName,
        int32 IngestAmount,
        FExampleCPPSubsystemIngestStatComplete OnDone);

private:
    void HandleIngestStat(
        const FOnlineError &ResultState,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemIngestStatComplete OnDone);

    /********** ExampleCPPSubsystem.TitleFile.cpp **********/

public:
    UFUNCTION(BlueprintCallable, Category = "Title")
    void ReadTitleFile(const FString &FileName, FExampleCPPSubsystemReadTitleFileComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "Title")
    void EnumerateTitleFiles(FExampleCPPSubsystemEnumerateTitleFilesComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "Title")
    FString GetFileContents(const FString &FileName);

    void HandleReadTitleFileComplete(
        bool WasSuccessful,
        const FString &FileName,
        FExampleCPPSubsystemReadTitleFileComplete OnDone);

    void HandleEnumerateTitleFilesCompelte(
        bool WasSuccessful,
        const FString &Error,
        FExampleCPPSubsystemEnumerateTitleFilesComplete OnDone);

private:
    static FString ReadFileDataAsString(UCPPFileData *FileData);

    /********** ExampleCPPSubsystem.UserCloud.cpp **********/

private:
    void WriteUserFile(
        const FString &FileName,
        TArray<uint8> &FileData,
        FExampleCPPSubsystemWriteUserFileComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void WriteUserFileFromString(
        const FString &FileName,
        const FString &FileData,
        FExampleCPPSubsystemWriteUserFileComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void WriteUserFileFromSaveGame(
        const FString &FileName,
        class USaveGame *SaveGame,
        FExampleCPPSubsystemWriteUserFileComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void ReadUserFileAsString(const FString &FileName, FExampleCPPSubsystemReadUserFileAsStringComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void ReadUserFileAsSaveGame(const FString &FileName, FExampleCPPSubsystemReadUserFileAsSaveGameComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "User Cloud")
    void EnumerateUserFiles(FExampleCPPSubsystemEnumerateUserFilesComplete OnDone);

    void HandleWriteUserFileComplete(
        bool WasSuccessful,
        const FUniqueNetId &UserId,
        const FString &FileName,
        FExampleCPPSubsystemWriteUserFileComplete OnDone);

    void HandleWriteUserFileCancelled(bool WasSuccessful, const FUniqueNetId &UserId, const FString &FileName);

    void HandleWriteUserFileProgress(
        int32 BytesWritten,
        const FUniqueNetId &UserId,
        const FString &FileName,
        int32 TotalFileSize);

    void HandleReadUserFileComplete(
        bool WasSuccessful,
        const FUniqueNetId &UserId,
        const FString &FileName,
        FExampleCPPSubsystemReadUserFileAsStringComplete OnDone);

    void HandleReadUserFileComplete(
        bool WasSuccessful,
        const FUniqueNetId &UserId,
        const FString &FileName,
        FExampleCPPSubsystemReadUserFileAsSaveGameComplete OnDone);

    void HandleEnumerateUserFilesComplete(
        bool WasSuccessful,
        const FUniqueNetId &UserId,
        FExampleCPPSubsystemEnumerateUserFilesComplete OnDone);

    UPROPERTY(BlueprintAssignable, Category = "User Cloud")
    FExampleCPPSubsystemWriteUserFileProgress ExampleCPPSubsystemWriteUserFileProgress;

    UPROPERTY(BlueprintAssignable, Category = "User Cloud")
    FExampleCPPSubsystemWriteUserFileCancelled ExampleCPPSubsystemWriteUserFileCancelled;

    /********** ExampleCPPSubsystem.VoiceChat.cpp **********/

public:
    /* Creates a voice chat user. Sets the PrimaryVoiceUser if it's not already set. */
    UFUNCTION(BlueprintCallable, Category = "Voice")
    void CreateVoiceChatUser();

    UFUNCTION(BlueprintCallable, Category = "Voice")
    void LoginToVoice(class AVoiceChatServices *VoiceChatService, FExampleCPPSubsystemOnVoiceChatLoginComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "Voice")
    void LogoutOfVoice(
        class AVoiceChatServices *VoiceChatService,
        FExampleCPPSubsystemOnVoiceChatLogoutComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "Voice")
    void JoinVoiceChannel(
        class AVoiceChatServices *VoiceChatService,
        const FString &ChannelName,
        const FString &ChannelCredentials,
        const EVoiceChatChannelTypeCPP ChannelType,
        const struct FVoiceChatChannel3dPropertiesCPP Channel3dProperties,
        FExampleCPPSubsystemOnVoiceChatChannelJoinedComplete OnDone);

    UFUNCTION(BlueprintCallable, Category = "Voice")
    bool IsConnected();

    UFUNCTION(BlueprintCallable, Category = "Voice")
    bool IsConnecting();

    class IVoiceChatUser *PrimaryVoiceUser;

private:
    void HandleVoiceChatLoginComplete(
        const FString &PlayerName,
        const FVoiceChatResult &Result,
        FExampleCPPSubsystemOnVoiceChatLoginComplete OnDone);

    void HandleVoiceChatLogoutComplete(
        const FString &PlayerName,
        const FVoiceChatResult &Result,
        class AVoiceChatServices *VoiceChatService,
        FExampleCPPSubsystemOnVoiceChatLogoutComplete OnDone);

    void HandleVoiceChatJoinChannelComplete(
        const FString &ChannelName,
        const FVoiceChatResult &Result,
        FExampleCPPSubsystemOnVoiceChatChannelJoinedComplete OnDone);

    /********** ExampleCPPSubsystem.UserInfo.cpp **********/

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryUserInfo(
        const UObject *WorldContextObject,
        FString ProductUserIdInput,
        FString EpicAccountIdInput,
        FExampleCPPSubsystemQueryUserInfoComplete OnDone);

private:
    FDelegateHandle QueryUserInfoDelegateHandle;
    void HandleQueryUserInfoComplete(
        int32 LocalUserNum,
        bool bWasSuccessful,
        const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
        const FString &ErrorStr,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryUserInfoComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryUserIdMapping(
        const UObject *WorldContextObject,
        FString DisplayNameInput,
        FExampleCPPSubsystemQueryUserIdMappingComplete OnDone);

private:
    void HandleQueryUserMappingComplete(
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FString &DisplayNameOrEmail,
        const FUniqueNetId &FoundUserId,
        const FString &Error,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryUserIdMappingComplete OnDone);

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    void QueryExternalIdMappings(
        const UObject *WorldContextObject,
        FString PlatformType,
        FString ExternalIds,
        FExampleCPPSubsystemQueryExternalIdMappingsComplete OnDone);

private:
    void HandleQueryExternalIdMappings(
        bool bWasSuccessful,
        const FUniqueNetId &UserId,
        const FExternalIdQueryOptions &QueryOptions,
        const TArray<FString> &ExternalIds,
        const FString &Error,
        const UObject *WorldContextObject,
        FExampleCPPSubsystemQueryExternalIdMappingsComplete OnDone);
};