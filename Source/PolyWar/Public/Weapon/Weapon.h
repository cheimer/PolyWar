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

	void SetCollisionEnabled(bool IsEnabled);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Maybe Not Use
	// UPROPERTY(EditAnywhere, Category = "Set Should")
	// EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EWeaponSkill WeaponSkillFirst;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EWeaponSkill WeaponSkillSecond;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float WeaponDamage = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackDelay = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackRange = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float AttackAngle = 60.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UBoxComponent* AttackCollision;

	TArray<TObjectPtr<AActor>> HitActors;

public:
	float GetWeaponDamage() const {return WeaponDamage;}
	float GetAttackDelay() const {return AttackDelay;}
	float GetAttackRange() const {return AttackRange;}
	float GetAttackAngle() const {return AttackAngle;}
	EWeaponSkill GetWeaponSkillFirst() const {return WeaponSkillFirst;}
	EWeaponSkill GetWeaponSkillSecond() const {return WeaponSkillSecond;}

};
