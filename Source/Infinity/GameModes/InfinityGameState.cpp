// Made by Bruce Crum


#include "InfinityGameState.h"
#include "Net/UnrealNetwork.h"
#include "Infinity/Factions/TeamInfo.h"

AInfinityGameState::AInfinityGameState()
{
	RoundEndTime = 0.f;
}

void AInfinityGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInfinityGameState, RoundEndTime);
	DOREPLIFETIME(AInfinityGameState, Teams);
}

void AInfinityGameState::SetRoundTimer(const int32 Seconds)
{
	if (!HasAuthority())
	{
		return;
	}

	RoundEndTime = GetServerWorldTimeSeconds() + Seconds;
}

float AInfinityGameState::GetRoundTimeRemaining() const
{
	return FMath::Max(RoundEndTime - GetServerWorldTimeSeconds(), 0.0f);
}

void AInfinityGameState::AddTeam(ATeamInfo* NewTeam)
{
	Teams.AddUnique(NewTeam);
}

ATeamInfo* AInfinityGameState::GetTeamFromId(const uint8 TeamId) const
{
	if (Teams.Num() <= 0)
	{
		// No Teams
		return nullptr;
	}

	for (auto& Team : Teams)
	{
		if (Team && Team->GetTeamId() == TeamId)
		{
			return Team;
		}
	}

	return nullptr;
}

void AInfinityGameState::AddPlayerForTeam(AInfinityPlayerState* ForPlayer, uint8 TeamId)
{
	const auto Team = GetTeamFromId(TeamId);
	if (!Team)
	{
		return;
	}

	Team->AddPlayer(ForPlayer);
}
