// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TeamDefinition.generated.h"

class USkeletalMesh;

/**
 * 
 */
UCLASS(BlueprintType)
class INFINITY_API UTeamDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UTeamDefinition();

	// What is the name of this team?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Team")
	FText TeamName;

	// What skins does this team use?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Team")
	TArray<TSoftObjectPtr<USkeletalMesh>> AllowedSkins;
};
