// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class USurfaceReaction;
class UPhysicalMaterial;

UCLASS()
class INFINITY_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AProjectileBase();

	// Called after all components have been initialized.
	virtual void PostInitializeComponents() override;

	// Intializes the velocity of the weapon.
	void InitVelocity(const FVector& Direction);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/* Collision shape component */
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(BlueprintReadOnly, Replicated, ReplicatedUsing = OnRep_InitialVelocity, Category = "Movement")
	FVector InitialVelocity;

	UFUNCTION()
	void OnRep_InitialVelocity();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	float DirectDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	bool bDestroyOnHit;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<UDamageType> DirectDamageTypeClass;

	// The maximum amount of bounces this projectile is allowed to have before destroying itself (assuming bDestroyOnHit is false), if 0, this means for an unlimited amount of bounces.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	uint8 MaxAmountOfBounces;

	// The current number of bounces this projectile has had throughout it's life.
	UPROPERTY(BlueprintReadOnly, Category = "Projectile")
	uint8 CurrentAmountOfBounces;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHit(const FHitResult& Hit);
	void MulticastHit_Implementation(const FHitResult& Hit);

	virtual void HandleImpact(const FHitResult& Impact);

	// What surface reaction does this weapon uses when it hits something.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<USurfaceReaction> SurfaceReaction;

};
