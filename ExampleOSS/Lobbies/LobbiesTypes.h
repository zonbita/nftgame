// Copyright June Rhodes. MIT Licensed.

#pragma once

#include "OnlineLobbyInterface.h"

#include "LobbiesTypes.generated.h"

USTRUCT(BlueprintType)
struct FExampleCPPCreateOrConnectLobbyResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    class UExampleCPPLobby *Lobby;
};

USTRUCT(BlueprintType)
struct FExampleCPPLobbySearchResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    TArray<class UExampleCPPLobbyId *> Lobbies;
};

USTRUCT(BlueprintType)
struct FExampleCPPUpdateLobbyResult
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorRaw;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FString ErrorCode;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FText ErrorMessage;

    UPROPERTY(BlueprintReadOnly, Category = "Lobbies")
    FUniqueNetIdRepl UserId;
};

UCLASS(BlueprintType)
class EXAMPLEOSS_API UExampleCPPLobbyId : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<const FOnlineLobbyId> LobbyId;

public:
    static UExampleCPPLobbyId *FromNative(const FOnlineLobbyId &InObj);
    static UExampleCPPLobbyId *FromNative(const TSharedPtr<const FOnlineLobbyId> &InObj);
    const FOnlineLobbyId &ToNative() const
    {
        return this->LobbyId.ToSharedRef().Get();
    }
    bool IsValid() const
    {
        return this->LobbyId.IsValid();
    }

    UFUNCTION(BlueprintPure, Category = "Online")
    FString ToDebugString()
    {
        if (!this->IsValid())
        {
            return TEXT("");
        }

        return this->LobbyId->ToDebugString();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class EXAMPLEOSS_API UExampleCPPLobby : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobby> Lobby;

public:
    UFUNCTION(BlueprintPure, Category = "Online")
    UExampleCPPLobbyId *GetId();

    UFUNCTION(BlueprintPure, Category = "Online")
    FUniqueNetIdRepl GetOwnerId();

    static UExampleCPPLobby *FromNative(TSharedPtr<FOnlineLobby> InObj);
    TSharedPtr<FOnlineLobby> ToNative() const
    {
        return this->Lobby;
    }
    bool IsValid() const
    {
        return this->Lobby.IsValid();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class EXAMPLEOSS_API UExampleCPPOnlineLobbyTransaction : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobbyTransaction> Txn;

public:
    void SetMetadata(const FString &Key, const FVariantData &Value);

    void SetMetadataByMap(const TMap<FString, FVariantData> &Metadata);

    void DeleteMetadata(const FString &Key);

    void DeleteMetadataByArray(const TArray<FString> &MetadataKeys);

    void SetLocked(bool Locked);

    void SetCapacity(int64 Capacity);

    void SetPublic(bool Public);

    static UExampleCPPOnlineLobbyTransaction *FromNative(TSharedPtr<FOnlineLobbyTransaction> InObj);
    TSharedPtr<FOnlineLobbyTransaction> ToNative() const
    {
        return this->Txn;
    }
    bool IsValid() const
    {
        return this->Txn.IsValid();
    }
};

UCLASS(BlueprintType, Transient, Meta = (DontUseGenericSpawnObject, IsBlueprintBase = "false"))
class EXAMPLEOSS_API UExampleCPPOnlineLobbyMemberTransaction : public UObject
{
    GENERATED_BODY()

private:
    TSharedPtr<FOnlineLobbyMemberTransaction> Txn;

public:
    void SetMetadata(const FString &Key, const FVariantData &Value);

    void SetMetadataByMap(const TMap<FString, FVariantData> &Metadata);

    void DeleteMetadata(const FString &Key);

    void DeleteMetadataByArray(const TArray<FString> &MetadataKeys);

    static UExampleCPPOnlineLobbyMemberTransaction *FromNative(TSharedPtr<FOnlineLobbyMemberTransaction> InObj);
    TSharedPtr<FOnlineLobbyMemberTransaction> ToNative() const
    {
        return this->Txn;
    }
    bool IsValid() const
    {
        return this->Txn.IsValid();
    }
};