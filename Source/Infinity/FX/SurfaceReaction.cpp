// Made by Bruce Crum


#include "SurfaceReaction.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

USurfaceReaction::USurfaceReaction()
{

}

FSurfaceReactionInfo USurfaceReaction::GetSurfaceReactionFromHit(TWeakObjectPtr<UPhysicalMaterial> PhysMaterial)
{
	if (PhysMaterial.IsValid())
	{
		FSurfaceReactionInfo* Info = SurfaceTypeReactions.Find(PhysMaterial->SurfaceType);
		if (Info)
		{
			return *Info;
		}
	}

	return DefaultReaction;;
}