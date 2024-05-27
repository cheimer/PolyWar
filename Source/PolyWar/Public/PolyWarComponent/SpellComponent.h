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

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<class ASpell> SpellFirstClass;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellSecondClass;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<ASpell> SpellUltClass;

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* OwnerCharacter);
	TSubclassOf<ASpell> GetSpellFirstClass() const;
	TSubclassOf<ASpell> GetSpellSecondClass() const;
	TSubclassOf<ASpell> GetSpellUltClass() const;

};
