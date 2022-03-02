// Copyright Cosugames, LLC 2021

#include "Core/DVBaseGameMode.h"
#include "Core/DVBaseController.h"
#include "Math/TransformCalculus3D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DVWorldSettings.h"
#include "Core/DVBaseGameState.h"
#include "Core/Human/DVBaseCharacter.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/CheatManager.h"

UClass* ADVBaseGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	TSubclassOf<AActor> SpawnClass;
	IDVEventInterface::Execute_GetSpawnClass(InController->PlayerState, SpawnClass);
	if (SpawnClass)
		return SpawnClass;
#if WITH_EDITOR
	return DefaultPawnClass;
#endif
	return nullptr;
}

void ADVBaseGameMode::RestartPlayerAtTransform(AController* NewPlayer, const FTransform& SpawnTransform)
{
	if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}

	//UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtTransform %s"), (NewPlayer && NewPlayer->PlayerState) ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));

	if (MustSpectate(Cast<APlayerController>(NewPlayer)))
	{
		UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtTransform: Tried to restart a spectator-only player!"));
		return;
	}

	FRotator SpawnRotation = SpawnTransform.GetRotation().Rotator();

	if (NewPlayer->GetPawn() != nullptr)
	{
		// If we have an existing pawn, just use it's rotation
		SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
	}
	else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
	{
		// Try to create a pawn to use of the default class for this player
		NewPlayer->SetPawn(SpawnDefaultPawnAtTransform(NewPlayer, SpawnTransform));
	}

	if (NewPlayer->GetPawn() == nullptr)
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		FinishRestartPlayer(NewPlayer, SpawnRotation);
	}
}

void ADVBaseGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	NewPlayer->Possess(NewPlayer->GetPawn());

	// If the Pawn is destroyed as part of possession we have to abort
	if (NewPlayer->GetPawn() == nullptr)
	{
		NewPlayer->FailedToSpawnPawn();
	}
	else
	{
		// Set initial control rotation to starting rotation rotation
		NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);
		FRotator NewControllerRot = StartRotation;
		NewControllerRot.Roll = 0.f;
		NewPlayer->SetControlRotation(NewControllerRot);

		SetPlayerDefaults(NewPlayer->GetPawn());

		K2_OnRestartPlayer(NewPlayer);
	}
}

void ADVBaseGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void ADVBaseGameMode::EventRaceUpdated_Implementation(AController* InController)
{
	RestartPlayer(InController);
}

void ADVBaseGameMode::EventPlayerFirstJoin_Implementation(AController* NewController)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADVSpaceShip::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0) {
		for (AActor* FoundActor : FoundActors) {
			if (FoundActor && !FoundActor->IsPendingKill() && FoundActor->GetOwner() == NewController) {
				FTransform SpawnTransform;
				IDVEventInterface::Execute_GetPortalSpawnTransform(FoundActor, SpawnTransform);
				RequestRespawn(NewController, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator());
			}
		}
	}
	else {
		RestartPlayer(NewController);
	}
}

void ADVBaseGameMode::IssuedCredentialsForPlayer(const FOnlineError& Result, const FUniqueNetId& LocalUserId,const TArray<FVoiceAdminChannelCredentials>& Credentials, FString InChannelName, ADVBaseController* NewPlayer)
{
	if (!Result.WasSuccessful())
	{
		return;
	}
	
	if (IsValid(NewPlayer))
	{
		NewPlayer->JoinVoiceChannel(InChannelName, Credentials[0].ChannelCredentials);
	}
}

void ADVBaseGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	Calculator();
}

void ADVBaseGameMode::Logout(AController* Exiting)
{
	if (!IsValid(Exiting))
		{return;}
	
	APlayerController* PC = Cast<APlayerController>(Exiting);
	if (!IsValid(PC))
		{return;}
	
	if (!PC->IsLocalPlayerController() || !IsValid(PC->Player))
		{return;}
	
	UNetConnection* PCNet = Cast<UNetConnection>(PC->Player);
	if (!IsValid(PCNet) || !PCNet->PlayerId.IsValid())
		{return;}
	
	ADVWorldSettings* WorldSettings = Cast<ADVWorldSettings>(GetWorld()->GetWorldSettings());
	if (WorldSettings && WorldSettings->bSpawnSpaceship) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), SpaceShipClass, FoundActors);
		for (AActor* FoundActor : FoundActors)
		{
			if (FoundActor->GetOwner() == Exiting)
			{
				ADVBaseGameState* GS = Cast<ADVBaseGameState>(GameState);
				if (GS) {
					GS->RemoveSpaceshipId(FString::Printf(TEXT("portal_%s_ship"), *Exiting->GetName()), FoundActor);
				}			
				FoundActor->Destroy();
				break;
			}
		}
	}

	// TSharedRef<const FUniqueNetId> UserId = PCNet->PlayerId.GetUniqueNetId().ToSharedRef();
	// IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	// TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity = Subsystem->GetIdentityInterface();
	// TSharedPtr<IOnlineVoiceAdmin, ESPMode::ThreadSafe> VoiceAdmin = Online::GetVoiceAdminInterface(Subsystem);
	// FString ChannelName = TEXT("TODO");
	// VoiceAdmin->KickParticipant(
	// 	*Identity->GetUniquePlayerId(0),
	// 	// This will get the "dedicated server" ID.
	// 	ChannelName,
	// 	*UserId,
	// 	FOnVoiceAdminKickParticipantComplete::CreateLambda([](
	// 		const FOnlineError& Result,
	// 		const FUniqueNetId& LocalUserId,
	// 		const FUniqueNetId& TargetUserId)
	// 		{
	// 		if (!Result.WasSuccessful())
	// 			{               }
	// 		}));
	Super::Logout(Exiting);
}

void ADVBaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (NewPlayer->IsLocalPlayerController())
	{
		return;
	}
	ADVBaseController* CustomPC = Cast<ADVBaseController>(NewPlayer);
	if (!IsValid(CustomPC))
	{
		return;
	}
	UNetConnection* IncomingNetConnection = Cast<UNetConnection>(CustomPC->Player);
	if (!IsValid(IncomingNetConnection) || !IncomingNetConnection->PlayerId.IsValid())
	{       
		// Not a networked player controller, or there's no player ID.       
		return;
	}
	// IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	// TSharedPtr<IOnlineIdentity, ESPMode::ThreadSafe> Identity = Subsystem->GetIdentityInterface();
	// TSharedPtr<IOnlineVoiceAdmin, ESPMode::ThreadSafe> VoiceAdmin = Online::GetVoiceAdminInterface(Subsystem);
	// FString ChannelName = TEXT("TODO");
	// TArray<TSharedRef<const FUniqueNetId>> TargetUserIds;
	// TargetUserIds.Add(IncomingNetConnection->PlayerId.GetUniqueNetId().ToSharedRef());
	// VoiceAdmin->CreateChannelCredentials(*Identity->GetUniquePlayerId(0),ChannelName, TargetUserIds,
	// 	FOnVoiceAdminCreateChannelCredentialsComplete::CreateUObject( this, &ADVBaseGameMode::IssuedCredentialsForPlayer, ChannelName, CustomPC));
}

void ADVBaseGameMode::Calculator()
{
	int rs = SpaceArea/SpaceRange;
	for(int h = 0; h < rs; h++){
		for(int i = 0; i < rs; i++){
			FVector RandomPoint(
			(i * SpaceRange) - (SpaceArea/2),
			FMath::RandRange(-(SpaceArea/2), (SpaceArea/2)),
			(h * SpaceRange) - (SpaceArea/2));
			SSLoc.Add(RandomPoint);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%d"), SSLoc.Num());
#if WITH_EDITOR
	//Debug();
#endif
}

bool ADVBaseGameMode::RequestRespawn(AController* InController, const FVector SpawnLocation, const FRotator SpawnRotation)
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());
	RestartPlayerAtTransform(InController, SpawnTransform);
	return true;
}

#if WITH_EDITOR
void ADVBaseGameMode::Debug()
{
	for(int x = 0; x< SSLoc.Num()-1; x++)
	{
		if (x == MaxDebugShip) return;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetWorld()->GetFirstPlayerController();
		FRotator Rotate( 0, FMath::RandRange(0.f, 360.f), 0);
		GetWorld()->SpawnActor<AActor>(SpaceShipClass, SSLoc[x], Rotate, SpawnParams);// spawn debug
	}
}
#endif

void ADVBaseGameMode::SpawnShip(APlayerController* controller)
{
	ADVWorldSettings* WorldSettings = Cast<ADVWorldSettings>(GetWorld()->GetWorldSettings());
	if (WorldSettings && WorldSettings->bSpawnSpaceship) {
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = controller;
		FRotator R(0, FMath::RandRange(0.f, 360.f), 0);
		for (int x = 0; x < SSLoc.Num() - 1; x++)
		{
			if (!SSInt.Contains(x))
			{
				GetWorld()->SpawnActor<AActor>(SpaceShipClass, SSLoc[x], R, SpawnParams);	
				SSInt.Add(x);
				Number = x;
				break;
			}
		}
	}
}



