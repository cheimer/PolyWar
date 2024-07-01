#pragma once

UENUM(BlueprintType)
enum class EWeaponSkill : uint8
{
	EWS_SpearSlash,
	EWS_SpearThrow,
	EWS_SwordTakeDown,
	EWS_SwordBlock,

	EWS_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EWeaponSkill, EWeaponSkill::EWS_MAX);