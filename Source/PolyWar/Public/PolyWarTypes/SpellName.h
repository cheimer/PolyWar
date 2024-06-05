#pragma once

UENUM(BlueprintType)
enum class ESpellName : uint8
{
    // AttackSpell
    ESN_Fireball,
    ESN_Flare,

    // BuffSpell
    ESN_SpeedUp,

	// UltSpell
	ESN_Meteor,

	ESN_MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(ESpellName, ESpellName::ESN_MAX);