// Made by Bruce Crum


#include "TeamDeathMatch.h"
#include "Infinity/Player/InfinityPlayerController.h"
#include "Infinity/Player/InfinityPlayerState.h"
#include "Infinity/Characters/InfinityCharacter.h"

ATeamDeathMatch::ATeamDeathMatch()
{
	ScoreNeededToWin = 75;
}

void ATeamDeathMatch::OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::OnCharacterKilled(Victim, KillingDamage, DamageEvent, EventInstigator, DamageCauser);

	const auto Controller = Cast<AInfinityPlayerController>(Victim->GetController());
	if (Controller)
	{
		Controller->QueueRespawnDelay(2.25f);
	}
}