// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PolyWarTypes/WeaponSkill.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Melee,
	EWT_Range
};

UCLASS()
class POLYWAR_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	AWeapon();

	virtual void SetCollisionEnabled(bool IsEnabled);

	virtual bool SkillFocusOnScreen(EWeaponSkill WeaponSkill) {return false;}

	void WeaponSkillStart(EWeaponSkill WeaponSkill);

	float GetWeaponSkillCoolDown(EWeaponSkill WeaponSkill);
	float GetWeaponSkillRemainCoolDown(EWeaponSkill WeaponSkill);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UBoxComponent* AttackCollision;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EWeaponSkill WeaponSkillFirst;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponSkillFirstCoolDown = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponSkillFirstDamage = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EWeaponSkill WeaponSkillSecond;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponSkillSecondCoolDown = 10.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponSkillSecondDamage = 50.0f;

	EWeaponSkill CurrentWeaponSkill = EWeaponSkill::EWS_MAX;

	virtual void WeaponSkillAttack(class APolyWarBaseCharacter* Victim) {}

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponDamage = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackDelay = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackRange = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackAngle = 60.0f;

private:
	TArray<TObjectPtr<AActor>> HitActors;

	FTimerHandle SkillFirstCoolTimer;
	FTimerHandle SkillSecondCoolTimer;

	bool bWeaponSkillFirstAble = true;
	bool bWeaponSkillSecondAble = true;

	void WeaponSkillFirstReady();
	void WeaponSkillSecondReady();

public:
	float GetWeaponDamage() const {return WeaponDamage;}
	float GetAttackDelay() const {return AttackDelay;}
	float GetAttackRange() const {return AttackRange;}
	float GetAttackAngle() const {return AttackAngle;}
	EWeaponSkill GetWeaponSkillFirst() const {return WeaponSkillFirst;}
	EWeaponSkill GetWeaponSkillSecond() const {return WeaponSkillSecond;}
	void SetWeaponSkill(const EWeaponSkill WeaponSkill);
	bool GetWeaponSkillAble(EWeaponSkill WeaponSkill);

};
