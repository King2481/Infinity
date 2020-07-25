// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InfinityMovementComponent.generated.h"

class AInfinityCharacter;

/**
 * 
 */
UCLASS()
class INFINITY_API UInfinityMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UInfinityMovementComponent();

	UPROPERTY()
	AInfinityCharacter* OwningCharacter;

	
};

// Networking support
class INFINITY_API FSavedMove_InfinityCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	FSavedMove_InfinityCharacter()
	{
	
	}

	// Flags used to synchronize dodging in networking (analoguous to bPressedJump)
	bool bSavedWantsSlide;
	bool bPressedSlide;

	// local only properties (not replicated) used when replaying moves
	float SavedFloorSlideEndTime;

	// return true if rotation affects this moves implementation
	virtual bool NeedsRotationSent() const; 

	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override; 
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;
	virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const override;
	virtual bool IsCriticalMove(const FSavedMovePtr& LastAckedMove) const;
	virtual void PostUpdate(class ACharacter* C, EPostUpdateMode PostUpdateMode) override;
	virtual void PrepMoveFor(class ACharacter* C) override;
};

class INFINITY_API FNetworkPredictionData_Client_InfinityCharacter : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_InfinityCharacter(const UCharacterMovementComponent& ClientMovement) : FNetworkPredictionData_Client_Character(ClientMovement) {}

	/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
	virtual FSavedMovePtr AllocateNewMove() override;
};
