// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "InfinityAIController.generated.h"

/**
 * 
 */
UCLASS()
class INFINITY_API AInfinityAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	AInfinityAIController();

	// Called when the controller possess a pawn
	virtual void OnPossess(APawn* InPawn) override;

protected:

	// What behavior tree does this AI use?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AI")
	UBehaviorTree* AIBehaviorTree;

	// Initializes the AI
	virtual void InitAI();
};
