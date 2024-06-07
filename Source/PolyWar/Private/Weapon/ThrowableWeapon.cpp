// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowableWeapon.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AThrowableWeapon::AThrowableWeapon()
{
	WeaponProjectile = CreateDefaultSubobject<UProjectileMovementComponent>("WeaponProjectile");
	WeaponProjectile->SetAutoActivate(false);
	WeaponProjectile->ProjectileGravityScale = 0.0f;
}

void AThrowableWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AThrowableWeapon::SetProjectile()
{
	WeaponProjectile->Activate();

	AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
}
