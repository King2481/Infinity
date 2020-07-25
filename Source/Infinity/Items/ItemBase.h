// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class AInfinityCharacter;

UCLASS()
class INFINITY_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:

	// Sets default values for this actor's properties
	AItemBase();

	// Initilizes the item.
	virtual void InitItem(AInfinityCharacter* NewOwner);

	// Sets the pawn owner.
	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetPawnOwner(AInfinityCharacter* NewOwner);

	// Returns the pawn owner of this item.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	AInfinityCharacter* GetPawnOwner() const;

	// Checks to see if the pawn owner is locally controlled.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Item")
	bool IsPawnOwnerLocallyControlled() const;

	// Variable replication setup.
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// What is the localized name of this item?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMesh* Mesh1PAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMesh* Mesh3PAsset;

	USkeletalMeshComponent* GetMesh3P() const;

protected:

	// Who "owns" this item?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_PawnOwner, BlueprintReadOnly, Category = "Item")
	AInfinityCharacter* PawnOwner;

	UFUNCTION()
	void OnRep_PawnOwner();

	// Item mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh3P;

	virtual void OnConstruction(const FTransform& Transform) override;

};
