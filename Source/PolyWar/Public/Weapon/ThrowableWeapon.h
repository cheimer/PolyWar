// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "ThrowableWeapon.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API AThrowableWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AThrowableWeapon();

	virtual void ThrowWeaponStart(const FVector& StartPos, const FVector& Direction) {};

	void SetProjectile();

protected:
	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, Category = "Component")
	class UProjectileMovementComponent* WeaponProjectile;

};
