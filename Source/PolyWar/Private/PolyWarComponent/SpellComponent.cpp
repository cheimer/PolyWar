// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/SpellComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Weapon/Spell.h"

USpellComponent::USpellComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USpellComponent::BeginPlay()
{
	Super::BeginPlay();

}

/*
 * Get, Set Func
 */
void USpellComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;
	OwnerCharacter = InOwnerCharacter;
}

TSubclassOf<ASpell> USpellComponent::GetSpellFirstClass() const
{
	return SpellFirstClass;
}

TSubclassOf<ASpell> USpellComponent::GetSpellSecondClass() const
{
	return SpellSecondClass;
}

TSubclassOf<ASpell> USpellComponent::GetSpellUltClass() const
{
	return SpellUltClass;
}
