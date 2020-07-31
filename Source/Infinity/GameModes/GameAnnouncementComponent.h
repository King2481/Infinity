// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameAnnouncementDataAsset.h"
#include "GameAnnouncementComponent.generated.h"

class AInfinityCharacter;
class AInfinityPlayerState;
class AController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Config = Game)
class INFINITY_API UGameAnnouncementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UGameAnnouncementComponent();

	virtual void PostInitProperties() override;

	virtual void OnCharacterKilled(AInfinityCharacter* Victim, AInfinityPlayerState* VictimPlayerState, AInfinityPlayerState* KillerPlayerState, AController* EventInstigator);

protected:

	UPROPERTY()
	TMap<EGameAnnouncement, USoundBase*> AnnouncementToSoundMap;

	UPROPERTY(Config)
	TArray<TSoftObjectPtr<UGameAnnouncementDataAsset>> AnnouncementDataAssets;


};
