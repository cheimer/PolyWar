// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BuffSpell.h"

void ABuffSpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);

	if(SpellName == ESpellName::ESN_SpeedUp)
	{
		// SpeedUp
	}

}
