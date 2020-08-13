// Made by Bruce Crum

#include "PowerUp.h"
#include "Engine/World.h"
#include "Infinity/Characters/InfinityCharacter.h"

UPowerUp::UPowerUp()
{
	PowerUpLength = 10.f;
	OwningCharacter = nullptr;
}

void UPowerUp::Activate(AInfinityCharacter* ForCharacter)
{
	const auto World = GetWorld();
	if (!World)
	{
		return;
	}
	
	World->GetTimerManager().SetTimer(DeactivateTimerHandle, this, &UPowerUp::OnPowerUpLengthReached, PowerUpLength);
	OwningCharacter = ForCharacter;
	
	BlueprintActivate(ForCharacter);
}

void UPowerUp::Deactivate(AInfinityCharacter* ForCharacter)
{
	const auto World = GetWorld();
	if (!World)
	{
		return;
	}

	// Kill any potential lingering timer handles
	World->GetTimerManager().ClearTimer(DeactivateTimerHandle);
	BlueprintDeactivate(ForCharacter);

	// Note: From what I've read, we don't need to destroy UObjects, it is automatically handled during
	//       the garbage collection pass when there are no more references.
}

void UPowerUp::OnPowerUpLengthReached()
{
	if (OwningCharacter)
	{
		OwningCharacter->RemovePowerUp(this);
		Deactivate(OwningCharacter);
	}
}
