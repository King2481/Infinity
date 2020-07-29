// Made by Bruce Crum


#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionComp = CreateOptionalDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetGenerateOverlapEvents(false);

	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	ProjectileMovement->InitialSpeed = 50000.f;
	ProjectileMovement->MaxSpeed = 50000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	InitialVelocity = FVector::ZeroVector;

	DirectDamage = 10.f;
	bDestroyOnHit = true;
	DirectDamageTypeClass = nullptr;
	MaxAmountOfBounces = 3;
	CurrentAmountOfBounces = 0;
	SurfaceReaction = nullptr;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Projectiles should be removed from the world if it hasn't hit anything in 8 seconds.
	SetLifeSpan(8.f);
}

void AProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CollisionComp->IgnoreActorWhenMoving(this, true);

	if (GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
	}

	if (CollisionComp)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	}
}

void AProjectileBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AProjectileBase, InitialVelocity, COND_InitialOnly);
}

void AProjectileBase::InitVelocity(const FVector& Direction)
{
	if (ProjectileMovement)
	{
		InitialVelocity = Direction * ProjectileMovement->InitialSpeed;
	}
}

void AProjectileBase::OnRep_InitialVelocity()
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = InitialVelocity;
	}
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	HandleImpact(Hit);
}

void AProjectileBase::HandleImpact(const FHitResult& Impact)
{
	if (HasAuthority())
	{
		MulticastHit();

		if (Impact.GetActor())
		{
			UGameplayStatics::ApplyDamage(Impact.GetActor(), DirectDamage, GetInstigatorController(), this, DirectDamageTypeClass);
		}

		if (bDestroyOnHit)
		{
			Destroy();
		}
		else if (ProjectileMovement && ProjectileMovement->bShouldBounce && MaxAmountOfBounces > 0)
		{
			++CurrentAmountOfBounces;
			if (CurrentAmountOfBounces >= MaxAmountOfBounces)
			{
				Destroy();
			}
		}
	}
}

void AProjectileBase::MulticastHit_Implementation()
{

}

