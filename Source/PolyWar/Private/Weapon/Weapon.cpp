// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicatingMovement(true);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
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

	APolyWarBaseCharacter* Victim = Cast<APolyWarBaseCharacter>(OtherActor);
	if(Victim)
	{
		if(HitActors.Contains(Victim))
		{
			return;
		}
		HitActors.Emplace(Victim);
		if(GetOwner()->GetInstigatorController())
		{
			UGameplayStatics::ApplyDamage(Victim, WeaponDamage,
				GetOwner()->GetInstigatorController(), GetOwner(), UDamageType::StaticClass());
		}
	}

}
