#pragma once

UENUM(BlueprintType)
enum class ESpellType : uint8
{
	EST_None,
	EST_Fire,
	EST_Ice,

	ET_MAX UMETA(Hidden)
};
