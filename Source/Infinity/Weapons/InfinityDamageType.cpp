// Made by Bruce Crum


#include "InfinityDamageType.h"

UInfinityDamageType::UInfinityDamageType()
{
	Magnitude = 1.f;
}

UDamageTypeBullet::UDamageTypeBullet()
{
	Magnitude = 0.5f;
}

UDamageTypePellet::UDamageTypePellet()
{
	Magnitude = 100.f;
}

UDamageTypeImpact::UDamageTypeImpact()
{
	Magnitude = 0.1f;
}

UDamageTypeExplosive::UDamageTypeExplosive()
{
	Magnitude = 1000.f;
}