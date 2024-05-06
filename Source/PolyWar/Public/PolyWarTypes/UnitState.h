#pragma once

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	EUS_UnClicked,
	EUS_Clicked,
	EUS_Removed,

	EUS_MAX
};
