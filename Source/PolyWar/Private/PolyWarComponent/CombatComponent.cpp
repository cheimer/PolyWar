// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/CombatComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PolyWarComponent/SpellComponent.h"
#include "Spell/Spell.h"
#include "Weapon/ThrowableWeapon.h"
#include "Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, CurrentAnimIndex)
	DOREPLIFETIME(UCombatComponent, CurrentWeaponSkill)
	DOREPLIFETIME(UCombatComponent, CurrentSpell);
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::BeginWeaponAttack()
{
	if(!OwnerCharacter || !OwnerCharacter->GetEquippedWeapon() || CombatState != ECombatState::ECS_Wait || bIsAttackDelay) return;

	SetCurrentAnimIndexRand();

	CombatState = ECombatState::ECS_WeaponAttack;

	if(OwnerCharacter->HasAuthority())
	{
		WeaponAttack(CurrentAnimIndex);
	}
	else if(!OwnerCharacter->HasAuthority())
	{
		ServerWeaponAttack(CurrentAnimIndex);
	}

}

void UCombatComponent::BeginWeaponSkill(EWeaponSkill WeaponSkill)
{
	if(!OwnerCharacter || !EquippedWeapon || CombatState != ECombatState::ECS_Wait) return;
	if(!EquippedWeapon->GetWeaponSkillAble(WeaponSkill)) return;

	CurrentWeaponSkill = WeaponSkill;
	CombatState = ECombatState::ECS_WeaponSkill;

	if(OwnerCharacter->HasAuthority())
	{
		WeaponSkillAttack(WeaponSkill);
	}
	if(!OwnerCharacter->HasAuthority())
	{
		ServerWeaponSkillAttack(WeaponSkill);
	}
}

void UCombatComponent::BeginSpell(TSubclassOf<ASpell> Spell)
{
	if(!OwnerCharacter || !SpellComponent || CombatState != ECombatState::ECS_Wait || !Spell) return;
	if(!SpellComponent->IsSpellAble(Spell)) return;

	CurrentSpell = Spell;
	CombatState = ECombatState::ECS_SpellCast;

	if(OwnerCharacter->HasAuthority())
	{
		SpellCast(Spell);
	}
	if(!OwnerCharacter->HasAuthority())
	{
		ServerSpellCast(Spell);
	}
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_WeaponAttack:
		WeaponAttack(CurrentAnimIndex);
		break;
	case ECombatState::ECS_WeaponSkill:
		WeaponSkillAttack(CurrentWeaponSkill);
		break;
	case ECombatState::ECS_SpellCast:
		SpellCast(CurrentSpell);
		break;
	}
}

void UCombatComponent::WeaponAttack(int32 AnimIndex)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayAttackAnimMontage(false, AnimIndex);

	bIsAttackDelay = true;

	FTimerHandle AttackTimer;
	float Delay = EquippedWeapon->GetAttackDelay() > OwnerCharacter->GetAttackAnimMontageLen(AnimIndex) ?
		EquippedWeapon->GetAttackDelay() : OwnerCharacter->GetAttackAnimMontageLen(AnimIndex);
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::WeaponAttackEnd, Delay * 0.9f);

}

void UCombatComponent::ServerWeaponAttack_Implementation(int32 AnimIndex)
{
	MulticastWeaponAttack(AnimIndex);
}

void UCombatComponent::MulticastWeaponAttack_Implementation(int32 AnimIndex)
{
	WeaponAttack(AnimIndex);
}

void UCombatComponent::WeaponSkillAttack(EWeaponSkill WeaponSkill)
{
	if(!OwnerCharacter || !EquippedWeapon) return;

	if(EquippedWeapon->SkillFocusOnScreen(WeaponSkill))
	{
		APolyWarPlayerCharacter* OwnerPlayerCharacter = Cast<APolyWarPlayerCharacter>(OwnerCharacter);
		if(OwnerPlayerCharacter)
		{
			OwnerPlayerCharacter->SetIsFocusOnScreen(true);
		}
	}

	CurrentWeaponSkill = WeaponSkill;
	EquippedWeapon->SetWeaponSkill(WeaponSkill);
	EquippedWeapon->WeaponSkillStart(WeaponSkill);
	OwnerCharacter->PlayWeaponSkillAnimMontage(WeaponSkill);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		AttackTimer, this, &ThisClass::WeaponSkillEnd, OwnerCharacter->GetWeaponSkillAnimPlayLen(WeaponSkill) * 0.9f);
}

void UCombatComponent::ServerWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	MulticastWeaponSkillAttack(WeaponSkill);
}

void UCombatComponent::MulticastWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	WeaponSkillAttack(WeaponSkill);
}

void UCombatComponent::SpellCast(TSubclassOf<ASpell> Spell)
{
	if(!OwnerCharacter || !SpellComponent || !Spell) return;

	APolyWarPlayerCharacter* OwnerPlayerCharacter = Cast<APolyWarPlayerCharacter>(OwnerCharacter);
	if(OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->SetIsFocusOnScreen(true);
	}

	CurrentSpell = Spell;

	UAnimMontage* SpellAnimMontage = SpellComponent->GetSpellAnimMontage(Spell);
	if(SpellAnimMontage)
	{
		OwnerCharacter->PlaySpellAnimMontage(SpellAnimMontage);
	}

	SpellComponent->SpellStart(Spell);
}

void UCombatComponent::ServerSpellCast_Implementation(TSubclassOf<ASpell> Spell)
{
	MulticastSpellCast(Spell);
}

void UCombatComponent::MulticastSpellCast_Implementation(TSubclassOf<ASpell> Spell)
{
	SpellCast(Spell);
}

void UCombatComponent::WeaponAttackEnd()
{
	CombatState = ECombatState::ECS_Wait;

	// Wait for Network delay. if don't use this and server WeaponAttack continuously, client can't receive On_RepCombatState.
	if(bIsAttackDelay)
	{
		FTimerHandle LittleTimer;
		OwnerCharacter->GetWorldTimerManager().SetTimer(LittleTimer, this, &ThisClass::AttackDelayEnd, 0.1f);
	}

}

void UCombatComponent::AttackDelayEnd()
{
	bIsAttackDelay = false;
}

void UCombatComponent::WeaponSkillEnd()
{
	if(OwnerCharacter && OwnerCharacter->GetEquippedWeapon())
	{
		OwnerCharacter->GetEquippedWeapon()->SetWeaponSkill(EWeaponSkill::EWS_MAX);
	}

	CurrentWeaponSkill = EWeaponSkill::EWS_MAX;
	CombatState = ECombatState::ECS_Wait;
}

void UCombatComponent::SpellCastEnd()
{
	CombatState = ECombatState::ECS_Wait;
}

void UCombatComponent::WeaponAttackCheckStart()
{
	if(!OwnerCharacter || !EquippedWeapon) return;

	EquippedWeapon->SetCollisionEnabled(true);

}

void UCombatComponent::WeaponAttackCheckEnd()
{
	if(!OwnerCharacter || !EquippedWeapon) return;

	EquippedWeapon->SetCollisionEnabled(false);
}

void UCombatComponent::ThrowWeapon(AWeapon* Weapon, const FVector& Direction)
{
	if(!Weapon || !OwnerCharacter || !OwnerCharacter->HasAuthority()) return;

	APolyWarPlayerCharacter* OwnerPlayerCharacter = Cast<APolyWarPlayerCharacter>(OwnerCharacter);
	if(OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->SetIsFocusOnScreen(false);
	}

	AThrowableWeapon* ThrowableWeapon = Cast<AThrowableWeapon>(Weapon);
	if(!ThrowableWeapon) return;

	ThrowableWeapon->SetCollisionEnabled(true);
	ThrowableWeapon->SetWeaponSkill(CurrentWeaponSkill);
	ThrowableWeapon->SetProjectile();
	ThrowableWeapon->ThrowWeaponStart(Weapon->GetActorLocation(), Direction);

}

void UCombatComponent::SpellEffect()
{
	if(!SpellComponent) return;

	APolyWarPlayerCharacter* OwnerPlayerCharacter = Cast<APolyWarPlayerCharacter>(OwnerCharacter);
	if(OwnerPlayerCharacter)
	{
		OwnerPlayerCharacter->SetIsFocusOnScreen(false);
	}

	SpellComponent->SpellEffect();
}

void UCombatComponent::SetCurrentAnimIndexRand()
{
	if(!OwnerCharacter || OwnerCharacter->GetAttackAnimMontagesLen() == 0) return;

	CurrentAnimIndex = FMath::RandRange(0, OwnerCharacter->GetAttackAnimMontagesLen() - 1);
}

/*
 * Get, Set Func
 */
void UCombatComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;

	OwnerCharacter = InOwnerCharacter;
}

void UCombatComponent::SetSpellComponent(USpellComponent* InSpellComponent)
{
	if(!InSpellComponent) return;

	SpellComponent = InSpellComponent;
}

void UCombatComponent::SetEquippedWeapon(AWeapon* InEquippedWeapon)
{
	if(!InEquippedWeapon) return;

	EquippedWeapon = InEquippedWeapon;
}

bool UCombatComponent::GetIsAttacking() const
{
	if(CombatState == ECombatState::ECS_Wait)
	{
		return false;
	}
	else
	{
		return true;
	}
}
