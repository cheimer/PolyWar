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

	DOREPLIFETIME(UCombatComponent, bIsAttacking);
	DOREPLIFETIME(UCombatComponent, CurrentAnimIndex)
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UCombatComponent::BeginAttack()
{
	if(!OwnerCharacter || bIsAttacking) return;

	bIsAttacking = true;

	SetCurrentAnimIndexRand();
	Attack(CurrentAnimIndex);

	if(!OwnerCharacter->HasAuthority())
	{
		ServerAttack(CurrentAnimIndex);
	}

}

void UCombatComponent::OnRep_IsAttacking()
{
	if(bIsAttacking)
	{
		Attack(CurrentAnimIndex);
	}
}

void UCombatComponent::Attack(int32 AnimIndex)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayAttackAnimMontage(false, AnimIndex);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::AttackEnd, EquippedWeapon->GetAttackDelay());

}

void UCombatComponent::ServerAttack_Implementation(int32 AnimIndex)
{
	MulticastAttack(AnimIndex);
}

void UCombatComponent::MulticastAttack_Implementation(int32 AnimIndex)
{
	if(!OwnerCharacter || !EquippedWeapon) return;
	OwnerCharacter->PlayAttackAnimMontage(false, AnimIndex);

	FTimerHandle AttackTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::AttackEnd, EquippedWeapon->GetAttackDelay());

}

void UCombatComponent::AttackEnd()
{
	bIsAttacking = false;
}

void UCombatComponent::WeaponAttackStart()
{
	if(!OwnerCharacter || !EquippedWeapon) return;

	EquippedWeapon->SetCollisionEnabled(true);

}

void UCombatComponent::WeaponAttackEnd()
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
