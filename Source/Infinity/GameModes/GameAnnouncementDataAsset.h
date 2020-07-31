// Made by Bruce Crum

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameAnnouncementDataAsset.generated.h"

UENUM(BlueprintType)
enum class EGameAnnouncement : uint8
{
	GA_None	         UMETA(DisplayName = "None"),
	GA_DoubleKill	 UMETA(DisplayName = "Double Kill"),
	GA_TripleKill	 UMETA(DisplayName = "Triple Kill"),
	GA_QuadKill      UMETA(DisplayName = "Quad Kill"),
	GA_PentaKill     UMETA(DisplayName = "Penta Kill"),
	GA_KillingSpree  UMETA(DisplayName = "Killing Spree"),
	GA_GodLike       UMETA(DisplayName = "God Like"),
	GA_Domination    UMETA(DisplayName = "Domination")
};


/**
 * 
 */
UCLASS(BlueprintType)
class INFINITY_API UGameAnnouncementDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UGameAnnouncementDataAsset();

	UPROPERTY(EditDefaultsOnly, Category = "Game Announcements")
	TMap<EGameAnnouncement, USoundBase*> AnnouncementToSoundMap;
	
};
