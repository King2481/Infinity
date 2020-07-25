// Copyright Epic Games, Inc. All Rights Reserved.


#include "InfinityGameModeBase.h"

#include "Infinity/Gamemodes/InfinityGameModeBase.h"
#include "Infinity/Characters/InfinityCharacter.h"
#include "Infinity/Gamemodes/InfinityGameState.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Infinity/Player/InfinityPlayerState.h"
#include "Infinity/HUD/InfinityHUD.h"
#include "UObject/ConstructorHelpers.h" 
#include "Kismet/GameplayStatics.h"

AInfinityGameModeBase::AInfinityGameModeBase()
{
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
}

void AInfinityGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// Password
	ServerPassword = UGameplayStatics::ParseOption(Options, TEXT("Password"));
	if (!ServerPassword.IsEmpty())
	{
		UE_LOG(LogGameMode, Display, TEXT("Server is now password protected!"));
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


void AInfinityGameModeBase::OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!Victim)
	{
		UE_LOG(LogGameMode, Warning, TEXT("A character was killed, but the victim was nullptr."));
		return;
	}

	const auto VictimPS = Cast<AInfinityPlayerState>(Victim->GetPlayerState());
	const auto KillerPS = EventInstigator ? EventInstigator->GetPlayerState<AInfinityPlayerState>() : nullptr;

	if (VictimPS && KillerPS)
	{
		UE_LOG(LogGameMode, VeryVerbose, TEXT("%s Was killed by %s"), *VictimPS->GetPlayerName(), *KillerPS->GetPlayerName());
	}
}

float AInfinityGameModeBase::OnCharacterTakeDamage(AInfinityCharacter* Reciever, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	float AlteredDamage = Damage * UniversalDamageMultiplayer;

	const auto DamagedController = Reciever ? Reciever->GetController() : nullptr;
	const auto DamagerController = EventInstigator;
	const bool bSelfDamage = DamagedController == DamagerController;

	if (bSelfDamage)
	{
		AlteredDamage *= SelfDamageMultiplier;
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
	HandleMatchHasStarted();
}

bool AInfinityGameModeBase::IsPasswordProtected() const
{
	return !ServerPassword.IsEmpty();
}

bool AInfinityGameModeBase::ShouldValidateClientSideHits() const
{
	return bValidateClientSideHits;
}