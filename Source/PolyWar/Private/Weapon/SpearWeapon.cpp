// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SpearWeapon.h"

#include "Character/PolyWarBaseCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"

ASpearWeapon::ASpearWeapon()
{
	WeaponDamage = 50.0f;
	AttackDelay = 1.5f;
	AttackRange = 50.0f;
	AttackAngle = 60.0f;

	WeaponSkillFirst = EWeaponSkill::EWS_SpearSlash;
	WeaponSkillFirstCoolDown = 10.0f;
	WeaponSkillFirstDamage = 30.0f;

	WeaponSkillSecond = EWeaponSkill::EWS_SpearThrow;
	WeaponSkillSecondCoolDown = 10.0f;
	WeaponSkillSecondDamage = 30.0f;

	SpearSlashKnockBackAmount = 5000.0f;

	SpearThrowKnockBackAmount = 5000.0f;
	SpearThrowSpeed = 1000.0f;
	SpearThrowDuration = 2.0f;
}

void ASpearWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeaponProjectile->InitialSpeed = SpearThrowSpeed;
	WeaponProjectile->MaxSpeed = SpearThrowSpeed;

}

void ASpearWeapon::OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnAttackBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherComp)
	{
		ECollisionChannel CollisionChannel = OtherComp->GetCollisionObjectType();
		if(CollisionChannel == ECC_WorldStatic)
		{
			DestroyWeapon();
		}
	}
}

void ASpearWeapon::ThrowWeaponStart(const FVector& StartPos, const FVector& Direction)
{
	FRotator Rotator = Direction.Rotation();
	FRotator RotateDirection = FRotator(Rotator.Pitch - 90.0f, Rotator.Yaw, Rotator.Roll);
	SetActorRotation(RotateDirection);

	WeaponProjectile->Velocity = Direction * SpearThrowSpeed;
	//WeaponProjectile->AddForce(Direction * SpearThrowSpeed);

	FTimerHandle DestroyTimerHandle;
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, this, &ThisClass::DestroyWeapon, SpearThrowDuration);
}

void ASpearWeapon::DestroyWeapon()
{
	Destroy();
}

void ASpearWeapon::WeaponSkillAttack(APolyWarBaseCharacter* Victim)
{
	//Super::WeaponSkillAttack(Victim, WeaponSkill);

	if(!GetOwner()) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	if(CurrentWeaponSkill == EWeaponSkill::EWS_SpearSlash)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst || CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSpearSlash(Victim);
		}
	}
	else if (CurrentWeaponSkill == EWeaponSkill::EWS_SpearThrow)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst || CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSpearThrow(Victim);
		}
	}
}

void ASpearWeapon::HitSpearSlash(APolyWarBaseCharacter* Victim)
{
	if(!GetOwner() || !GetOwner()->GetInstigatorController()) return;

	SimpleApplyDamage(Victim, EWeaponSkill::EWS_SpearSlash);

	FVector OwnerLoc = GetOwner()->GetActorLocation();
	FVector VictimLoc = Victim->GetActorLocation();
	FVector OwnerToVictim = VictimLoc - OwnerLoc;
	OwnerToVictim.Z = 0.0f;

	Victim->LaunchCharacter(OwnerToVictim.GetSafeNormal() * SpearSlashKnockBackAmount, true, true);
}

void ASpearWeapon::HitSpearThrow(APolyWarBaseCharacter* Victim)
{
	if(!GetOwner() || !GetOwner()->GetInstigatorController()) return;

	SimpleApplyDamage(Victim, EWeaponSkill::EWS_SpearThrow);

	FVector WeaponLoc = GetActorLocation();
	FVector VictimLoc = Victim->GetActorLocation();
	FVector WeaponToVictim = VictimLoc - WeaponLoc;
	WeaponToVictim.Z = 0.0f;

	Victim->LaunchCharacter(WeaponToVictim.GetSafeNormal() * SpearThrowKnockBackAmount, true, true);
}

bool ASpearWeapon::SkillFocusOnScreen(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == EWeaponSkill::EWS_SpearThrow)
	{
		return true;
	}

	return false;
}
