#pragma once

UENUM(BlueprintType)
enum class EUnitNum : uint8
{
	EUN_Unit1,
	EUN_Unit2,
	EUN_Unit3,
	EUN_Unit4,
	EUN_Unit5,
	EUN_Unit6,
	EUN_Unit7,
	EUN_Unit8,

	EUN_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EUnitNum, EUnitNum::EUN_MAX);