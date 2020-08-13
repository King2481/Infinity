// Copyright Epic Games, Inc. All Rights Reserved.


#include "InfinityGameModeBase.h"
#include "GameAnnouncementComponent.h"
#include "Infinity/Gamemodes/InfinityGameModeBase.h"
#include "Infinity/Characters/InfinityCharacter.h"
#include "Infinity/Gamemodes/InfinityGameState.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Infinity/Player/InfinityPlayerState.h"
#include "Infinity/HUD/InfinityHUD.h"
#include "UObject/ConstructorHelpers.h" 
#include "Kismet/GameplayStatics.h"
#include "Infinity/Factions/TeamInterface.h"
#include "Infinity/Factions/TeamInfo.h"

namespace MatchState
{
	const FName RoundWon = FName(TEXT("RoundWon"));
	const FName GameOver = FName(TEXT("GameOver"));
}

AInfinityGameModeBase::AInfinityGameModeBase()
{
	GameAnnouncement = CreateDefaultSubobject<UGameAnnouncementComponent>(TEXT("Game Announcements"));

	static ConstructorHelpers::FClassFinder<AInfinityCharacter>BPPawnClass(TEXT("/Game/Game/Actors/Characters/BP_Character_Player"));
	DefaultPawnClass = BPPawnClass.Class;

	GameStateClass = AInfinityGameState::StaticClass();
	PlayerControllerClass = AInfinityPlayerController::StaticClass();
	PlayerStateClass = AInfinityPlayerState::StaticClass();
	HUDClass = AInfinityHUD::StaticClass();

	// Generic
	ScoreNeededToWin = -1;
	UniversalDamageMultiplayer = 1.f;
	SelfDamageMultiplier = 0.25f;
	bValidateClientSideHits = true;
	bAllowFriendlyFire = false;
	FriendlyFireDamageMultiplier = 0.25f;
	RoundTimeLimit = 480; // 8 Minutes for all rounds by default.
	WinningPlayerState = nullptr;
	WinningTeamId = ITeamInterface::InvalidId;
}

void AInfinityGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	RoundTimeLimit = UGameplayStatics::GetIntOption(Options, TEXT("RoundTimeLimit"), RoundTimeLimit);

	// Password
	ServerPassword = UGameplayStatics::ParseOption(Options, TEXT("Password="));
	if (!ServerPassword.IsEmpty())
	{
		UE_LOG(LogGameMode, Display, TEXT("Server is now password protected!"));
	}
}

void AInfinityGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InitializeTeams();
}

void AInfinityGameModeBase::InitializeTeams()
{
	if (TeamsForMode.Num() <= 0)
	{
		// No teams to initialize
		return;
	}

	const auto GS = GetGameState<AInfinityGameState>();
	if (!GS)
	{
		// No GameState available.
		return;
	}

	for (int i = 0; i <= TeamsForMode.Num() - 1; i++)
	{
		const auto Team = TeamsForMode[i].LoadSynchronous();
		if (Team)
		{
			const auto NewTeam = GetWorld()->SpawnActor<ATeamInfo>(ATeamInfo::StaticClass(), FTransform());
			if (NewTeam)
			{
				NewTeam->InitializeTeam(Team, i);
				GS->AddTeam(NewTeam);
			}
		}
	}
}

void AInfinityGameModeBase::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	// Password checking
	if (IsPasswordProtected())
	{
		const auto ProvidedPassword = UGameplayStatics::ParseOption(Options, TEXT("Password"));
		if (ProvidedPassword.IsEmpty())
		{
			ErrorMessage = TEXT("No password provided.");
		}
		else if (ProvidedPassword.Compare(ServerPassword, ESearchCase::CaseSensitive) != 0)
		{
			ErrorMessage = TEXT("Password mismatch.");
		}

		if (!ErrorMessage.IsEmpty())
		{
			UE_LOG(LogGameMode, Warning, TEXT("Rejecting incoming connection, password failure: %s"), *ErrorMessage);
			return;
		}
	}

	// Everything is fine, go ahead with login
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void AInfinityGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	const auto PC = Cast<AInfinityPlayerController>(NewPlayer);
	const auto PS = Cast<AInfinityPlayerState>(NewPlayer->PlayerState);
	if (PC && PS)
	{
		if (TeamsForMode.Num() >= 2)
		{
			PC->JoinTeam(ChooseTeam(PS));
		}
		else
		{
			PC->JoinTeam(ITeamInterface::InvalidId);
		}
	}
	
	Super::PostLogin(NewPlayer);
}

uint8 AInfinityGameModeBase::ChooseTeam(AInfinityPlayerState* ForPlayerState) const
{
	TArray<int32> TeamBalance;
	TeamBalance.AddZeroed(TeamsForMode.Num());

	// get current team balance
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		AInfinityPlayerState const* const TestPlayerState = Cast<AInfinityPlayerState>(GameState->PlayerArray[i]);
		if (TestPlayerState && TestPlayerState != ForPlayerState && TeamBalance.IsValidIndex(TestPlayerState->GetTeamId()))
		{
			TeamBalance[TestPlayerState->GetTeamId()]++;
		}
	}

	// find least populated one
	int32 BestTeamScore = TeamBalance[0];
	for (int32 i = 1; i < TeamBalance.Num(); i++)
	{
		if (BestTeamScore > TeamBalance[i])
		{
			BestTeamScore = TeamBalance[i];
		}
	}

	// there could be more than one...
	TArray<int32> BestTeams;
	for (int32 i = 0; i < TeamBalance.Num(); i++)
	{
		if (TeamBalance[i] == BestTeamScore)
		{
			BestTeams.Add(i);
		}
	}

	// get random from best list
	const int32 RandomBestTeam = BestTeams[FMath::RandHelper(BestTeams.Num())];
	return RandomBestTeam;
}

void AInfinityGameModeBase::OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!Victim)
	{
		UE_LOG(LogGameMode, Warning, TEXT("A character was killed, but the victim was nullptr."));
		return;
	}

	const auto VictimPS = Cast<AInfinityPlayerState>(Victim->GetPlayerState());
	const auto KillerPS = EventInstigator ? EventInstigator->GetPlayerState<AInfinityPlayerState>() : nullptr;
	const auto GS = Cast<AInfinityGameState>(GameState);
	const bool bSelfKill = VictimPS == KillerPS;

	if (VictimPS && KillerPS)
	{
		UE_LOG(LogGameMode, VeryVerbose, TEXT("%s Was killed by %s"), *VictimPS->GetPlayerName(), *KillerPS->GetPlayerName());
	}

	if (KillerPS)
	{
		KillerPS->ScoreKill();
	}

	if (VictimPS)
	{
		VictimPS->ScoreDeath();
	}

	if (GameAnnouncement)
	{
		GameAnnouncement->OnCharacterKilled(Victim, VictimPS, KillerPS, EventInstigator);
	}
}

float AInfinityGameModeBase::OnCharacterTakeDamage(AInfinityCharacter* Reciever, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float AlteredDamage = Damage * UniversalDamageMultiplayer;

	const auto DamagedController = Reciever ? Reciever->GetController() : nullptr;
	const auto DamagerController = EventInstigator;
	const bool bSelfDamage = DamagedController == DamagerController;
	const auto DamagingCharacter = Cast<AInfinityCharacter>(EventInstigator->GetPawn());

	if (bSelfDamage)
	{
		AlteredDamage *= SelfDamageMultiplier;
	}

	// Friendly fire specfics check.
	if (ITeamInterface::IsAlly(Reciever, DamagingCharacter) && bAllowFriendlyFire && !bSelfDamage)
	{
		AlteredDamage *= FriendlyFireDamageMultiplier;
	}

	return AlteredDamage;
}

void AInfinityGameModeBase::SetMatchState(FName NewState)
{
	Super::SetMatchState(NewState);

	UE_LOG(LogGameMode, Display, TEXT("State: %s -> %s"), *MatchState.ToString(), *NewState.ToString());

	if (GetGameState<AGameState>())
	{
		GetGameState<AGameState>()->SetMatchState(NewState);
	}

	CallMatchStateChangeNotify();
}

void AInfinityGameModeBase::CallMatchStateChangeNotify()
{
	if (MatchState == MatchState::RoundWon)
	{
		HandleRoundWon();
	}
	else if (MatchState == MatchState::GameOver)
	{
		HandleGameOver();
	}
	else
	{
		HandleMatchHasStarted();
	}
}

void AInfinityGameModeBase::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	const auto GS = GetGameState<AInfinityGameState>();
	if (GS)
	{
		GS->SetRoundTimer(RoundTimeLimit);
	}
}

void AInfinityGameModeBase::HandleRoundWon()
{
	// Inform all connected players that we have won the round.
	for (FConstControllerIterator Iterator = GetWorld()->GetControllerIterator(); Iterator; ++Iterator)
	{
		auto Controller = Cast<AInfinityPlayerController>(Iterator->Get());
		if (!Controller)
		{
			continue;
		}

		Controller->OnRoundWon(WinningPlayerState, WinningTeamId);
	}

	GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &AInfinityGameModeBase::OnGameOverStart, 10.f);
}

void AInfinityGameModeBase::OnGameOverStart()
{
	SetMatchState(MatchState::GameOver);
}

void AInfinityGameModeBase::HandleGameOver()
{
	// Just restart the game for now.
	RestartGame();
}

bool AInfinityGameModeBase::IsPasswordProtected() const
{
	return !ServerPassword.IsEmpty();
}

bool AInfinityGameModeBase::ShouldValidateClientSideHits() const
{
	return bValidateClientSideHits;
}