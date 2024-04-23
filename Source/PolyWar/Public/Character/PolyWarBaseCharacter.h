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
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

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

	bool bIsAttacking = false;
	//~End Weapon

private:
	void SpawnWeapon();

public:
	bool GetIsRunning() const {return bIsRunning;}

};
