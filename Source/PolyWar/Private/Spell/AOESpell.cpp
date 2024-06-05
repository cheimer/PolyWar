// Fill out your copyright notice in the Description page of Project Settings.


#include "Spell/AOESpell.h"

bool AAOESpell::GetSpawnLocation(FVector& SpawnLocation)
{
	if(!GetOwner()) return false;

	FHitResult HitResult;
	if(!LineTraceSpellRange(HitResult)) return false;

	if(HitResult.bBlockingHit)
	{
		SpawnLocation = HitResult.Location;
	}
	else
	{
		SpawnLocation = HitResult.TraceEnd;
	}

	return true;
}

void AAOESpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);

}
