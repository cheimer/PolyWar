// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell/Spell.h"
#include "ThrowSpell.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API AThrowSpell : public ASpell
{
	GENERATED_BODY()

public:
	virtual bool GetSpawnLocation(FVector& SpawnLocation) override;

protected:
	virtual void ApplyEffectOnce(class APolyWarBaseCharacter* EffectedActor) override;

};
