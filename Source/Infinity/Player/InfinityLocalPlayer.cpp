// Made by Bruce Crum


#include "InfinityLocalPlayer.h"

UInfinityLocalPlayer::UInfinityLocalPlayer()
{
	PendingJoinPassword = FString("");
}

void UInfinityLocalPlayer::SetNextJoinPassword(const FString& Password)
{
	PendingJoinPassword = Password;
}

FString UInfinityLocalPlayer::GetNextJoinPassword() const
{
	return PendingJoinPassword;
}
