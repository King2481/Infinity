// Made by Bruce Crum


#include "TeamDeathMatch.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Infinity/Player/InfinityPlayerState.h"
#include "Infinity/Characters/InfinityCharacter.h"
#include "Infinity/Gamemodes/InfinityGameState.h"
#include "Infinity/Factions/TeamInfo.h"

ATeamDeathMatch::ATeamDeathMatch()
{
	ScoreNeededToWin = 75;
}

void ATeamDeathMatch::OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::OnCharacterKilled(Victim, KillingDamage, DamageEvent, EventInstigator, DamageCauser);

	const auto VictimPS = Cast<AInfinityPlayerState>(Victim->GetPlayerState());
	const auto KillerPS = EventInstigator ? EventInstigator->GetPlayerState<AInfinityPlayerState>() : nullptr;
	const auto GS = Cast<AInfinityGameState>(GameState);
	const bool bSelfKill = VictimPS == KillerPS;

	if (KillerPS && !bSelfKill && GS)
	{
		const auto Team = GS->GetTeamFromId(KillerPS->GetTeamId());
		if (Team)
		{
			Team->ScorePoints();

			if (Team->GetScore() >= ScoreNeededToWin)
			{
				// Winner, inform everyone.
				WinningTeamId =  Team->GetTeamId();
				SetMatchState(MatchState::RoundWon);
			}
		}
	}
	
	const auto Controller = Cast<AInfinityPlayerController>(Victim->GetController());
	if (Controller)
	{
		Controller->QueueRespawnDelay(2.25f);
	}
}