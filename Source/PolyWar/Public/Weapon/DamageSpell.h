// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Spell.h"
#include "DamageSpell.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API ADamageSpell : public ASpell
{
	GENERATED_BODY()

protected:
	virtual void ApplyEffectOnce(class APolyWarBaseCharacter* EffectedActor) override;

};
