// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Infinity/Items/ItemBase.h"
#include "Infinity/Items/EquipableEnums.h"
#include "ItemEquipable.generated.h"

class AInfinityCharacter;

UENUM(BlueprintType)
enum class EEquipableState : uint8
{
	Unequipped 	  UMETA(DisplayName = "Unequipped"),
	Unequipping   UMETA(DisplayName = "Unequipping"),
	Equipping	  UMETA(DisplayName = "Equipping"),
	Equipped      UMETA(DisplayName = "Equipped")
};

/**
 * 
 */
UCLASS()
class INFINITY_API AItemEquipable : public AItemBase
{
	GENERATED_BODY()

public:

	AItemEquipable();

	// Called when we press the fire button
	virtual void OnFirePressed() { };

	// Called when we release the fire button
	virtual void OnFireReleased() { };

	// Called when we equip
	virtual void Equip();

	// Called when we unequip
	virtual void Unequip();

	// Can we equip this item?
	virtual bool CanEquip() const;

	// Initializes the item
	virtual void InitItem(AInfinityCharacter* NewOwner) override;

	// The item slot
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Item")
	EItemSlot ItemSlot;

protected:

	// Set the equipable state.
	void SetEquipableState(EEquipableState NewState);

	// Returns the current equipable state of the weapon, will return simulated equippable state on remote clients.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	EEquipableState GetEquipableState() const;

	// Variable replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The current equipable state of the item.
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_EquipableState, BlueprintReadOnly, Category = "Item")
	EEquipableState EquipableState;

	// The simulated equipable state of the item (clients)
	UPROPERTY(BlueprintReadOnly, Category = "Item")
	EEquipableState SimulatedEquipableState;

	UFUNCTION()
	void OnRep_EquipableState();

	// Called when the equipable state has changed.
	void OnEquipableStateChanged();

	// Called when equipped.
	virtual void OnEquipped();

	// Called when Unequipped
	virtual void OnUnequipped();
	
};
