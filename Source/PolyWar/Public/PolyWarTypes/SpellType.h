#pragma once

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	EST_None,
	EST_Fire,
	EST_Ice,
	EST_SpeedBuf,
	EST_PowerBuf,
	EST_SpellBuf,

	ET_MAX UMETA(Hidden)
};
