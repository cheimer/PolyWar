// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	AttackCollision = CreateDefaultSubobject<UBoxComponent>("AttackCollision");
	AttackCollision->SetupAttachment(WeaponMesh);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAttackBeginOverlap);
	
}

void AWeapon::SetCollisionEnabled(bool IsEnabled)
{
	if(IsEnabled)
	{
		HitActors.Empty();
		AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void AWeapon::OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!GetOwner() || GetOwner() == OtherActor) return;

	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	APolyWarBaseCharacter* Victim = Cast<APolyWarBaseCharacter>(OtherActor);
	if(OwnerCharacter && Victim)
	{
		if(HitActors.Contains(Victim))// TODO: For Test || Victim->GetTeamType() == OwnerCharacter->GetTeamType())
		{
			return;
		}
		HitActors.Emplace(Victim);
		if(CurrentWeaponSkill == EWeaponSkill::EWS_MAX)
		{
			if(GetOwner()->GetInstigatorController())
			{
				UGameplayStatics::ApplyDamage(Victim, WeaponDamage,
					GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
			}
		}
		else
		{
			WeaponSkillAttack(Victim);
		}
	}

}

void AWeapon::WeaponSkillAttack(APolyWarBaseCharacter* Victim)
{
	UGameplayStatics::ApplyDamage(Victim, WeaponDamage,
		GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
}

void AWeapon::WeaponSkillStart(EWeaponSkill WeaponSkill)
{
	if(!GetOwner()) return;
	FTimerHandle SkillCoolTimer;

	if(WeaponSkill == WeaponSkillFirst)
	{
		bWeaponSkillFirstAble = false;
		GetWorldTimerManager().SetTimer(SkillCoolTimer, this, &ThisClass::WeaponSkillFirstReady, WeaponSkillFirstCoolDown);
	}
	else if(WeaponSkill == WeaponSkillSecond)
	{
		bWeaponSkillSecondAble = false;
		GetWorldTimerManager().SetTimer(SkillCoolTimer, this, &ThisClass::WeaponSkillSecondReady, WeaponSkillSecondCoolDown);
	}
}

void AWeapon::WeaponSkillFirstReady()
{
	bWeaponSkillFirstAble = true;
}

void AWeapon::WeaponSkillSecondReady()
{
	bWeaponSkillSecondAble = true;
}

/*
 * Get, Set Func
 */
void AWeapon::SetWeaponSkill(const EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == WeaponSkillFirst || WeaponSkill == WeaponSkillSecond)
	{
		CurrentWeaponSkill = WeaponSkill;
	}
	else
	{
		CurrentWeaponSkill = EWeaponSkill::EWS_MAX;
	}
}

bool AWeapon::GetWeaponSkillAble(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == WeaponSkillFirst)
	{
		return bWeaponSkillFirstAble;
	}
	if(WeaponSkill == WeaponSkillSecond)
	{
		return bWeaponSkillSecondAble;
	}

	return false;
}
