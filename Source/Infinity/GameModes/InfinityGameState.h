// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "InfinityGameState.generated.h"

class ATeamInfo;

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
	
	void SetRoundTimer(const int32 Seconds);

	void AddTeam(ATeamInfo* NewTeam);

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
};
