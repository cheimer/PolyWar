// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	void Attack();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EWeaponType WeaponType;

private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USphereComponent* AttackCollision;

	bool IsAttacking = false;

public:
	void SetIsAttacking(const bool InIsAttacking) {IsAttacking = InIsAttacking;}

};
