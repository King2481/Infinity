// Made by Bruce Crum


#include "InfinityAIController.h"

AInfinityAIController::AInfinityAIController()
{
	AIBehaviorTree = nullptr;
}

void AInfinityAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitAI();
}

void AInfinityAIController::InitAI()
{
	if (AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
	}
}