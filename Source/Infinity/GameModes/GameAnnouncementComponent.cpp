// Made by Bruce Crum


#include "GameAnnouncementComponent.h"

// Sets default values for this component's properties
UGameAnnouncementComponent::UGameAnnouncementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UGameAnnouncementComponent::OnCharacterKilled(AInfinityCharacter* Victim, AInfinityPlayerState* VictimPlayerState, AInfinityPlayerState* KillerPlayerState, AController* EventInstigator)
{
}
