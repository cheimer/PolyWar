// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spell/Spell.h"
#include "FallingSpell.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API AFallingSpell : public ASpell
{
	GENERATED_BODY()

public:
	virtual bool GetSpawnLocation(FVector& SpawnLocation) override;

	virtual void SetSpawnDefault() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSpellBeginHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SpellHitGround();

	UPROPERTY(EditAnywhere, Category = "Set Should")
	UParticleSystem* SpellHitGroundParticle;

private:
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellSpawnHeight = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellDurationAfterGround = 1.0f;

};
