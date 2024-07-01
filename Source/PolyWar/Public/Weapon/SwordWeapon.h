// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/Weapon.h"
#include "SwordWeapon.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API ASwordWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	ASwordWeapon();

	virtual void BeginPlay() override;

	virtual void SetCollisionEnabled(bool IsEnabled) override;
	virtual bool SkillFocusOnScreen(EWeaponSkill WeaponSkill) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USphereComponent* SkillCollision;

	virtual void WeaponSkillAttack(APolyWarBaseCharacter* Victim) override;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SwordBlockDuration = 3.0f;

private:
	void HitSwordTakeDown(APolyWarBaseCharacter* Victim);
	void HitSwordBlock(APolyWarBaseCharacter* Victim);

};
