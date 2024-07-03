// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API USpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellComponent();

	void SpellStart(TSubclassOf<class ASpell> Spell);
	void SpellEffect();
	void SpellEnd();

	bool IsSpellAble(TSubclassOf<ASpell> Spell);

	void SetSpellCoolDown(TSubclassOf<ASpell> Spell, float CoolDown);
	float GetSpellCoolDown(TSubclassOf<ASpell> Spell);
	float GetSpellRemainCoolDown(TSubclassOf<ASpell> Spell);

private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;

	TObjectPtr<ASpell> CurrentSpell;
	TObjectPtr<UAnimMontage> CurrentAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellFirstClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellFirstAnimMontage;
	float SpellFirstCoolDown = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellSecondClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellSecondAnimMontage;
	float SpellSecondCoolDown = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellUltClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellUltAnimMontage;
	float SpellUltCoolDown = 0.0f;

	bool IsValidSpell(TSubclassOf<ASpell> Spell);
	void SpellCastingEnd();

	UFUNCTION(Server, Reliable)
	void ServerSpellFinishSpawning(const FTransform& SpawnTransform);

	UFUNCTION(Server, Reliable)
	void ServerThrowSpellFinishSpawning(const FTransform& SpawnTransform, const FVector& Direction);

	FVector GetThrowSpellDirection(const FVector& SpellLocation);

	FTimerHandle SpellFirstCoolTimer;
	FTimerHandle SpellSecondCoolTimer;
	FTimerHandle SpellUltCoolTimer;

	bool bSpellFirstAble = true;
	bool bSpellSecondAble = true;
	bool bSpellUltAble = true;

	void SpellCoolDownStart(TSubclassOf<ASpell> Spell);
	void SpellFirstReady();
	void SpellSecondReady();
	void SpellUltReady();

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* OwnerCharacter);
	TSubclassOf<ASpell> GetSpellFirstClass() const;
	TSubclassOf<ASpell> GetSpellSecondClass() const;
	TSubclassOf<ASpell> GetSpellUltClass() const;

	UAnimMontage* GetSpellAnimMontage(TSubclassOf<ASpell> Spell);

};
