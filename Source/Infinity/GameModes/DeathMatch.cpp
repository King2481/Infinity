// Made by Bruce Crum


#include "DeathMatch.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Infinity/Player/InfinityPlayerState.h"
#include "Infinity/Characters/InfinityCharacter.h"

ADeathMatch::ADeathMatch()
{
	ScoreNeededToWin = 30;
}

void ADeathMatch::OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::OnCharacterKilled(Victim, KillingDamage, DamageEvent, EventInstigator, DamageCauser);

	const auto KillerPS = EventInstigator ? EventInstigator->GetPlayerState<AInfinityPlayerState>() : nullptr;
	if (KillerPS)
	{
		if (KillerPS->GetMatchStats().Kills >= ScoreNeededToWin)
		{
			// Winner, inform everyone.
			WinningPlayerState = KillerPS;
			SetMatchState(MatchState::RoundWon);
		}
	}

	const auto Controller = Cast<AInfinityPlayerController>(Victim->GetController());
	if (Controller)
	{
		Controller->QueueRespawnDelay(2.25f);
	}
}