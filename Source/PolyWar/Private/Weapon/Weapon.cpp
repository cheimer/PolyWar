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

	AttackCollision = CreateDefaultSubobject<UBoxComponent>("AttackCollision");
	AttackCollision->SetupAttachment(WeaponMesh);
	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackCollision->SetIsReplicated(true);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

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
		if(HitActors.Contains(Victim) || Victim->GetTeamType() == OwnerCharacter->GetTeamType())
		{
			return;
		}
		if(CurrentWeaponSkill == EWeaponSkill::EWS_MAX)
		{
			// Only hit 1 target
			if(HitActors.Num() > 0)
			{
				return;
			}

			HitActors.Emplace(Victim);
			if(GetOwner()->GetInstigatorController())
			{
				if(GetOwner()->HasAuthority())
				{
					SimpleApplyDamage(Victim);
				}
				else
				{
					ServerSimpleApplyDamage(Victim);
				}
			}
		}
		else
		{
			HitActors.Emplace(Victim);
			WeaponSkillAttack(Victim);
		}
	}

}

void AWeapon::SimpleApplyDamage(AActor* Victim, EWeaponSkill WeaponSkill)
{
	APolyWarBaseCharacter* OwnerCharacter = Cast<APolyWarBaseCharacter>(GetOwner());
	if(!OwnerCharacter) return;

	float DamageBasic = 0.0f;
	if(WeaponSkill == EWeaponSkill::EWS_MAX)
	{
		DamageBasic = WeaponDamage;
	}
	else if(WeaponSkill == WeaponSkillFirst)
	{
		DamageBasic = WeaponSkillFirstDamage;
	}
	else if(WeaponSkill == WeaponSkillSecond)
	{
		DamageBasic = WeaponSkillSecondDamage;
	}
	else
	{
		return;
	}

	UGameplayStatics::ApplyDamage(Victim, DamageBasic * OwnerCharacter->GetPowerRate(),
		GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
}

void AWeapon::ServerSimpleApplyDamage_Implementation(AActor* Victim, EWeaponSkill WeaponSkill)
{
	SimpleApplyDamage(Victim);
}

void AWeapon::WeaponSkillStart(EWeaponSkill WeaponSkill)
{
	if(!GetOwner()) return;

	if(WeaponSkill == WeaponSkillFirst)
	{
		bWeaponSkillFirstAble = false;
		GetWorldTimerManager().SetTimer(SkillFirstCoolTimer, this, &ThisClass::WeaponSkillFirstReady, WeaponSkillFirstCoolDown);
	}
	else if(WeaponSkill == WeaponSkillSecond)
	{
		bWeaponSkillSecondAble = false;
		GetWorldTimerManager().SetTimer(SkillSecondCoolTimer, this, &ThisClass::WeaponSkillSecondReady, WeaponSkillSecondCoolDown);
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

float AWeapon::GetWeaponSkillCoolDown(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == WeaponSkillFirst)
	{
		return WeaponSkillFirstCoolDown;
	}
	if(WeaponSkill == WeaponSkillSecond)
	{
		return WeaponSkillSecondCoolDown;
	}

	return 0.0f;
}

float AWeapon::GetWeaponSkillRemainCoolDown(EWeaponSkill WeaponSkill)
{
	if(WeaponSkill == WeaponSkillFirst)
	{
		return GetWorldTimerManager().GetTimerRemaining(SkillFirstCoolTimer);
	}
	if(WeaponSkill == WeaponSkillSecond)
	{
		return GetWorldTimerManager().GetTimerRemaining(SkillSecondCoolTimer);
	}

	return 0.0f;
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
