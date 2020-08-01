// Made by Bruce Crum


#include "TeamInfo.h"
#include "TeamInterface.h"
#include "Net/UnrealNetwork.h"

ATeamInfo::ATeamInfo()
{
	bReplicates = true;
	TeamId = ITeamInterface::InvalidId;
}

void ATeamInfo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATeamInfo, TeamId);
}

void ATeamInfo::InitializeTeam(UTeamDefinition* NewTeamDefition, uint8 NewTeamId)
{
	TeamId = NewTeamId;
}

void ATeamInfo::OnRep_TeamId()
{

}

uint8 ATeamInfo::GetTeamId() const
{
	return TeamId;
}