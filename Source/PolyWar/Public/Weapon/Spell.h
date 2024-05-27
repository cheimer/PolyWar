// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyWarTypes/SpellName.h"
#include "Spell.generated.h"

UCLASS()
class POLYWAR_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:
	ASpell();

	ESpellName aa;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSpellBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USphereComponent* SpellCollision;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	UParticleSystem* SpellParticle;

	ESpellName CurrentSpellName = ESpellName::ESN_MAX;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponDamage = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackDelay = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackRange = 50.0f;

private:
	TArray<TObjectPtr<AActor>> HitActors;

};
