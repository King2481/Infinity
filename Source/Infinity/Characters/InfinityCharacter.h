// Made by Bruce Crum
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Infinity/Items/EquipableEnums.h"
#include "Infinity/Weapons/WeaponEnums.h"
#include "Infinity/Factions/TeamInterface.h"
#include "InfinityCharacter.generated.h"

class UInfinityMovementComponent;
class UCameraComponent;
class AItemBase;
class AItemEquipable;
class UPowerUp;

USTRUCT(BlueprintType)
struct FStoredAmmo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(BlueprintReadWrite)
	int32 Ammo;

	FStoredAmmo()
	{
		AmmoType = EAmmoType::AT_None;
		Ammo = 0;
	}

	FStoredAmmo(EAmmoType Type, int32 StoreAmount)
	{
		AmmoType = Type;
		Ammo = StoreAmount;
	}
};

UCLASS()
class INFINITY_API AInfinityCharacter : public ACharacter, public ITeamInterface
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AInfinityCharacter(const FObjectInitializer& ObjectInitializer);

	// Are we currently at or beyond our maximum health?
	UFUNCTION(BlueprintPure, Category = "Character")
	bool IsAtOrBeyondMaxHealth() const;

	// Are we currently at or beyond our maximum armor?
	UFUNCTION(BlueprintPure, Category = "Character")
	bool IsAtOrBeyondMaxArmor() const;

	// Restores the characters health by this amount (Server Only)
	UFUNCTION(BlueprintCallable, Category = "Character")
	void RestoreHealth(const float Amount);

	// Adds armor by this amount (Server Only)
	UFUNCTION(BlueprintCallable, Category = "Character")
	void AddArmor(const float Amount);

	// Adds an item to our inventory
	UFUNCTION(BlueprintCallable, Category = "Character")
	void AddItemToInventory(TSubclassOf<AItemBase> ItemToAdd);

	// Checks to see if an item class exists in our iventory
	UFUNCTION(BlueprintPure, Category = "Character")
	bool HasItemInInventory(TSubclassOf<AItemBase> ItemToFind) const;

	// Attempts to equip the first available inventory item.
	void EquipFirstAvailableInventoryItem();

	// Equips an item
	void EquipItem(AItemEquipable* Item);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEquipItem(AItemEquipable* Item);
	void ServerEquipItem_Implementation(AItemEquipable* Item);
	bool ServerEquipItem_Validate(AItemEquipable* Item);

	// Sets the current equipable
	void SetCurrentEquipable(AItemEquipable* Item, bool bFromReplication = false);

	// Gives a power up to this character
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GivePowerUp(TSubclassOf<UPowerUp> PowerUpClass);

	// Deactivates all power ups for this character
	UFUNCTION(BlueprintCallable, Category = "Character")
	void DeactivateAnyPowerUps();

	// Checks to see if we have any active powerups.
	UFUNCTION(BlueprintPure, Category = "Character")
	bool HasAnyActivePowerUps() const;

	// Checks to see if we have a specific powerup
	UFUNCTION(BlueprintPure, Category = "Character")
	bool HasSpecificPowerUp(TSubclassOf<UPowerUp> PowerUpClassToFind) const;

	// Removes an instance of a powerup
	UFUNCTION(BlueprintCallable, Category = "Character")
	void RemovePowerUp(UPowerUp* PowerUpToFind);

	// Returns the first person weapon mesh for the character
	UFUNCTION(BlueprintPure, Category = "Character")
	USkeletalMeshComponent* GetWeaponMesh1P() const;
	
	// Returns the camera component's location for this charater
	UFUNCTION(BlueprintPure, Category = "Character")
	FVector GetCameraLocation() const;

	// Sets the new first person mesh for the weapon
	void SetupFirstPersonWeaponMesh(USkeletalMesh* NewFirstPersonMesh);

	// Gets the next item for this item slot
	AItemEquipable* GetNextItemInSlot(EItemSlot Slot, AItemEquipable* CurrentItem, bool bFallbackToFirst);

	// Gets the previous for this item slot
	AItemEquipable* GetPreviousItemInSlot(EItemSlot Slot, AItemEquipable* CurrentItem, bool bFallbackToLast);

	// Updates a characters base damage multiplier
	UFUNCTION(BlueprintCallable, Category = "Character")
	void UpdateCharacterDamageMultiplier(float DamageMultiplier);

	// Get any damage multipliers this character may have.
	UFUNCTION(BlueprintPure, Category = "Character")
	float GetDamageMultipliers() const;

	// Checks to see if we are allowed to weapon swap
	UFUNCTION(BlueprintPure, Category = "Character")
	bool AllowWeaponSwapping() const;

	// Adds additional ammo for the specified ammo type.
	UFUNCTION(BlueprintCallable, Category = "Character")
	void GiveAmmo(EAmmoType AmmoType, int32 AmountToGive);

	// Stores whatever ammo we have for the specifed ammo type. (Note, this will OVERWRITE the stored ammo, if you wish to add ammo. Please use GiveAmmo())
	UFUNCTION(BlueprintCallable, Category = "Character")
	void StoreAmmo(EAmmoType AmmoType, int32 AmountToStore);

	// Returns the amount of ammo we have for the specifed type.
	UFUNCTION(BlueprintPure, Category = "Character")
	int32 GetAmmoAmountForType(EAmmoType AmmoType) const;

	// Changes the FOV for this character (does not blend)
	UFUNCTION(BlueprintCallable, Category = "Character")
	void ChangeFOV(const float NewFOV);

	virtual uint8 GetTeamId() const override;

protected:

	// What team does this character belong to?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TeamId, BlueprintReadOnly, Category = "Character")
	uint8 TeamId;

	UFUNCTION()
	void OnRep_TeamId();

	// This character damage multiplier (server only)
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	float CharacterDamageMultiplier;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Intializes the character
	void InitCharacter();

	// Called when components have been initialized.
	virtual void PostInitializeComponents() override;

	// Cache of the movement component
	UPROPERTY(BlueprintReadOnly, Category = "Character: Movement")
	UInfinityMovementComponent* InfinityMovement;

	// Is this character currently sliding?
	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing=OnRep_IsSliding, Category = "Character: Movement")
	bool bIsSliding;

	UFUNCTION()
	void OnRep_IsSliding();

	// The Camera for this character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UCameraComponent* CameraComponent;

	/** First person character mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* ArmMesh1P;

	/** Weapon mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* WeaponMesh1P;

	// Applies damage to the character.
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	// Modifies damage coming into the player. Factors in things like armor, team, etc.
	virtual float ModifyDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// Damages our armor
	virtual float DamageArmor(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// Called when we fall out of the world map.
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	// Checks to see if we should actually take damage.
	virtual bool ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const override;

	// Attempts to kill the pawn, returns true if successful.
	bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Checks to see if this pawn can die.
	bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	// Called when a character dies.
	void OnDeath();

	// Function for replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The current health of the character.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	float Health;

	// The max health of the character
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	float MaxHealth;

	// The Starting health of the character
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	float StartingHealth;

	// The current armor of the character.
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	float Armor;

	// The max armor of the character
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	float MaxArmor;

	// The starting armor of the character
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	float StartingArmor;

	// Is the character dead?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_IsDying, BlueprintReadOnly, Category = "Character")
	bool bIsDying;

	UFUNCTION()
	void OnRep_IsDying();

	/* Reliably broadcasts a death event to clients, used to apply ragdoll forces */
	UFUNCTION(NetMulticast, Reliable)
	void BroadcastDeath(const FVector_NetQuantize& HitPosition, const FVector_NetQuantize& DamageForce, const FName& BoneName);
	void BroadcastDeath_Implementation(const FVector_NetQuantize& HitPosition, const FVector_NetQuantize& DamageForce, const FName& BoneName);

	// The stored ammo for this character
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Character")
	TArray<FStoredAmmo> StoredAmmo;

	// Destroys all inventory items. Must be called on Authority
	void DestroyInventoryItems();

	UPROPERTY(Replicated, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentEquipable, Category = "Character")
	AItemEquipable* CurrentEquipable;

	UFUNCTION()
	void OnRep_CurrentEquipable();

	// This characters inventory
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_Inventory, Category = "Inventory")
	TArray<AItemBase*> Inventory;

	UFUNCTION()
	void OnRep_Inventory();

	// The default weapon this character starts with.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	TSubclassOf<AItemEquipable> DefaultWeapon;

	// The current powerups this character has
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PowerUps, Category = "Inventory")
	TArray<UPowerUp*> PowerUps;

	UFUNCTION()
	void OnRep_PowerUps();

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Character side checks for allowing slide (called from MovementComponents CanSlide()
	bool CanSlide() const;

protected:

	////////////////////////////////////////////////////////////////
	// Input

	void MoveForward(float Value);
	void MoveRight(float Value);

	void MouseX(float Value);
	void MouseY(float Value);

	void OnJumpPressed();
	void OnJumpReleased();

	void OnCrouchPressed();
	void OnCrouchReleased();

	void OnSlidePressed();
	void OnSlideReleased();

	void OnFirePressed();
	void OnFireReleased();

	void OnAltFirePressed();
	void OnAltFireReleased();

	void OnSelectWeaponSlotShotgun();
	void OnSelectWeaponSlotBullet();
	void OnSelectWeaponSlotEnergy();
	void OnSelectWeaponSlotExplosive();

	void OnSelectInventoryPrevious();
	void OnSelectInventoryNext();
};
