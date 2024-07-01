// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SwordWeapon.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/SphereComponent.h"


ASwordWeapon::ASwordWeapon()
{
	SkillCollision = CreateDefaultSubobject<USphereComponent>("SkillCollision");
	SkillCollision->SetupAttachment(WeaponMesh);
	SkillCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkillCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkillCollision->SetIsReplicated(true);

}

void ASwordWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(SkillCollision)
	{
		SkillCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackBeginOverlap);
	}

	if(WeaponSkillFirst == EWeaponSkill::EWS_SwordBlock)
	{
		bWeaponSkillFirstLoop = true;
		WeaponSkillLoopTime = SwordBlockDuration;
	}
	if (WeaponSkillSecond == EWeaponSkill::EWS_SwordBlock)
	{
		bWeaponSkillSecondLoop = true;
		WeaponSkillLoopTime = SwordBlockDuration;
	}
}

void ASwordWeapon::SetCollisionEnabled(bool IsEnabled)
{
	Super::SetCollisionEnabled(IsEnabled);

	if(!GetOwner()) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	if(IsEnabled)
	{
		if(CurrentWeaponSkill == EWeaponSkill::EWS_SwordTakeDown)
		{
			SkillCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		}
		if(CurrentWeaponSkill == EWeaponSkill::EWS_SwordBlock)
		{
			OwnerCharacter->SetApplyDamageRate(0.0f);
		}
	}
	else
	{
		SkillCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		OwnerCharacter->SetApplyDamageRate(1.0f);
	}
}

void ASwordWeapon::WeaponSkillAttack(APolyWarBaseCharacter* Victim)
{
	//Super::WeaponSkillAttack(Victim, WeaponSkill);

	if(!GetOwner()) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	if(CurrentWeaponSkill == EWeaponSkill::EWS_SwordTakeDown)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst || CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSwordTakeDown(Victim);
		}
	}
	else if (CurrentWeaponSkill == EWeaponSkill::EWS_SwordBlock)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst || CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSwordBlock(Victim);
		}
	}
}

void ASwordWeapon::HitSwordTakeDown(APolyWarBaseCharacter* Victim)
{
	if(!GetOwner() || !GetOwner()->GetInstigatorController()) return;

	SimpleApplyDamage(Victim, EWeaponSkill::EWS_SwordTakeDown);

}

void ASwordWeapon::HitSwordBlock(APolyWarBaseCharacter* Victim)
{
	//
}

bool ASwordWeapon::SkillFocusOnScreen(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == EWeaponSkill::EWS_SwordBlock)
	{
		return true;
	}

	return false;
}
