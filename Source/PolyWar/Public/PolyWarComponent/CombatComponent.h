// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PolyWarTypes/WeaponSkill.h"
#include "CombatComponent.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Wait,
	ECS_WeaponAttack,
	ECS_WeaponSkill,
	ECS_SpellCast,

	ECS_MAX UMETA(Hidden)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BeginWeaponAttack();
	void BeginWeaponSkill(EWeaponSkill WeaponSkill);
	void BeginSpell(TSubclassOf<class ASpell> Spell);

	void WeaponAttackCheckStart();
	void WeaponAttackCheckEnd();

	void ThrowWeapon(class AWeapon* Weapon, const FVector& Direction);

	void SpellCastEnd();
	void SpellEffect();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;
	TObjectPtr<class USpellComponent> SpellComponent;
	UPROPERTY(Replicated)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(ReplicatedUsing = "OnRep_CombatState")
	ECombatState CombatState = ECombatState::ECS_Wait;
	UFUNCTION()
	void OnRep_CombatState();

	void WeaponAttack(int32 AnimIndex = 0);
	UFUNCTION(Server, Reliable)
	void ServerWeaponAttack(int32 AnimIndex = 0);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponAttack(int32 AnimIndex = 0);

	void WeaponSkillAttack(EWeaponSkill WeaponSkill);
	UFUNCTION(Server, Reliable)
	void ServerWeaponSkillAttack(EWeaponSkill WeaponSkill);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastWeaponSkillAttack(EWeaponSkill WeaponSkill);

	void SpellCast(TSubclassOf<ASpell> Spell);
	UFUNCTION(Server, Reliable)
	void ServerSpellCast(TSubclassOf<ASpell> Spell);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpellCast(TSubclassOf<ASpell> Spell);

	void WeaponAttackEnd();
	void WeaponSkillEnd();

	bool bIsAttackDelay = false;
	void AttackDelayEnd();

	UPROPERTY(Replicated)
	EWeaponSkill CurrentWeaponSkill;
	UPROPERTY(Replicated)
	TSubclassOf<ASpell> CurrentSpell;
	UPROPERTY(Replicated)
	int32 CurrentAnimIndex = 0;
	void SetCurrentAnimIndexRand();

public:
	USpellComponent* GetSpellComponent() const {return SpellComponent;}
	void SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter);
	void SetSpellComponent(USpellComponent* InSpellComponent);
	void SetEquippedWeapon(AWeapon* InEquippedWeapon);
	AWeapon* GetEquippedWeapon() const {return EquippedWeapon;}
	bool GetIsAttacking() const;

};
