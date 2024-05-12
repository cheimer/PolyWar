#pragma once

// Maybe not use
UENUM(BlueprintType)
enum class EUnitState : uint8
{
	EUS_Stay,
	EUS_Move,
	EUS_Hold,
	EUS_Attack,
	EUS_Rush,

	EUS_MAX
};
