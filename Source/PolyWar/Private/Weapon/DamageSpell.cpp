// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/DamageSpell.h"

#include "Kismet/GameplayStatics.h"
#include "Character/PolyWarBaseCharacter.h"

void ADamageSpell::ApplyEffectOnce(APolyWarBaseCharacter* EffectedActor)
{
	Super::ApplyEffectOnce(EffectedActor);

	if(GetOwner()->GetInstigatorController())
	{
		UGameplayStatics::ApplyDamage(EffectedActor, SpellDamage,
			GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
	}
}
