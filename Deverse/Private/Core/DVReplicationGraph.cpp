// Copyright Cosugames, LLC 2021

#include "Core/DVReplicationGraph.h"
#include "Core/Human/DVBaseCharacter.h"
#include "AI/DVDog.h"
#include "Engine/LevelScriptActor.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebuggerCategoryReplicator.h"
#endif

float CVar_DVRepGraph_CellSize = 10000.f;
static FAutoConsoleVariableRef CVarShooterRepGraphCellSize(TEXT("DVRepGraph.CellSize"), CVar_DVRepGraph_CellSize, TEXT(""), ECVF_Default);

UDVReplicationGraph::UDVReplicationGraph()
{
	
}

void UDVReplicationGraph::ResetGameWorldState()
{
	Super::ResetGameWorldState();
	AlwaysRelevantStreamingLevelActors.Empty();
	for (auto& ConnectionList : { Connections, PendingConnections }) {
		for (UNetReplicationGraphConnection* Connection : ConnectionList) {
			for (UReplicationGraphNode* ConnectionNode : Connection->GetConnectionGraphNodes()) {
				UDVReplicationGraphNode_AlwaysRelevant_ForConnection* Node = Cast<UDVReplicationGraphNode_AlwaysRelevant_ForConnection>(ConnectionNode);
				if (Node) {
					Node->ResetGameWorldState();
				}
			}
		}
	}
}

void UDVReplicationGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager)
{
	UDVReplicationGraphNode_AlwaysRelevant_ForConnection* Node = CreateNewNode<UDVReplicationGraphNode_AlwaysRelevant_ForConnection>();
	ConnectionManager->OnClientVisibleLevelNameAdd.AddUObject(Node, &UDVReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd);
	ConnectionManager->OnClientVisibleLevelNameRemove.AddUObject(Node, &UDVReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove);
	AddConnectionGraphNode(Node, ConnectionManager);
}

void UDVReplicationGraph::InitGlobalActorClassSettings()
{
	Super::InitGlobalActorClassSettings();
	// Assign mapping to classes
	auto SetRule = [&](UClass* InClass, EClassRepPolicy Mapping) {
		ClassRepPolicies.Set(InClass, Mapping);
	};

	SetRule(AReplicationGraphDebugActor::StaticClass(),			EClassRepPolicy::NotRouted);
	SetRule(ALevelScriptActor::StaticClass(),					EClassRepPolicy::NotRouted);
	SetRule(AInfo::StaticClass(),								EClassRepPolicy::RelevantAllConnections);
	SetRule(ADVFrame::StaticClass(),							EClassRepPolicy::Spatialize_Static);

#if WITH_GAMEPLAY_DEBUGGER
	SetRule(AGameplayDebuggerCategoryReplicator::StaticClass(), EClassRepPolicy::NotRouted);
#endif

	TArray<UClass*> ReplicatedClasses;
	for (TObjectIterator<UClass> Itr; Itr; ++Itr) {
		UClass* Class = *Itr;
		AActor* ActorCDO = Cast<AActor>(Class->GetDefaultObject());
		if (!ActorCDO || !ActorCDO->GetIsReplicated()){
			continue;
		}

		FString ClassName = Class->GetName();
		if (ClassName.StartsWith("SKEL_") || ClassName.StartsWith("REINST_")) {
			continue;
		}

		ReplicatedClasses.Add(Class);

		if (ClassRepPolicies.Contains(Class, false)) {
			continue;
		}

		auto ShouldSpatialize = [](const AActor* InActor) {
			return InActor->GetIsReplicated() && (!InActor->bAlwaysRelevant || InActor->bOnlyRelevantToOwner || InActor->bNetUseOwnerRelevancy);
		};

		// Skip mapping if net attributes are the same as parent classes
		UClass* SuperClass = Class->GetSuperClass();
		if (AActor* SuperCDO = Cast<AActor>(SuperClass->GetDefaultObject())) {
			if (SuperCDO->GetIsReplicated() == ActorCDO->GetIsReplicated()
				&& SuperCDO->bAlwaysRelevant == ActorCDO->bAlwaysRelevant
				&& SuperCDO->bOnlyRelevantToOwner == ActorCDO->bOnlyRelevantToOwner
				&& SuperCDO->bNetUseOwnerRelevancy == ActorCDO->bNetUseOwnerRelevancy
			){
				continue;
			}

			if (!ShouldSpatialize(ActorCDO) && ShouldSpatialize(SuperCDO)) {
				NonSpatializedClasses.Add(Class);
			}
		}

		if (ShouldSpatialize(ActorCDO)) {
			SetRule(Class, EClassRepPolicy::Spatialize_Dynamic);
		}
		else if (ActorCDO->bAlwaysRelevant && !ActorCDO->bOnlyRelevantToOwner) {
			SetRule(Class, EClassRepPolicy::RelevantAllConnections);
		}
	}

	// Explicitly set replication info for our classes
	TArray<UClass*> ExplicitSetClasses;

	auto SetClassInfo = [&](UClass* InClass, FClassReplicationInfo& RepInfo) {
		GlobalActorReplicationInfoMap.SetClassInfo(InClass, RepInfo);
		ExplicitSetClasses.Add(InClass);
	};

	FClassReplicationInfo PawnClassInfo;
	PawnClassInfo.SetCullDistanceSquared(15000.f * 15000.f);
	SetClassInfo(APawn::StaticClass(), PawnClassInfo);

	FClassReplicationInfo PlayerStateRepInfo;
	PlayerStateRepInfo.DistancePriorityScale = 0.f;
	PlayerStateRepInfo.ActorChannelFrameTimeout = 0;
	SetClassInfo(APlayerState::StaticClass(), PlayerStateRepInfo);

	for (UClass* ReplicatedClass : ReplicatedClasses) {
		if (ExplicitSetClasses.FindByPredicate([&](const UClass* InClass) {return ReplicatedClass->IsChildOf(InClass); }) != nullptr) {
			continue;
		}

		bool bSpatialized = IsSpatialized(ClassRepPolicies.GetChecked(ReplicatedClass));

		FClassReplicationInfo ClassInfo;
		InitClassReplicationInfo(ClassInfo, ReplicatedClass, bSpatialized, NetDriver->NetServerMaxTickRate);
		GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassInfo);
	}

	// Bind events here
	ADVBaseCharacter::OnNewNftSpawn.AddUObject(this, &UDVReplicationGraph::OnHumanNewNftSpawn);
	ADVBaseCharacter::OnNftUnSpawn.AddUObject(this, &UDVReplicationGraph::OnNftUnSpawn);

	// TODO also figure out the replication flow for frames
#if WITH_GAMEPLAY_DEBUGGER
	AGameplayDebuggerCategoryReplicator::NotifyDebuggerOwnerChange.AddUObject(this, &UDVReplicationGraph::OnGameplayDebuggerOwnerChange);
#endif
}

void UDVReplicationGraph::InitGlobalGraphNodes()
{
	//PreAllocateRepList(3, 12);
	//PreAllocateRepList(6, 12);
	//PreAllocateRepList(128, 64);
	//PreAllocateRepList(512, 16);
	// Create our grid node
	GridNode = CreateNewNode<UReplicationGraphNode_GridSpatialization2D>();
	GridNode->CellSize = CVar_DVRepGraph_CellSize;
	GridNode->SpatialBias = FVector2D(SpatialBiasX, SpatialBiasY);

	if (bDisableSpatialRebuilding) {
		GridNode->AddSpatialRebuildBlacklistClass(AActor::StaticClass()); // Disable All spatial rebuilding
	}

	AddGlobalGraphNode(GridNode);

	// -----------------------------------------------
	//	Always Relevant (to everyone) Actors
	// -----------------------------------------------
	AlwaysRelevantNode = CreateNewNode<UReplicationGraphNode_ActorList>();
	AddGlobalGraphNode(AlwaysRelevantNode);

	// -----------------------------------------------
	//	Player State specialization. This will return a rolling subset of the player states to replicate
	// -----------------------------------------------
	UDVReplicationGraphNode_PlayerStateFrequencyLimiter* PlayerStateNode = CreateNewNode<UDVReplicationGraphNode_PlayerStateFrequencyLimiter>();
	AddGlobalGraphNode(PlayerStateNode);
}

void UDVReplicationGraph::RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo)
{
	EClassRepPolicy MappingPolicy = GetMappingPolicy(ActorInfo.Class);
	switch (MappingPolicy) {
	case EClassRepPolicy::RelevantAllConnections:
		if (ActorInfo.StreamingLevelName == NAME_None) {
			AlwaysRelevantNode->NotifyAddNetworkActor(ActorInfo);
		}
		else {
			FActorRepListRefView& RepList = AlwaysRelevantStreamingLevelActors.FindOrAdd(ActorInfo.StreamingLevelName);
			//RepList.PrepareForWrite();
			RepList.ConditionalAdd(ActorInfo.Actor);
		}
		break;
	case EClassRepPolicy::Spatialize_Static:
		GridNode->AddActor_Static(ActorInfo, GlobalInfo);
		break;
	case EClassRepPolicy::Spatialize_Dynamic:
		GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);
		break;
	case EClassRepPolicy::Spatialize_Dormancy:
		GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
		break;
	}
}

void UDVReplicationGraph::RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo)
{
	EClassRepPolicy MappingPolicy = GetMappingPolicy(ActorInfo.Class);
	switch (MappingPolicy) {
	case EClassRepPolicy::RelevantAllConnections:
		if (ActorInfo.StreamingLevelName == NAME_None) {
			AlwaysRelevantNode->NotifyRemoveNetworkActor(ActorInfo);
		}
		else {
			FActorRepListRefView& RepList = AlwaysRelevantStreamingLevelActors.FindOrAdd(ActorInfo.StreamingLevelName);
			RepList.RemoveFast(ActorInfo.Actor);
		}
		break;
	case EClassRepPolicy::Spatialize_Static:
		GridNode->RemoveActor_Static(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Dynamic:
		GridNode->RemoveActor_Dynamic(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Dormancy:
		GridNode->RemoveActor_Dormancy(ActorInfo);
		break;
	}
}

void UDVReplicationGraph::InitClassReplicationInfo(FClassReplicationInfo& Info, UClass* InClass, bool bSpatialize, float ServerMaxTickRate)
{
	if (AActor* ActorCDO = Cast<AActor>(InClass->GetDefaultObject())) {
		if (bSpatialize) {
			Info.SetCullDistanceSquared(ActorCDO->NetCullDistanceSquared);
			UE_LOG(LogTemp, Log, TEXT("Setting cull distance for %s to %f (%f)"), *InClass->GetName(), Info.GetCullDistanceSquared(), Info.GetCullDistance());
		}

		Info.ReplicationPeriodFrame = FMath::Max<uint32>(FMath::RoundToFloat(ServerMaxTickRate / ActorCDO->NetUpdateFrequency), 1);
	}
}

EClassRepPolicy UDVReplicationGraph::GetMappingPolicy(UClass* InClass)
{
	return ClassRepPolicies.Get(InClass) != NULL ? *ClassRepPolicies.Get(InClass) : EClassRepPolicy::NotRouted;
}

UDVReplicationGraphNode_AlwaysRelevant_ForConnection* UDVReplicationGraph::GetAlwaysRelevantNode(APlayerController* PC)
{
	if (PC) {
		if (UNetConnection* NetConnection = PC->NetConnection) {
			if (UNetReplicationGraphConnection* GraphConnection = FindOrAddConnectionManager(NetConnection)) {
				for (UReplicationGraphNode* ConnectionNode : GraphConnection->GetConnectionGraphNodes()) {
					UDVReplicationGraphNode_AlwaysRelevant_ForConnection* Node = Cast<UDVReplicationGraphNode_AlwaysRelevant_ForConnection>(ConnectionNode);
					if (Node) {
						return Node;
					}
				}
			}
		}
	}
	return nullptr;
}

#if WITH_GAMEPLAY_DEBUGGER
void UDVReplicationGraph::OnGameplayDebuggerOwnerChange(AGameplayDebuggerCategoryReplicator* Debugger, APlayerController* OldOwner)
{
	if (UDVReplicationGraphNode_AlwaysRelevant_ForConnection* Node = GetAlwaysRelevantNode(OldOwner)) {
		Node->GameplayDebugger = nullptr;
	}

	if (UDVReplicationGraphNode_AlwaysRelevant_ForConnection* Node = GetAlwaysRelevantNode(Debugger->GetReplicationOwner())) {
		Node->GameplayDebugger = Debugger;
	}
}
#endif

void UDVReplicationGraph::OnHumanNewNftSpawn(class ADVBaseCharacter* OwningCharacter, ADVDog* Nft)
{
	if (!OwningCharacter || OwningCharacter->GetWorld() != GetWorld()) {
		return;
	}

	if (Nft != nullptr) {
		GlobalActorReplicationInfoMap.AddDependentActor(OwningCharacter, Nft);
	}
}

void UDVReplicationGraph::OnNftUnSpawn(class ADVBaseCharacter* OwningCharacter, ADVDog* Nft)
{
	if (!OwningCharacter || OwningCharacter->GetWorld() != GetWorld()) {
		return;
	}

	if (Nft != nullptr) {
		GlobalActorReplicationInfoMap.RemoveDependentActor(OwningCharacter, Nft);
	}
}

// UDVReplicationGraphNode_AlwaysRelevant_ForConnection
void UDVReplicationGraphNode_AlwaysRelevant_ForConnection::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	UDVReplicationGraph* RepGraph = CastChecked<UDVReplicationGraph>(GetOuter());

	ReplicationActorList.Reset();

	auto ResetActorCullDistance = [&](AActor* ActorToSet, AActor*& LastActor) {
		if (ActorToSet != LastActor) {
			LastActor = ActorToSet;
			//UE_LOG(LogTemp, Warning, TEXT("Setting pawn cull distance to 0. %s"), *ActorToSet->GetName());
			FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(ActorToSet);
			ConnectionActorInfo.SetCullDistanceSquared(0.f);
		}
	};

	for (const FNetViewer& CurViewer : Params.Viewers) {
		ReplicationActorList.ConditionalAdd(CurViewer.InViewer);
		ReplicationActorList.ConditionalAdd(CurViewer.ViewTarget);
		if (ADVBaseController* PC = Cast<ADVBaseController>(CurViewer.InViewer)) {
			// 50% throttling of PlayerStates.
			const bool bReplicatePS = (Params.ConnectionManager.ConnectionOrderNum % 2) == (Params.ReplicationFrameNum % 2);
			if (bReplicatePS) {
				// Always return the player state to the owning player. Simulated proxy player states are handled by UDVReplicationGraphNode_PlayerStateFrequencyLimiter
				if (APlayerState* PS = PC->PlayerState) {
					if (!bInitializedPlayerState) {
						bInitializedPlayerState = true;
						FConnectionReplicationActorInfo& ConnectionActorInfo = Params.ConnectionManager.ActorInfoMap.FindOrAdd(PS);
						ConnectionActorInfo.ReplicationPeriodFrame = 1;
					}
					ReplicationActorList.ConditionalAdd(PS);
				}
			}
			FAlwaysRelevantActorInfo* LastData = PastRelevantActors.FindByKey<UNetConnection*>(CurViewer.Connection);

			// We've not seen this actor before, go ahead and add them.
			if (LastData == nullptr) {
				FAlwaysRelevantActorInfo NewActorInfo;
				NewActorInfo.Connection = CurViewer.Connection;
				LastData = &(PastRelevantActors[PastRelevantActors.Add(NewActorInfo)]);
			}

			check(LastData != nullptr);
			if (ADVBaseCharacter* Pawn = Cast<ADVBaseCharacter>(PC->GetPawn())) {
				ResetActorCullDistance(Pawn, static_cast<AActor*&>(LastData->LastViewer));
				if (Pawn != CurViewer.ViewTarget) {
					ReplicationActorList.ConditionalAdd(Pawn);
				}

				TArray<AActor*> AttachedActors;
				Pawn->GetAttachedActors(AttachedActors);
				for (AActor* AttachActor : AttachedActors) {
					ADVFrame* AttachedFrame = Cast<ADVFrame>(AttachActor);
					if (AttachedFrame) {
						ReplicationActorList.ConditionalAdd(AttachedFrame);
					}
				}

			}

			if (ADVBaseCharacter* ViewTargetPawn = Cast<ADVBaseCharacter>(CurViewer.ViewTarget)) {
				ResetActorCullDistance(ViewTargetPawn, static_cast<AActor*&>(LastData->LastViewTarget));
			}
		}
	}

	PastRelevantActors.RemoveAll([&](FAlwaysRelevantActorInfo& RelActorInfo) { // Remove all actors that no longer have connections
		return RelActorInfo.Connection == nullptr;
		});

	Params.OutGatheredReplicationLists.AddReplicationActorList(ReplicationActorList);

	FPerConnectionActorInfoMap& ConnectionActorInfoMap = Params.ConnectionManager.ActorInfoMap;
	TMap<FName, FActorRepListRefView>& AlwaysRelevantStreamingLevelActors = RepGraph->AlwaysRelevantStreamingLevelActors;

	for (int32 i = AlwaysRelevantStreamingLevelActors.Num() - 1; i >= 0; i--) {
		FName StreamingLevelName = AlwaysRelevantStreamingLevels[i];
		FActorRepListRefView* ListPtr = AlwaysRelevantStreamingLevelActors.Find(StreamingLevelName);

		if (!ListPtr) {
			AlwaysRelevantStreamingLevels.RemoveAtSwap(i, 1, false);
			continue;
		}

		FActorRepListRefView& RepList = *ListPtr;
		if (RepList.Num() > 0) {
			bool bAllDormant = true;
			for (FActorRepListType Actor : RepList) {
				FConnectionReplicationActorInfo& ConnectionActorInfo = ConnectionActorInfoMap.FindOrAdd(Actor);
				// If one actor is flagged dormant, we mark the whole rep list non-dormant
				if (!ConnectionActorInfo.bDormantOnConnection) {
					bAllDormant = false;
					break;
				}
			}

			if (bAllDormant) {
				AlwaysRelevantStreamingLevels.RemoveAtSwap(i, 1, false);
			}
			else {
				Params.OutGatheredReplicationLists.AddReplicationActorList(RepList);
			}
		}
	}

#if WITH_GAMEPLAY_DEBUGGER
	if (GameplayDebugger) {
		ReplicationActorList.ConditionalAdd(GameplayDebugger);
	}
#endif
}

void UDVReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd(FName LevelName, UWorld* LevelWorld)
{
	AlwaysRelevantStreamingLevels.Add(LevelName);
}

void UDVReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove(FName LevelName)
{
	AlwaysRelevantStreamingLevels.Remove(LevelName);
}

void UDVReplicationGraphNode_AlwaysRelevant_ForConnection::ResetGameWorldState()
{
	AlwaysRelevantStreamingLevels.Empty();
}

UDVReplicationGraphNode_PlayerStateFrequencyLimiter::UDVReplicationGraphNode_PlayerStateFrequencyLimiter()
{
	bRequiresPrepareForReplicationCall = true;
}

void UDVReplicationGraphNode_PlayerStateFrequencyLimiter::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	const int32 ListIdx = Params.ReplicationFrameNum % ReplicationActorLists.Num();
	Params.OutGatheredReplicationLists.AddReplicationActorList(ReplicationActorLists[ListIdx]);

	if (ForceNetUpdateReplicationActorList.Num() > 0)
	{
		Params.OutGatheredReplicationLists.AddReplicationActorList(ForceNetUpdateReplicationActorList);
	}
}

void UDVReplicationGraphNode_PlayerStateFrequencyLimiter::PrepareForReplication()
{
	QUICK_SCOPE_CYCLE_COUNTER(UDVReplicationGraphNode_PlayerStateFrequencyLimiter_GlobalPrepareForReplication);

	ReplicationActorLists.Reset();
	ForceNetUpdateReplicationActorList.Reset();

	ReplicationActorLists.AddDefaulted();
	FActorRepListRefView* CurrentList = &ReplicationActorLists[0];

	// We rebuild our lists of player states each frame. This is not as efficient as it could be but its the simplest way
	// to handle players disconnecting and keeping the lists compact. If the lists were persistent we would need to defrag them as players left.

	for (TActorIterator<APlayerState> It(GetWorld()); It; ++It)
	{
		APlayerState* PS = *It;
		if (IsActorValidForReplicationGather(PS) == false)
		{
			continue;
		}

		if (CurrentList->Num() >= TargetActorsPerFrame)
		{
			ReplicationActorLists.AddDefaulted();
			CurrentList = &ReplicationActorLists.Last();
		}

		CurrentList->Add(PS);
	}
}

void UDVReplicationGraphNode_PlayerStateFrequencyLimiter::LogNode(FReplicationGraphDebugInfo& DebugInfo, const FString& NodeName) const
{
	DebugInfo.Log(NodeName);
	DebugInfo.PushIndent();

	int32 i = 0;
	for (const FActorRepListRefView& List : ReplicationActorLists)
	{
		LogActorRepList(DebugInfo, FString::Printf(TEXT("Bucket[%d]"), i++), List);
	}

	DebugInfo.PopIndent();
}
