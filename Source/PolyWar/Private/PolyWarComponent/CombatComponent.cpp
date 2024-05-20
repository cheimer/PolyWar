// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/CombatComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Net/UnrealNetwork.h"
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
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::BeginWeaponAttack()
{
	if(!OwnerCharacter || CombatState != ECombatState::ECS_Wait) return;

	CombatState = ECombatState::ECS_WeaponAttack;

	SetCurrentAnimIndexRand();
	WeaponAttack(CurrentAnimIndex);

	if(!OwnerCharacter->HasAuthority())
	{
		ServerWeaponAttack(CurrentAnimIndex);
	}

}

void UCombatComponent::BeginWeaponSkill(EWeaponSkill WeaponSkill)
{
	if(!OwnerCharacter || CombatState != ECombatState::ECS_Wait) return;

	CombatState = ECombatState::ECS_WeaponSkill;
	CurrentWeaponSkill = WeaponSkill;

	WeaponSkillAttack(CurrentWeaponSkill);

	if(!OwnerCharacter->HasAuthority())
	{
		ServerWeaponSkillAttack(CurrentWeaponSkill);
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
	}
}

void UCombatComponent::WeaponAttack(int32 AnimIndex)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayAttackAnimMontage(false, AnimIndex);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::AttackEnd, EquippedWeapon->GetAttackDelay());

}

void UCombatComponent::ServerWeaponAttack_Implementation(int32 AnimIndex)
{
	MulticastWeaponAttack(AnimIndex);
}

void UCombatComponent::MulticastWeaponAttack_Implementation(int32 AnimIndex)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayAttackAnimMontage(false, AnimIndex);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::AttackEnd, EquippedWeapon->GetAttackDelay());

}

void UCombatComponent::WeaponSkillAttack(EWeaponSkill WeaponSkill)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayWeaponSkillAnimMontage(WeaponSkill);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		AttackTimer, this, &ThisClass::AttackEnd, OwnerCharacter->GetWeaponSkillAnimPlayLen(WeaponSkill));
}

void UCombatComponent::ServerWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	MulticastWeaponSkillAttack(WeaponSkill);
}

void UCombatComponent::MulticastWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayWeaponSkillAnimMontage(WeaponSkill);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(
		AttackTimer, this, &ThisClass::AttackEnd, OwnerCharacter->GetWeaponSkillAnimPlayLen(WeaponSkill));
}

void UCombatComponent::AttackEnd()
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
