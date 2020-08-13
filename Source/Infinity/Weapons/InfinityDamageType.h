// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "InfinityDamageType.generated.h"

/**
 * 
 */
UCLASS()
class INFINITY_API UInfinityDamageType : public UDamageType
{
	GENERATED_BODY()

public:

	UInfinityDamageType();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float Magnitude;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	float RagdollLaunchMagnitude;
	
};

UCLASS()
class INFINITY_API UDamageTypeBullet : public UInfinityDamageType
{
	GENERATED_BODY()

public:

	UDamageTypeBullet();

};

UCLASS()
class INFINITY_API UDamageTypePellet : public UDamageTypeBullet
{
	GENERATED_BODY()

public:

	UDamageTypePellet();

};

UCLASS()
class INFINITY_API UDamageTypeImpact : public UInfinityDamageType
{
	GENERATED_BODY()

public:

	UDamageTypeImpact();

};

UCLASS()
class INFINITY_API UDamageTypeExplosive : public UInfinityDamageType
{
	GENERATED_BODY()

public:

	UDamageTypeExplosive();
};
