// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/SpearWeapon.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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
	SpearThrowSpeed = 100000.0f;
	SpearThrowDuration = 2.0f;
}

void ASpearWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeaponProjectile->InitialSpeed = SpearThrowSpeed;
	WeaponProjectile->MaxSpeed = SpearThrowSpeed;
	WeaponProjectile->Velocity = FVector::UpVector;
}

void ASpearWeapon::SetCollisionEnabled(bool IsEnabled)
{
	Super::SetCollisionEnabled(IsEnabled);

	if(IsEnabled)
	{
		AttackCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		AttackCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
		AttackCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	}
	else
	{
		AttackCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
		AttackCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
		AttackCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
}

void ASpearWeapon::OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnAttackBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	auto CollisionComponent = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
	if(CollisionComponent)
	{
		ECollisionChannel CollisionChannel = CollisionComponent->GetCollisionObjectType();
		if(CollisionChannel == ECC_WorldDynamic || CollisionChannel == ECC_WorldStatic)
		{
		}
	}
}

void ASpearWeapon::ThrowWeaponStart(const FVector& StartPos, const FVector& Direction)
{
	Super::ThrowWeaponStart(StartPos, Direction);

	FRotator Rotator = Direction.Rotation();
	FRotator RotateDirection = FRotator(Rotator.Pitch - 90.0f, Rotator.Yaw, Rotator.Roll);
	SetActorRotation(RotateDirection);
	WeaponProjectile->AddForce(Direction * SpearThrowSpeed);

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

	if(CurrentWeaponSkill == EWeaponSkill::EWS_SpearSlash)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst)
		{
			HitSpearSlash(Victim, WeaponSkillFirstDamage);
		}
		else if(CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSpearSlash(Victim, WeaponSkillSecondDamage);
		}
	}
	else if (CurrentWeaponSkill == EWeaponSkill::EWS_SpearThrow)
	{
		if(CurrentWeaponSkill == WeaponSkillFirst)
		{
			HitSpearThrow(Victim, WeaponSkillFirstDamage);
		}
		else if(CurrentWeaponSkill == WeaponSkillSecond)
		{
			HitSpearThrow(Victim, WeaponSkillSecondDamage);
		}
	}
}

void ASpearWeapon::HitSpearSlash(APolyWarBaseCharacter* Victim, float Damage)
{
	if(!GetOwner() || !GetOwner()->GetInstigatorController()) return;

	UGameplayStatics::ApplyDamage(Victim, Damage,
		GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

	FVector OwnerLoc = GetOwner()->GetActorLocation();
	FVector VictimLoc = Victim->GetActorLocation();
	FVector OwnerToVictim = VictimLoc - OwnerLoc;
	OwnerToVictim.Z = 0.0f;

	Victim->LaunchCharacter(OwnerToVictim.GetSafeNormal() * SpearSlashKnockBackAmount, true, true);
}

void ASpearWeapon::HitSpearThrow(APolyWarBaseCharacter* Victim, float Damage)
{
	if(!GetOwner() || !GetOwner()->GetInstigatorController()) return;

	UGameplayStatics::ApplyDamage(Victim, Damage,
		GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());

	FVector WeaponLoc = GetActorLocation();
	FVector VictimLoc = Victim->GetActorLocation();
	FVector WeaponToVictim = VictimLoc - WeaponLoc;
	WeaponToVictim.Z = 0.0f;

	Victim->LaunchCharacter(WeaponToVictim.GetSafeNormal() * SpearSlashKnockBackAmount, true, true);
}

bool ASpearWeapon::SkillFocusOnScreen(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == EWeaponSkill::EWS_SpearThrow)
	{
		return true;
	}

	return false;
}
