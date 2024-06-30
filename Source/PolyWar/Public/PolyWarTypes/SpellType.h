#pragma once

#include "SpellType.generated.h"

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

USTRUCT(BlueprintType)
struct POLYWAR_API FSpellTypeSpec
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Damage = 0.0f;
	UPROPERTY(EditAnywhere)
	float Duration = 0.0f;
	UPROPERTY(EditAnywhere)
	float Rate = 1.0f;

};