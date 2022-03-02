// Copyright Cosugames, LLC 2021

#pragma once

#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "DVReplicationGraph.generated.h"

enum class EClassRepPolicy : uint8 {
	NotRouted,
	RelevantAllConnections,
	Spatialize_Static, // Used for actors for frequent updates
	Spatialize_Dynamic, // Used for actors that update every frame
	Spatialize_Dormancy // Actors that can be either Static or Dynamic determined by their AActor::NetDormancy
};

class UReplicationGraphNode_ActorList;
class UReplicationGraphNode_GridSpatialization2D;
class UReplicationGraphNode_AlwaysRelevant_ForConnection;
class AGameplayDebuggerCategoryReplicator;
/**
 * 
 */
UCLASS(Transient, config = Engine)
class DEVERSE_API UDVReplicationGraph : public UReplicationGraph
{
	GENERATED_BODY()
	
protected:
	UDVReplicationGraph();

	//~ Start UReplicationGraph implementation
	virtual void ResetGameWorldState() override;
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;
	virtual void InitGlobalActorClassSettings() override;
	virtual void InitGlobalGraphNodes() override;
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;
	//~ End UReplicationGraph

	void InitClassReplicationInfo(FClassReplicationInfo& Info, UClass* InClass, bool bSpatialize, float ServerMaxTickRate);

	UPROPERTY()
	TArray<UClass*> SpatializedClasses;

	UPROPERTY()
		TArray<UClass*> NonSpatializedClasses;

	UPROPERTY()
		TArray<UClass*> AlwaysRelevantClasses;

	UPROPERTY()
	UReplicationGraphNode_GridSpatialization2D* GridNode;

	UPROPERTY()
		UReplicationGraphNode_ActorList* AlwaysRelevantNode;

	FORCEINLINE bool IsSpatialized(EClassRepPolicy Mapping) {
		return Mapping >= EClassRepPolicy::Spatialize_Static;
	}

	EClassRepPolicy GetMappingPolicy(UClass* InClass); 

	/** Map a class to mapping policies */
	TClassMap<EClassRepPolicy> ClassRepPolicies;

	// The size of one grid cell in the grid node
	float GridCellSize = 10000.f;
	// Min x for replication
	float SpatialBiasX = -150000.f;
	// Min y for replication
	float SpatialBiasY = -200000.f;
	bool bDisableSpatialRebuilding = true;

	UDVReplicationGraphNode_AlwaysRelevant_ForConnection* GetAlwaysRelevantNode(APlayerController* PC);

#if WITH_GAMEPLAY_DEBUGGER
	void OnGameplayDebuggerOwnerChange(AGameplayDebuggerCategoryReplicator* Debugger, APlayerController* OldOwner);
#endif

	// Gameplay events
	UFUNCTION()
	void OnHumanNewNftSpawn(class ADVBaseCharacter* OwningCharacter, ADVDog* Nft);

	UFUNCTION()
		void OnNftUnSpawn(class ADVBaseCharacter* OwningCharacter, ADVDog* Nft);
public:
	/** Maps the actors that need to be always relevant across streaming levels */
	TMap<FName, FActorRepListRefView> AlwaysRelevantStreamingLevelActors;
};

UCLASS()
class UDVReplicationGraphNode_AlwaysRelevant_ForConnection : public UReplicationGraphNode_AlwaysRelevant_ForConnection
{
	GENERATED_BODY()
public:
	// ~ Begin UReplicationGraphNode_AlwaysRelevant_ForConnect implementation
	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;
	// ~ End
	void OnClientLevelVisibilityAdd(FName LevelName, UWorld* LevelWorld);
	void OnClientLevelVisibilityRemove(FName LevelName);

	void ResetGameWorldState();

#if WITH_GAMEPLAY_DEBUGGER
	AGameplayDebuggerCategoryReplicator* GameplayDebugger = nullptr;
#endif

protected:
	/** Stores levelstreaming actors */
	TArray<FName, TInlineAllocator<64>> AlwaysRelevantStreamingLevels;

private:
	bool bInitializedPlayerState = false;
};

/** This is a specialized node for handling PlayerState replication in a frequency limited fashion. It tracks all player states but only returns a subset of them to the replication driver each frame. */
UCLASS()
class UDVReplicationGraphNode_PlayerStateFrequencyLimiter : public UReplicationGraphNode
{
	GENERATED_BODY()

		UDVReplicationGraphNode_PlayerStateFrequencyLimiter();

	virtual void NotifyAddNetworkActor(const FNewReplicatedActorInfo& Actor) override { }
	virtual bool NotifyRemoveNetworkActor(const FNewReplicatedActorInfo& ActorInfo, bool bWarnIfNotFound = true) override { return false; }

	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;

	virtual void PrepareForReplication() override;

	virtual void LogNode(FReplicationGraphDebugInfo& DebugInfo, const FString& NodeName) const override;

	/** How many actors we want to return to the replication driver per frame. Will not suppress ForceNetUpdate. */
	int32 TargetActorsPerFrame = 2;

private:

	TArray<FActorRepListRefView> ReplicationActorLists;
	FActorRepListRefView ForceNetUpdateReplicationActorList;
};