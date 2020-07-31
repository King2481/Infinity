// Made by Bruce Crum


#include "InfinityGameState.h"
#include "Net/UnrealNetwork.h"

AInfinityGameState::AInfinityGameState()
{
	RoundEndTime = 0.f;
}

void AInfinityGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInfinityGameState, RoundEndTime);
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
