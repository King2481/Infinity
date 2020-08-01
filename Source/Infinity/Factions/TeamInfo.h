// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "TeamInfo.generated.h"

class UTeamDefinition;

/**
 * 
 */
UCLASS()
class INFINITY_API ATeamInfo : public AInfo, public ITeamInterface
{
	GENERATED_BODY()

public:

	ATeamInfo();

	void InitializeTeam(UTeamDefinition* NewTeamDefition, uint8 NewTeamId);

	virtual uint8 GetTeamId() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	// What team does this character belong to?
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TeamId, BlueprintReadOnly, Category = "Team")
	uint8 TeamId;

	UFUNCTION()
	void OnRep_TeamId();
	
};
