// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "InfinityGameState.generated.h"

class ATeamInfo;
class AInfinityPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerAmmountChangedDelegate);

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityGameState : public AGameState
{
	GENERATED_BODY()

public:

	AInfinityGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Sets the round timer for this many seconds
	void SetRoundTimer(const int32 Seconds);

	// Adds a team to the game
	void AddTeam(ATeamInfo* NewTeam);

	// Returns the team info for the specified TeamId
	UFUNCTION(BlueprintPure, Category = "Game State")
	ATeamInfo* GetTeamFromId(const uint8 TeamId) const;

	// Adds a player to the specifed TeamId
	void AddPlayerForTeam(AInfinityPlayerState* ForPlayer, uint8 TeamId);

	// Add PlayerState from the PlayerArray.
	virtual void AddPlayerState(APlayerState* PlayerState) override;

	// Remove PlayerState from the PlayerArray.
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

protected:

	// When is this game mode expected to end?
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Game State")
	float RoundEndTime;

	// Returns the estimated amount of time left for this round.
	UFUNCTION(BlueprintPure, Category = "Game State")
	float GetRoundTimeRemaining() const;

	// What teams have been created and are currently in play?
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Gamemode")
	TArray<ATeamInfo*> Teams;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAmmountChangedDelegate OnPlayerAmmountChangedDelegate;
};
