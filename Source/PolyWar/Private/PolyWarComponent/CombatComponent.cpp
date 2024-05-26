// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/CombatComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Character/PolyWarPlayerCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::BeginWeaponAttack()
{
	if(!OwnerCharacter || !OwnerCharacter->GetEquippedWeapon() || CombatState != ECombatState::ECS_Wait) return;

	CombatState = ECombatState::ECS_WeaponAttack;

	SetCurrentAnimIndexRand();

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
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::WeaponAttackEnd, EquippedWeapon->GetAttackDelay());

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
		AttackTimer, this, &ThisClass::WeaponSkillEnd, OwnerCharacter->GetWeaponSkillAnimPlayLen(WeaponSkill));
}

void UCombatComponent::ServerWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	MulticastWeaponSkillAttack(WeaponSkill);
}

void UCombatComponent::MulticastWeaponSkillAttack_Implementation(EWeaponSkill WeaponSkill)
{
	WeaponSkillAttack(WeaponSkill);
}

void UCombatComponent::WeaponAttackEnd()
{
	CombatState = ECombatState::ECS_Wait;
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
	ThrowableWeapon->SetProjectile(true);
	ThrowableWeapon->ThrowWeaponStart(Weapon->GetActorLocation(), Direction);

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
