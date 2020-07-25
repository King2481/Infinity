// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PowerUp.generated.h"

class AInfinityCharacter;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INFINITY_API UPowerUp : public UObject
{
	GENERATED_BODY()

public:

	UPowerUp();

	// Activates the power up.
	void Activate(AInfinityCharacter* ForCharacter);

	// Blueprint event for Activate()
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void BlueprintActivate(AInfinityCharacter* ForUser);

	// Deactivates the power up.
	void Deactivate(AInfinityCharacter* ForCharacter);

	// Blueprint event for Activate()
	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void BlueprintDeactivate(AInfinityCharacter* ForUser);

protected:

	// How long does this power up last once picked up?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Power Up")
	float PowerUpLength;

	// The character the power up is applied to.
	UPROPERTY(BlueprintReadOnly, Category = "Power Up")
	AInfinityCharacter* OwningCharacter;

	UPROPERTY()
	FTimerHandle DeactivateTimerHandle;

	// Callbacvk for when the power up has naturally ended.
	void OnPowerUpLengthReached();

};
