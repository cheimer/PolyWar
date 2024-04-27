// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BeginAttack();

	void WeaponAttackStart();
	void WeaponAttackEnd();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;
	TObjectPtr<class AWeapon> EquippedWeapon;

	UPROPERTY(ReplicatedUsing = "OnRep_IsAttacking")
	bool bIsAttacking = false;
	UFUNCTION()
	void OnRep_IsAttacking();

	void Attack(int32 AnimIndex = 0);
	UFUNCTION(Server, Reliable)
	void ServerAttack(int32 AnimIndex = 0);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttack(int32 AnimIndex = 0);

	void AttackEnd();

	UPROPERTY(Replicated)
	int32 CurrentAnimIndex = 0;
	void SetCurrentAnimIndexRand();

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter);
	void SetEquippedWeapon(AWeapon* InEquippedWeapon);
	bool GetIsAttacking() const {return bIsAttacking;}

};
