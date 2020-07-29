// Made by Bruce Crum


#include "InfinityPlayerState.h"
#include "Net/UnrealNetwork.h"

AInfinityPlayerState::AInfinityPlayerState()
{

}

void AInfinityPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AInfinityPlayerState, MatchStats);
}

FMatchStats AInfinityPlayerState::GetMatchStats() const
{
	return MatchStats;
}

void AInfinityPlayerState::ScoreKill(int32 Amount)
{
	MatchStats.Kills += Amount;
}

void AInfinityPlayerState::ScoreAssist(int32 Amount)
{
	MatchStats.Assists += Amount;
}

void AInfinityPlayerState::ScoreDeath(int32 Amount)
{
	MatchStats.Deaths += Amount;
}