// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameAnnouncementComponent.generated.h"

class AInfinityCharacter;
class AInfinityPlayerState;
class AController;

UENUM(BlueprintType)
enum class EGameAnnouncement : uint8
{
	GA_DoubleKill	 UMETA(DisplayName = "Double Kill"),
	GA_TripleKill	 UMETA(DisplayName = "Triple Kill"),
	GA_QuadKill      UMETA(DisplayName = "Quad Kill"),
	GA_PentaKill     UMETA(DisplayName = "Penta Kill"),
	GA_KillingSpree  UMETA(DisplayName = "Killing Spree"),
	GA_GodLike       UMETA(DisplayName = "God Like"),
	GA_Domination    UMETA(DisplayName = "Domination")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INFINITY_API UGameAnnouncementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UGameAnnouncementComponent();

	virtual void OnCharacterKilled(AInfinityCharacter* Victim, AInfinityPlayerState* VictimPlayerState, AInfinityPlayerState* KillerPlayerState, AController* EventInstigator);

};
