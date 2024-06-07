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
	AThrowSpell();

	virtual bool GetSpawnLocation(FVector& SpawnLocation) override;

	virtual void SetSpawnDefault() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Component")
	class UProjectileMovementComponent* SpellProjectile;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellThrowSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	FVector SpellSpawnLocation = FVector(0.0f, 0.0f, 100.0f);

	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool SpellIsPassing = true;

	UPROPERTY(EditAnywhere, Category = "Set Should", meta = (EditCondition = "SpellIsPassing == false"))
	UParticleSystem* SpellHitParticle;

	virtual void ApplyEffectOnce(class APolyWarBaseCharacter* EffectedActor) override;

};
