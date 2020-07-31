// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "InfinityGameModeBase.generated.h"

class AInfinityCharacter;
class AInfinityPlayerState;
class UGameAnnouncementComponent;

/**
* Additional match states
*/
namespace MatchState
{
	/* round has ended and a winner (or draw) has been declared */
	extern const FName RoundWon;

	/*The game is officially "over".*/
	extern const FName GameOver;
}

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityGameModeBase : public AGameMode
{
	GENERATED_BODY()

public:

	AInfinityGameModeBase();

	// Called when the game is initialized
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	// Called before a client attempts to join this match
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	// Called when a character is killed.
	virtual void OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Called when a character takes damage. Gamemodes may modify damage based on certain conditions.
	virtual float OnCharacterTakeDamage(AInfinityCharacter* Reciever, float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// True if the server requires a password to join 
	bool IsPasswordProtected() const;

	// Gamemode rulling checking to see if we should confirm clientside hits.
	bool ShouldValidateClientSideHits() const;

	virtual void HandleMatchHasStarted() override;

	// Called when we have "won" the Gamemode and need to handle it.
	virtual void HandleRoundWon();

	void OnGameOverStart();

	virtual void HandleGameOver();

protected:

	// Game Announcement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gamemode")
	UGameAnnouncementComponent* GameAnnouncement;

	// Override handling of match state changing to accomodate our custom states 
	virtual void SetMatchState(FName NewState) override;

	// Called when the match state has been changed.
	virtual void CallMatchStateChangeNotify();

	// A generic score that is needed to "win" this gamemode, generally used in other gamemode classes as part of their win condition checks.
	UPROPERTY(Config, BlueprintReadOnly, Category = "Gamemode")
	int32 ScoreNeededToWin;

	// What is the universal damage multiplayer?
	UPROPERTY(Config, BlueprintReadOnly, Category = "Gamemode")
	float UniversalDamageMultiplayer;

	// What is the self damage multiplier?
	UPROPERTY(Config, BlueprintReadOnly, Category = "Gamemode")
	float SelfDamageMultiplier;

	// Should we validate client side hits? This is checked only for players, as bot hits are never validated.
	UPROPERTY(Config, BlueprintReadOnly, Category = "Gamemode")
	bool bValidateClientSideHits;

	// Password required to connect to the server 
	FString ServerPassword;

	// How long does this game mode last, in seconds?
	UPROPERTY(Config, BlueprintReadOnly, Category = "Gamemode")
	int32 RoundTimeLimit;

	// This sole winner of this gamemode.
	AInfinityPlayerState* WinningPlayerState;

	// The winning team of this gamemode
	uint8 WinningTeamId;

	FTimerHandle GameOverTimerHandle;
};
