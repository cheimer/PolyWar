#pragma once

UENUM(BlueprintType)
enum class ESpellName : uint8
{
    // AttackSpell
    ESN_Meteor,

    // BuffSpell
    ESN_SpeedUp,

	// UltSpell
	ESN_UltMeteor,

	ESN_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(ESpellName, ESpellName::ESN_MAX);