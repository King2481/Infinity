// Made by Bruce Crum


#include "InfinityCharacter.h"
#include "InfinityMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Infinity/Items/ItemBase.h"
#include "Infinity/Items/ItemEquipable.h"
#include "Infinity/GameModes/InfinityGameModeBase.h"
#include "Infinity/Weapons/PowerUp.h"
#include "Infinity/Weapons/ItemWeapon.h"

// Sets default values
AInfinityCharacter::AInfinityCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UInfinityMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	bReplicates = true;
	GetMesh()->bOwnerNoSee = true;
	JumpMaxCount = 2;

	Health = 0.f;
	StartingHealth = 100.f;
	MaxHealth = 150.f;
	Armor = 0.f;
	MaxArmor = 100.f;
	StartingArmor = 0.f;
	bIsDying = false;
	CurrentEquipable = nullptr;
	DefaultWeapon = nullptr;
	CharacterDamageMultiplier = 1.f;
	
	bIsSliding = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);
	CameraComponent->bUsePawnControlRotation = true;

	ArmMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmMesh1P"));
	ArmMesh1P->SetupAttachment(CameraComponent);
	ArmMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	ArmMesh1P->CastShadow = false;
	ArmMesh1P->bOnlyOwnerSee = true;

	WeaponMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	WeaponMesh1P->SetupAttachment(ArmMesh1P);
	WeaponMesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	WeaponMesh1P->CastShadow = false;
	WeaponMesh1P->bOnlyOwnerSee = true;
}

// Called when the game starts or when spawned
void AInfinityCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitCharacter();
	
}

void AInfinityCharacter::InitCharacter()
{
	if (!HasAuthority())
	{
		return;
	}

	Health = StartingHealth;
	Armor = StartingArmor;

	if (DefaultWeapon)
	{
		AddItemToInventory(DefaultWeapon);
		EquipFirstAvailableInventoryItem();
	}
}

void AInfinityCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	InfinityMovement = Cast<UInfinityMovementComponent>(GetCharacterMovement());
	if (InfinityMovement)
	{
		InfinityMovement->OwningCharacter = this;
	}
}

void AInfinityCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone.
	DOREPLIFETIME(AInfinityCharacter, Health);
	DOREPLIFETIME(AInfinityCharacter, Armor);
	DOREPLIFETIME(AInfinityCharacter, bIsDying);
	DOREPLIFETIME(AInfinityCharacter, CurrentEquipable)
	DOREPLIFETIME(AInfinityCharacter, Inventory);
	DOREPLIFETIME(AInfinityCharacter, PowerUps);

	// Owner only
	DOREPLIFETIME_CONDITION(AInfinityCharacter, StoredAmmo, COND_OwnerOnly);

	// Third Parties
	DOREPLIFETIME_CONDITION(AInfinityCharacter, bIsSliding, COND_SkipOwner);
}

// Called every frame
void AInfinityCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AInfinityCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		FHitResult HitInfo;
		FVector MomentumDir;
		DamageEvent.GetBestHitInfo(this, EventInstigator, HitInfo, MomentumDir);

		const auto GM = GetWorld()->GetAuthGameMode<AInfinityGameModeBase>();
		if (GM)
		{
			// Gamemode may want to modify
			ActualDamage = GM->OnCharacterTakeDamage(this, ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}

		// Apply modifiers. 
		ActualDamage = ModifyDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);

		Health -= ActualDamage;

		if (Armor > 0)
		{
			Armor = FMath::Max<float>(Armor - DamageArmor(ActualDamage, DamageEvent, EventInstigator, DamageCauser), 0);
		}

		if (Health <= 0)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}

		return ActualDamage;
	}

	return 0.f;
}

float AInfinityCharacter::ModifyDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return DamageAmount;
	/*float AlteredDamage = DamageAmount * ArmorDamageMitigationCurve.GetRichCurveConst()->Eval(Armor);
	return AlteredDamage;*/
}

float AInfinityCharacter::DamageArmor(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	// TODO: better calculation for armor damage.
	return DamageAmount * 3.f;
}

void AInfinityCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	// TODO: Custom Damage Type Class for Falling out of world.
	Die(Health, FDamageEvent(dmgType.GetClass()), GetController(), nullptr);
}

bool AInfinityCharacter::ShouldTakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	return Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

bool AInfinityCharacter::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, EventInstigator, DamageCauser))
	{
		return false;
	}

	const auto GM = GetWorld()->GetAuthGameMode<AInfinityGameModeBase>();
	if (GM)
	{
		// Inform the Game mode.
		GM->OnCharacterKilled(this, KillingDamage, DamageEvent, EventInstigator, DamageCauser);
	}

	OnDeath();

	return true;
}

bool AInfinityCharacter::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| GetLocalRole() != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode<AInfinityGameModeBase>() == NULL
		|| GetWorld()->GetAuthGameMode<AInfinityGameModeBase>()->GetMatchState() == MatchState::LeavingMap) // level transition occurring
	{
		return false;
	}

	return true;
}

void AInfinityCharacter::OnDeath()
{
	bIsDying = true;

	DetachFromControllerPendingDestroy();

	SetReplicateMovement(false);
	TearOff();
	SetLifeSpan(30.f);
	DestroyInventoryItems();
	DeactivateAnyPowerUps();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponMesh1P->SetHiddenInGame(true);
	ArmMesh1P->SetHiddenInGame(true);

	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
}

void AInfinityCharacter::OnRep_IsDying()
{
	OnDeath();
}

void AInfinityCharacter::OnRep_Inventory()
{

}

void AInfinityCharacter::OnRep_CurrentEquipable()
{

}

void AInfinityCharacter::OnRep_PowerUps()
{

}

void AInfinityCharacter::OnRep_IsSliding()
{

}

// Called to bind functionality to input
void AInfinityCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AInfinityCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AInfinityCharacter::MoveRight);

	PlayerInputComponent->BindAxis("MouseY", this, &AInfinityCharacter::MouseY);
	PlayerInputComponent->BindAxis("MouseX", this, &AInfinityCharacter::MouseX);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AInfinityCharacter::OnJumpPressed);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AInfinityCharacter::OnJumpReleased);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AInfinityCharacter::OnCrouchPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AInfinityCharacter::OnCrouchReleased);

	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AInfinityCharacter::OnSlidePressed);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &AInfinityCharacter::OnSlideReleased);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AInfinityCharacter::OnFirePressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AInfinityCharacter::OnFireReleased);

	PlayerInputComponent->BindAction("AltFire", IE_Pressed, this, &AInfinityCharacter::OnAltFirePressed);
	PlayerInputComponent->BindAction("AltFire", IE_Released, this, &AInfinityCharacter::OnAltFireReleased);

	PlayerInputComponent->BindAction("SelectWeaponSlotShotgun", IE_Pressed, this, &AInfinityCharacter::OnSelectWeaponSlotShotgun);
	PlayerInputComponent->BindAction("SelectWeaponSlotBullet", IE_Pressed, this, &AInfinityCharacter::OnSelectWeaponSlotBullet);
	PlayerInputComponent->BindAction("SelectWeaponSlotEnergy", IE_Pressed, this, &AInfinityCharacter::OnSelectWeaponSlotEnergy);
	PlayerInputComponent->BindAction("SelectWeaponSlotExplosive", IE_Pressed, this, &AInfinityCharacter::OnSelectWeaponSlotExplosive);

	PlayerInputComponent->BindAction("SelectInventoryPrevious", IE_Pressed, this, &AInfinityCharacter::OnSelectInventoryPrevious);
	PlayerInputComponent->BindAction("SelectInventoryNext", IE_Pressed, this, &AInfinityCharacter::OnSelectInventoryNext);
}

void AInfinityCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AInfinityCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AInfinityCharacter::MouseX(float Value)
{
	AddControllerYawInput(Value);
}

void AInfinityCharacter::MouseY(float Value)
{
	AddControllerPitchInput(Value);
}

void AInfinityCharacter::OnJumpPressed()
{
	Jump();
}

void AInfinityCharacter::OnJumpReleased()
{
	StopJumping();
}

void AInfinityCharacter::OnCrouchPressed()
{
	Crouch();
}

void AInfinityCharacter::OnCrouchReleased()
{
	UnCrouch();
}

void AInfinityCharacter::OnSlidePressed()
{

}

void AInfinityCharacter::OnSlideReleased()
{

}

void AInfinityCharacter::OnFirePressed()
{
	if (CurrentEquipable)
	{
		CurrentEquipable->OnFirePressed();
	}
}

void AInfinityCharacter::OnFireReleased()
{
	if (CurrentEquipable)
	{
		CurrentEquipable->OnFireReleased();
	}
}

void AInfinityCharacter::OnAltFirePressed()
{
	if (CurrentEquipable)
	{
		CurrentEquipable->OnAltFirePressed();
	}
}

void AInfinityCharacter::OnAltFireReleased()
{
	if (CurrentEquipable)
	{
		CurrentEquipable->OnAltFireReleased();
	}
}

void AInfinityCharacter::OnSelectWeaponSlotShotgun()
{
	const auto Item = GetNextItemInSlot(EItemSlot::IS_Shotgun, CurrentEquipable, true);
	if (Item && Item != CurrentEquipable)
	{
		EquipItem(Item);
	}
}

void AInfinityCharacter::OnSelectWeaponSlotBullet()
{
	const auto Item = GetNextItemInSlot(EItemSlot::IS_Bullet, CurrentEquipable, true);
	if (Item && Item != CurrentEquipable)
	{
		EquipItem(Item);
	}
}

void AInfinityCharacter::OnSelectWeaponSlotEnergy()
{
	const auto Item = GetNextItemInSlot(EItemSlot::IS_Energy, CurrentEquipable, true);
	if (Item && Item != CurrentEquipable)
	{
		EquipItem(Item);
	}
}

void AInfinityCharacter::OnSelectWeaponSlotExplosive()
{
	const auto Item = GetNextItemInSlot(EItemSlot::IS_Explosive, CurrentEquipable, true);
	if (Item && Item != CurrentEquipable)
	{
		EquipItem(Item);
	}
}

void AInfinityCharacter::OnSelectInventoryPrevious()
{
	if (!CurrentEquipable)
	{
		EquipFirstAvailableInventoryItem();
		return;
	}

	if (!AllowWeaponSwapping())
	{
		return;
	}

	AItemEquipable* ActiveItem = CurrentEquipable;
	AItemEquipable* Item = nullptr;

	// if we have a weapon, we want to try the current slot first, then the next one
	int CurrentSlot = (int)ActiveItem->ItemSlot;
	int OriginalSlot = CurrentSlot;
	Item = GetPreviousItemInSlot((EItemSlot)CurrentSlot, ActiveItem, false);

	// nothing else in the current slot, move on
	while (Item == nullptr)
	{
		--CurrentSlot;

		if (CurrentSlot < (int)EItemSlot::IS_Shotgun)
		{
			return;
		}

		// if we got back to our original slot, give up
		if (CurrentSlot == OriginalSlot)
		{
			return;
		}

		Item = GetPreviousItemInSlot((EItemSlot)CurrentSlot, nullptr, false);
	}

	// invalid item
	if (Item == nullptr)
	{
		return;
	}

	EquipItem(Item);
}

void AInfinityCharacter::OnSelectInventoryNext()
{
	if (!CurrentEquipable)
	{
		EquipFirstAvailableInventoryItem();
		return;
	}

	if (!AllowWeaponSwapping())
	{
		return;
	}

	AItemEquipable* ActiveItem = CurrentEquipable;
	AItemEquipable* Item = nullptr;

	// if we have a weapon, we want to try the current slot first, then the next one
	int CurrentSlot = (int)ActiveItem->ItemSlot;
	int OriginalSlot = CurrentSlot;
	Item = GetNextItemInSlot((EItemSlot)CurrentSlot, ActiveItem, false);

	// nothing else in the current slot, move on
	while (Item == nullptr)
	{
		++CurrentSlot;

		// roll over back to primary
		if (CurrentSlot >= (int)EItemSlot::IS_Count)
		{
			return;
		}

		// if we got back to our original slot, give up
		if (CurrentSlot == OriginalSlot)
		{
			return;
		}

		Item = GetNextItemInSlot((EItemSlot)CurrentSlot, nullptr, false);
	}

	// invalid item
	if (Item == nullptr)
	{
		return;
	}

	EquipItem(Item);
}

AItemEquipable* AInfinityCharacter::GetNextItemInSlot(EItemSlot Slot, AItemEquipable* CurrentItem, bool bFallbackToFirst)
{
	const bool bCurrentInSameSlot = (CurrentItem && CurrentItem->ItemSlot == Slot); // Are we transitioning from a weapon in the same slot?
	bool bHasPassedCurrentItem = false;
	AItemEquipable* FirstItemInSlot = nullptr;

	// Iterate through our entire inventory
	for (auto Item : Inventory)
	{
		auto Equipable = Cast<AItemEquipable>(Item);

		// Skip null items or anything not matching our slot
		if (!Equipable || Equipable->IsPendingKillPending() || Equipable->ItemSlot != Slot || !Equipable->CanEquip())
		{
			continue;
		}

		// Not transitioning from an item in the same slot, the first item is fine
		if (!bCurrentInSameSlot)
		{
			return Equipable;
		}

		// Record the first item we encounter in this specific slot
		if (FirstItemInSlot == nullptr)
		{
			FirstItemInSlot = Equipable;
		}

		if (!bHasPassedCurrentItem)
		{
			// Flag if we've reached our CurrentItem, the next weapon in this slot is good to use
			bHasPassedCurrentItem = (CurrentItem == Equipable);
		}
		else
		{
			// We've passed our CurrentItem and hit a new weapon in the same slot
			return Equipable;
		}
	}

	// Unable to find a "next" item, assume that we reached the end of our inventory and are looping back to the first
	if (bFallbackToFirst && bHasPassedCurrentItem && FirstItemInSlot != CurrentItem)
	{
		return FirstItemInSlot;
	}

	return nullptr;
}

AItemEquipable* AInfinityCharacter::GetPreviousItemInSlot(EItemSlot Slot, AItemEquipable* CurrentItem, bool bFallbackToLast)
{
	const bool bCurrentInSameSlot = (CurrentItem && CurrentItem->ItemSlot == Slot); // Are we transitioning from a weapon in the same slot?
	bool bHasPassedCurrentItem = false;
	AItemEquipable* LastItemInSlot = nullptr;

	// Iterate through our entire inventory backwards
	for (int i = Inventory.Num() - 1; i >= 0; --i)
	{
		auto Item = Cast<AItemEquipable>(Inventory[i]);

		// Skip null items or anything not matching our slot
		if (!Item || Item->ItemSlot != Slot || !Item->CanEquip())
		{
			continue;
		}

		// Not transitioning from an item in the same slot, the first item is fine
		if (!bCurrentInSameSlot)
		{
			return Item;
		}

		// Record the first item we encounter in this specific slot (which will be the last one in the slot)
		if (LastItemInSlot == nullptr)
		{
			LastItemInSlot = Item;
		}

		if (!bHasPassedCurrentItem)
		{
			// Flag if we've reached our CurrentItem, the next weapon in this slot is good to use
			bHasPassedCurrentItem = (CurrentItem == Item);
		}
		else
		{
			// We've passed our CurrentItem and hit a new weapon in the same slot
			return Item;
		}
	}

	// Unable to find a "previous" item, assume that we reached the end of our inventory and are looping back to the last
	if (bFallbackToLast && bHasPassedCurrentItem && LastItemInSlot != CurrentItem)
	{
		return LastItemInSlot;
	}

	return nullptr;
}

bool AInfinityCharacter::IsAtOrBeyondMaxHealth() const
{
	return Health >= MaxHealth;
}

bool AInfinityCharacter::IsAtOrBeyondMaxArmor() const
{
	return Armor >= MaxArmor;
}

void AInfinityCharacter::RestoreHealth(const float Amount) 
{
	if (HasAuthority())
	{
		Health = FMath::Min<float>(Health + Amount, MaxHealth);
	}
}

void AInfinityCharacter::AddArmor(const float Amount)
{
	if (HasAuthority())
	{
		Armor = FMath::Min<float>(Armor + Amount, MaxArmor);
	}
}

bool AInfinityCharacter::CanSlide() const
{
	return true;
}

void AInfinityCharacter::DestroyInventoryItems()
{
	if (!HasAuthority())
	{
		return;
	}

	for (int i = Inventory.Num() - 1; i >= 0; i--)
	{
		const auto Item = Inventory[i];
		if (Item)
		{
			Inventory.RemoveSingle(Item);
			Item->Destroy();
		}
	}

	Inventory.Empty();
}

void AInfinityCharacter::AddItemToInventory(TSubclassOf<AItemBase> ItemToAdd)
{
	if (!ItemToAdd)
	{
		return;
	}

	const auto Item = GetWorld()->SpawnActor<AItemBase>(ItemToAdd);
	if (Item)
	{
		Item->InitItem(this);
		Inventory.Add(Item);
	}
}

bool AInfinityCharacter::HasItemInInventory(TSubclassOf<AItemBase> ItemToFind) const
{
	for (auto& Item : Inventory)
	{
		if (!Item)
		{
			continue;
		}

		if (Item->GetClass() == ItemToFind)
		{
			return true;
		}
	}

	return false;
}

void AInfinityCharacter::EquipFirstAvailableInventoryItem()
{
	for (auto& Item : Inventory)
	{
		if (!Item)
		{
			continue;
		}

		const auto Equipable = Cast<AItemEquipable>(Item);
		if (Equipable && Equipable->CanEquip())
		{
			EquipItem(Equipable);
			return;
		}
	}
}

void AInfinityCharacter::EquipItem(AItemEquipable* Item)
{
	if (!HasAuthority())
	{
		ServerEquipItem(Item);
	}

	SetCurrentEquipable(Item);
}

void AInfinityCharacter::ServerEquipItem_Implementation(AItemEquipable* Item)
{
	EquipItem(Item);
}

bool AInfinityCharacter::ServerEquipItem_Validate(AItemEquipable* Item)
{
	return true;
}

void AInfinityCharacter::SetCurrentEquipable(AItemEquipable* Item, bool bFromReplication /*= false*/)
{
	if (CurrentEquipable)
	{
		CurrentEquipable->Unequip();
	}

	CurrentEquipable = Item;

	if (CurrentEquipable)
	{
		CurrentEquipable->Equip();
	}
}

void AInfinityCharacter::GivePowerUp(TSubclassOf<UPowerUp> PowerUpClass)
{
	if (!HasAuthority() || !PowerUpClass)
	{
		return;
	}

	const auto PowerUp = NewObject<UPowerUp>(PowerUpClass);
	if (PowerUp)
	{
		PowerUp->Activate(this);
		PowerUps.AddUnique(PowerUp);
	}
}

void AInfinityCharacter::DeactivateAnyPowerUps()
{
	if (!HasAuthority())
	{
		return;
	}

	for (int i = PowerUps.Num() - 1; i >= 0; i--)
	{
		const auto PowerUp = PowerUps[i];
		if (PowerUp)
		{
			PowerUps.RemoveSingle(PowerUp);
			PowerUp->Deactivate(this);
		}
	}

	PowerUps.Empty();
}

bool AInfinityCharacter::HasAnyActivePowerUps() const
{
	return PowerUps.Num() > 0;
}

bool AInfinityCharacter::HasSpecificPowerUp(TSubclassOf<UPowerUp> PowerUpClassToFind) const
{
	for (auto& PowerUp : PowerUps)
	{
		if (!PowerUp)
		{
			continue;
		}

		if (PowerUp->GetClass() == PowerUpClassToFind)
		{
			return true;
		}
	}

	return false;
}

void AInfinityCharacter::RemovePowerUp(UPowerUp* PowerUpToFind)
{
	if (!HasAuthority())
	{
		return;
	}

	for (int i = PowerUps.Num() - 1; i >= 0; i--)
	{
		const auto PowerUp = PowerUps[i];
		if (PowerUp && PowerUp == PowerUpToFind)
		{
			PowerUps.RemoveSingle(PowerUp);
			return;
		}
	}
}

USkeletalMeshComponent* AInfinityCharacter::GetWeaponMesh1P() const
{
	return WeaponMesh1P;
}

FVector AInfinityCharacter::GetCameraLocation() const
{
	if (CameraComponent)
	{
		return CameraComponent->GetComponentLocation();
	}

	return FVector::ZeroVector;
}

void AInfinityCharacter::SetupFirstPersonWeaponMesh(USkeletalMesh* NewFirstPersonMesh)
{
	if (WeaponMesh1P)
	{
		WeaponMesh1P->SetSkeletalMesh(NewFirstPersonMesh);
	}
}

void AInfinityCharacter::UpdateCharacterDamageMultiplier(float DamageMultipler)
{
	CharacterDamageMultiplier = DamageMultipler;
}

float AInfinityCharacter::GetDamageMultipliers() const
{
	return CharacterDamageMultiplier;
}

bool AInfinityCharacter::AllowWeaponSwapping() const
{
	return true;
}

void AInfinityCharacter::GiveAmmo(EAmmoType AmmoType, int32 AmountToGive)
{
	bool bFound = false;

	for (auto& AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		AmmoSlot.Ammo += AmountToGive;
		bFound = true;
	}

	if (!bFound)
	{
		// Couldn't find it, add it to our stored ammo.
		FStoredAmmo NewAmmo(AmmoType, AmountToGive);
		StoredAmmo.Add(NewAmmo);
	}

	const auto Weapon = Cast<AItemWeapon>(CurrentEquipable);
	if (Weapon)
	{
		Weapon->UpdateAmmo();
	}
}

void AInfinityCharacter::StoreAmmo(EAmmoType AmmoType, int32 AmountToStore)
{
	for (auto& AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		AmmoSlot.Ammo = AmountToStore;
		return;
	}

	// Couldn't find it, add it to our stored ammo.
	FStoredAmmo NewAmmo(AmmoType, AmountToStore);
	StoredAmmo.Add(NewAmmo);
}

int32 AInfinityCharacter::GetAmmoAmountForType(EAmmoType AmmoType) const
{
	int32 Amount = 0;

	for (auto& AmmoSlot : StoredAmmo)
	{
		if (AmmoSlot.AmmoType != AmmoType)
		{
			continue;
		}

		Amount = AmmoSlot.Ammo;
		break;
	}

	return Amount;
}

void AInfinityCharacter::ChangeFOV(const float NewFOV)
{
	if (CameraComponent)
	{
		CameraComponent->SetFieldOfView(NewFOV);
	}
}
