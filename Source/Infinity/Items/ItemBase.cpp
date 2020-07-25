// Made by Bruce Crum


#include "ItemBase.h"
#include "Net/UnrealNetwork.h"
#include "Infinity/Characters/InfinityCharacter.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AItemBase::AItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh3P"));
	Mesh3P->SetupAttachment(RootComponent);
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->CastShadow = true;
	Mesh3P->bOnlyOwnerSee = false;
	Mesh3P->bOwnerNoSee = true;

	ItemName = FText::GetEmpty();

	Mesh1PAsset = nullptr;
	Mesh3PAsset = nullptr;
}

void AItemBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (Mesh3P)
	{
		Mesh3P->SetSkeletalMesh(Mesh3PAsset);
	}
}

void AItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemBase, PawnOwner);
}

void AItemBase::InitItem(AInfinityCharacter* NewOwner)
{
	SetPawnOwner(NewOwner);
}

void AItemBase::SetPawnOwner(AInfinityCharacter* NewOwner)
{
	PawnOwner = NewOwner;
	SetOwner(NewOwner);
}

AInfinityCharacter* AItemBase::GetPawnOwner() const
{
	return PawnOwner;
}

void AItemBase::OnRep_PawnOwner()
{

}

bool AItemBase::IsPawnOwnerLocallyControlled() const
{
	return PawnOwner && PawnOwner->IsLocallyControlled();
}

USkeletalMeshComponent* AItemBase::GetMesh3P() const
{
	return Mesh3P;
}


