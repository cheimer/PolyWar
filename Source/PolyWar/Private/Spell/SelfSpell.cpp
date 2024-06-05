// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/SelfSpell.h"

bool ASelfSpell::GetSpawnLocation(FVector& SpawnLocation)
{
	if(!GetOwner()) return false;

	SpawnLocation = GetOwner()->GetActorLocation();
	return true;
}

void ASelfSpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);
}
