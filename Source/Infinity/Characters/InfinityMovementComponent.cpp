// Made by Bruce Crum


#include "InfinityMovementComponent.h"
#include "InfinityCharacter.h"

UInfinityMovementComponent::UInfinityMovementComponent()
{
	OwningCharacter = nullptr;

	
}

bool FSavedMove_InfinityCharacter::NeedsRotationSent() const
{
	return bPressedSlide;
}

void FSavedMove_InfinityCharacter::Clear()
{
	Super::Clear();

	bSavedWantsSlide = false;
	bPressedSlide = false;
}

void FSavedMove_InfinityCharacter::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);
	UInfinityMovementComponent* MovementComponent = Cast<UInfinityMovementComponent>(Character->GetCharacterMovement());
	if (MovementComponent)
	{
		/*bSavedWantsSlide = MovementComponent->bWantsFloorSlide;
		SavedFloorSlideEndTime = MovementComponent->FloorSlideEndTime;
		bPressedSlide = MovementComponent->bPressedSlide;*/
	}

	// Round acceleration, so sent version and locally used version always match
	Acceleration.X = FMath::RoundToFloat(Acceleration.X);
	Acceleration.Y = FMath::RoundToFloat(Acceleration.Y);
	Acceleration.Z = FMath::RoundToFloat(Acceleration.Z);
}

uint8 FSavedMove_InfinityCharacter::GetCompressedFlags() const
{
	uint8 Result = 0;

	if (bPressedJump)
	{
		Result |= 1;
	}

	if (bWantsToCrouch)
	{
		Result |= FLAG_WantsToCrouch;
	}

	if (bPressedSlide)
	{
		Result |= (7 << 2);
	}

	if (bSavedWantsSlide)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

bool FSavedMove_InfinityCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	if (bSavedWantsSlide != ((FSavedMove_InfinityCharacter*)&NewMove)->bSavedWantsSlide)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}

bool FSavedMove_InfinityCharacter::IsImportantMove(const FSavedMovePtr& ComparedMove) const
{
	if (IsCriticalMove(ComparedMove))
	{
		return true;
	}

	if (!ComparedMove.IsValid())
	{
		// if no previous move to compare, always send impulses
		return bPressedJump;
	}

	// Check if any important movement flags have changed status.
	if (bWantsToCrouch != ComparedMove->bWantsToCrouch)
	{
		return true;
	}

	if (StartPackedMovementMode != ComparedMove->StartPackedMovementMode)
	{
		return true;
	}

	// check if acceleration has changed significantly
	if (Acceleration != ComparedMove->Acceleration)
	{
		// Compare magnitude and orientation
		if ((FMath::Abs(AccelMag - ComparedMove->AccelMag) > AccelMagThreshold) || ((AccelNormal | ComparedMove->AccelNormal) < AccelDotThreshold))
		{
			return true;
		}
	}
	return false;
}

bool FSavedMove_InfinityCharacter::IsCriticalMove(const FSavedMovePtr& ComparedMove) const
{
	if (bPressedSlide)
	{
		return true;
	}

	if (ComparedMove.IsValid() && (bPressedJump && (bPressedJump != ComparedMove->bPressedJump)))
	{
		return true;
	}

	return false;
}

void FSavedMove_InfinityCharacter::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UInfinityMovementComponent* MovementComponent = Cast<UInfinityMovementComponent>(Character->GetCharacterMovement());
	if (MovementComponent)
	{
		/*if (MovementComponent->bIsSettingUpFirstReplayMove)
		{
			// MovementComponent->FloorSlideEndTime = SavedFloorSlideEndTime;
		}
		else
		{
			// SavedFloorSlideEndTime = MovementComponent->FloorSlideEndTime;
		}*/
	}
}

void FSavedMove_InfinityCharacter::PostUpdate(ACharacter* Character, FSavedMove_Character::EPostUpdateMode PostUpdateMode)
{
	Super::PostUpdate(Character, PostUpdateMode);
}

FSavedMovePtr FNetworkPredictionData_Client_InfinityCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_InfinityCharacter());
}