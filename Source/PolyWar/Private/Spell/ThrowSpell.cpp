// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/ThrowSpell.h"

bool AThrowSpell::GetSpawnLocation(FVector& SpawnLocation)
{
	return Super::GetSpawnLocation(SpawnLocation);
}

void AThrowSpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);
}
