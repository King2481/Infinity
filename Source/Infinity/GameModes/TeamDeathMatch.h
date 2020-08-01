// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Infinity/GameModes/InfinityGameModeBase.h"
#include "TeamDeathMatch.generated.h"

/**
 * 
 */
UCLASS(Config = Game)
class INFINITY_API ATeamDeathMatch : public AInfinityGameModeBase
{
	GENERATED_BODY()

public:

	ATeamDeathMatch();

	virtual void OnCharacterKilled(AInfinityCharacter* Victim, float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
};
