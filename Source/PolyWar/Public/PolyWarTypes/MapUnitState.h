#pragma once

UENUM(BlueprintType)
enum class EMapUnitState : uint8
{
	EMUS_UnClicked,
	EMUS_Clicked,
	EMUS_Removed,

	EMUS_MAX
};
