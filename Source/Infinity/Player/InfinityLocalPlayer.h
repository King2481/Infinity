// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "InfinityLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class INFINITY_API UInfinityLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:

	UInfinityLocalPlayer();

	// When called, updates the password to be provided on the next server connect 
	UFUNCTION(BlueprintCallable, Category = Password)
	void SetNextJoinPassword(const FString& Password);

	// Query which password is going to be used on the next join 
	FString GetNextJoinPassword() const;

protected:

	// Password to use on next server connect 
	UPROPERTY()
	FString PendingJoinPassword;
	
};
