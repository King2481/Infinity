// Made by Bruce Crum


#include "InfinityPlayerState.h"
#include "Net/UnrealNetwork.h"

AInfinityPlayerState::AInfinityPlayerState()
{
	TeamId = ITeamInterface::InvalidId;
}

void AInfinityPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AInfinityPlayerState, MatchStats);
	DOREPLIFETIME(AInfinityPlayerState, TeamId);
}

FMatchStats AInfinityPlayerState::GetMatchStats() const
{
	return MatchStats;
}

uint8 AInfinityPlayerState::GetTeamId() const
{
	return TeamId;
}

void AInfinityPlayerState::SetTeamId(uint8 NewTeam)
{
	TeamId = NewTeam;
}

void AInfinityPlayerState::ScoreKill(int32 Amount)
{
	MatchStats.Kills += Amount;
	OnMatchStatsUpdated();
}

void AInfinityPlayerState::ScoreAssist(int32 Amount)
{
	MatchStats.Assists += Amount;
	OnMatchStatsUpdated();
}

void AInfinityPlayerState::ScoreDeath(int32 Amount)
{
	MatchStats.Deaths += Amount;
	OnMatchStatsUpdated();
}

void AInfinityPlayerState::OnMatchStatsUpdated()
{
	OnMatchStatsUpdatedDelegate.Broadcast();
}

void AInfinityPlayerState::OnRep_TeamId()
{

}

void AInfinityPlayerState::OnRep_MatchStats()
{
	OnMatchStatsUpdated();
}