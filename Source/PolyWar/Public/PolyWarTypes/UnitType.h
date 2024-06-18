#pragma once

UENUM(BlueprintType)
enum class EUnitType : uint8
{
	EU_Spear,
	EU_Sword,
	EU_Shield,
	EU_Arrow,

	EU_MAX UMETA(Hidden)
};

inline FText GetTextFromUnitType(EUnitType UnitType)
{
	switch(UnitType)
	{
	case EUnitType::EU_Spear:
		return FText::FromString("Spear");
	case EUnitType::EU_Sword:
		return FText::FromString("Sword");
	case EUnitType::EU_Shield:
		return FText::FromString("Shield");
	case EUnitType::EU_Arrow:
		return FText::FromString("Arrow");
	case EUnitType::EU_MAX:
		return FText::FromString("MAX");
	}

	return FText();
}
