// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PolyWarBaseCharacter.generated.h"

UCLASS()
class POLYWAR_API APolyWarBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APolyWarBaseCharacter();
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	void PlayAttackAnimMontage(bool RandPlay = true, int32 Index = 0);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UHealthComponent* HealthComponent;

	//~ Begin Move
	UPROPERTY(EditAnywhere, Category = "Settable")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float RunSpeed = 900.0f;

	bool bIsRunning = false;
	//~ End Move

	//~ Begin Weapon
	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	TObjectPtr<AWeapon> EquippedWeapon;

	void Attack();

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> AttackAnimMontages;

	UFUNCTION(BlueprintCallable)
	void WeaponAttackStart();

	UFUNCTION(BlueprintCallable)
	void WeaponAttackEnd();
	//~End Weapon

private:
	void SpawnWeapon();

public:
	bool GetIsRunning() const {return bIsRunning;}
	bool GetIsAttacking() const;
	int32 GetAttackAnimMontagesLen() const {return AttackAnimMontages.Num();}

};
