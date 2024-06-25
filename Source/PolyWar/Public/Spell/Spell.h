// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyWarTypes/SpellType.h"
#include "Spell.generated.h"

UCLASS()
class POLYWAR_API ASpell : public AActor
{
	GENERATED_BODY()
	
public:
	ASpell();

	virtual bool GetSpawnLocation(FVector& SpawnLocation);

	virtual void SetSpawnDefault() {}

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
	ESpellType SpellType = ESpellType::EST_None;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellDamage = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellCastingTime = -1.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellRange = 500.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellDuration = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpellCoolDown = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectEnemy = false;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectTeam = false;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	bool bEffectOwner = false;

	virtual void ApplyEffectOnce(class APolyWarBaseCharacter* EffectedActor);

	virtual void ApplySpellType(AActor* EffectedActor, AActor* DamageCauser);

	bool LineTraceSpellRange(FHitResult& HitResult);

	TObjectPtr<class UParticleSystemComponent> CurrentParticle;

	void DestroySpell();

	UFUNCTION()
	void FinishParticle(UParticleSystemComponent* PSystem);

private:
	TArray<TObjectPtr<AActor>> HitActors;

public:
	float GetSpellCastingTime() const {return SpellCastingTime;}
	float GetSpellDamage() const {return SpellDamage;}
	ESpellType GetSpellType() const {return SpellType;}
	float GetSpellCoolDown() const {return SpellCoolDown;}

};
