#pragma once

UENUM(BlueprintType)
enum class ESpellType : uint8
{
    EST_TargetAOE,
	EST_MyAOE,
	EST_TargetOverhead,

	EST_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(ESpellType, ESpellType::EST_MAX);