#pragma once

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	EOD_Move,
	EOD_Attack,
	EOD_Hold,
	EOD_Rush,
	EOD_Stop,
	EOD_Cancel,

	EOD_MAX UMETA(Hidden)
};
