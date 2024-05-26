// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ThrowableWeapon.h"

#include "GameFramework/ProjectileMovementComponent.h"

AThrowableWeapon::AThrowableWeapon()
{
	WeaponProjectile = CreateDefaultSubobject<UProjectileMovementComponent>("WeaponProjectile");
	WeaponProjectile->SetAutoActivate(false);
	WeaponProjectile->ProjectileGravityScale = 0.0f;
}

void AThrowableWeapon::SetProjectile(bool IsActivated)
{
	if(IsActivated)
	{
		WeaponProjectile->Activate();
	}
	else
	{
		WeaponProjectile->Deactivate();
	}
}
