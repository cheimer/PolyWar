// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ThrowableWeapon.h"
#include "SpearWeapon.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API ASpearWeapon : public AThrowableWeapon
{
	GENERATED_BODY()

public:
	ASpearWeapon();

	virtual void ThrowWeaponStart(const FVector& StartPos, const FVector& Direction) override;

	virtual bool SkillFocusOnScreen(EWeaponSkill WeaponSkill) override;

protected:
	virtual void PostInitializeComponents() override;

	virtual void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void WeaponSkillAttack(APolyWarBaseCharacter* Victim) override;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpearSlashKnockBackAmount;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpearThrowKnockBackAmount;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpearThrowSpeed;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float SpearThrowDuration;

private:
	void HitSpearSlash(APolyWarBaseCharacter* Victim, float Damage);
	void HitSpearThrow(APolyWarBaseCharacter* Victim, float Damage);

	void DestroyWeapon();

};
