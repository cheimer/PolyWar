#pragma once

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	ET_RedTeam,
	ET_BlueTeam,
	ET_NoTeam,

	ET_MAX UMETA(Hidden)
};
