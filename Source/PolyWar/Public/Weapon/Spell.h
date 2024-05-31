// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyWarTypes/SpellName.h"
#include "PolyWarTypes/SpellType.h"
#include "Spell.generated.h"

UCLASS()
class POLYWAR_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:
	ASpell();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSpellBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USphereComponent* SpellCollision;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	UParticleSystem* SpellParticle;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	ESpellName SpellName;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	ESpellType SpellType;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellDamage = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellCastingTime = -1.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellRange = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectEnemy = false;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectTeam = false;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectOwner = false;

	virtual void ApplyEffectOnce(class APolyWarBaseCharacter* EffectedActor);

private:
	TArray<TObjectPtr<AActor>> HitActors;

	void DestroySpell();

public:
	float GetSpellCastingTime() const {return SpellCastingTime;}
	float GetSpellDamage() const {return SpellDamage;}
	float GetSpellRange() const {return SpellRange;}
	ESpellType GetSpellType() const {return SpellType;}

};
