// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PolyWarTypes/SpellName.h"
#include "SpellComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API USpellComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpellComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SpellStart(TSubclassOf<class ASpell> Spell);
	void SpellEffect();
	void SpellEnd();

protected:
	virtual void BeginPlay() override;
private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;

	TObjectPtr<ASpell> CurrentSpell;
	TObjectPtr<UAnimMontage> CurrentAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellFirstClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellFirstAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellSecondClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellSecondAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellUltClass;
	UPROPERTY(EditAnywhere, Category = "Settable")
	UAnimMontage* SpellUltAnimMontage;

	bool IsValidSpell(TSubclassOf<ASpell> Spell);
	void SpellCastingEnd();

	UFUNCTION(Server, Reliable)
	void ServerFinishSpawning(const FTransform& SpawnTransform);

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* OwnerCharacter);
	TSubclassOf<ASpell> GetSpellFirstClass() const;
	TSubclassOf<ASpell> GetSpellSecondClass() const;
	TSubclassOf<ASpell> GetSpellUltClass() const;

	UAnimMontage* GetSpellAnimMontage(TSubclassOf<ASpell> Spell);

};
