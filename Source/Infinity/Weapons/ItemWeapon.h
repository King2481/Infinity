// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Infinity/Items/ItemEquipable.h"
#include "Infinity/Weapons/WeaponEnums.h"
#include "ItemWeapon.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeapon, Log, Verbose);

class USurfaceReaction;

/**
 * 
 */
UCLASS()
class INFINITY_API AItemWeapon : public AItemEquipable
{
	GENERATED_BODY()

public:

	AItemWeapon();

	// Initializes the item
	virtual void InitItem(AInfinityCharacter* NewOwner) override;

	// What socket is this weapon going to attach to?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	FName AttachWeaponSocketName;

	// Are we allowed to "fire" this weapon?
	virtual bool AllowFire() const;

	// Variable replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Will attempt to decrement ammo, will not decremtn ammo is bConsumesAmmo is set to false.
	void DecrementAmmo(int32 Amount = 1);

	// Updates ammo, may give or take based on what the owning characters stored ammo is.
	void UpdateAmmo();

	// Will play a sound related to the weapon at this location.
	void PlayWeaponSoundAtLocation(USoundBase* Sound, const FVector& Location);

	// Gets the direction we are currently aiming
	FVector GetAdjustedAim() const;

	// Checks to see where we should start the trace.
	FVector GetCameraDamageStartLocation() const; 

	// Returns a damage multiplier for when we are dealing damage.
	virtual float GetDamageMultiplier(TWeakObjectPtr<AActor> HitActor, TWeakObjectPtr<UPhysicalMaterial> HitMaterial) const;

	// Called when we equip the weapon
	virtual void Equip() override;

protected:

	// Checks to see if we can actually deal damage.
	virtual bool ShouldDealDamage(AActor* TestActor) const;

	// Deals damage to the thing we hit.
	void DealDamage(const FHitResult& Impact, float DamageAmount, const FVector& ShootDir);

	// What is the damage type of this weapon?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageTypeClass;

	// What is the "ammo" for this weapon? 
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Weapon")
	int32 Ammo;

	// What is the ammo type for this weapon?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	EAmmoType AmmoType;

	// Does this weapon consume ammo when fired?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	bool bConsumesAmmo;

	// How much ammo does this weapon start with?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	int32 StartingAmmo;

	// Maping for damage multiplier, use this if you want specific bonuses/penalties to apply when a weapon does damage against something.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	TMap<UPhysicalMaterial*, float> DamageMultiplierMap;

	// What surface reaction does this weapon uses when it hits something.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<USurfaceReaction> SurfaceReaction;
	
};
