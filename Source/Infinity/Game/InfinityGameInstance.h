// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InfinityGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class INFINITY_API UInfinityGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UInfinityGameInstance();

	virtual AGameModeBase* CreateGameModeForURL(FURL InURL, UWorld* InWorld) override;

};
