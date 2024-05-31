// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PolyWarTypes/TeamType.h"
#include "PolyWarTypes/WeaponSkill.h"
#include "PolyWarBaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeathDelegate, class APolyWarBaseCharacter*, DeathCharacter);

UCLASS()
class POLYWAR_API APolyWarBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APolyWarBaseCharacter();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	FOnCharacterDeathDelegate OnCharacterDeathDelegate;

	virtual void SetPlayerDeath();

	void PlayAttackAnimMontage(bool RandPlay = true, int32 Index = 0);
	void PlayWeaponSkillAnimMontage(EWeaponSkill WeaponSkill);
	void PlaySpellAnimMontage(UAnimMontage* SpellAnimMontage);
	void PlayDamagedAnimMontage(bool RandPlay = true, int32 Index = 0);
	void PlayDeathAnimMontage(bool RandPlay = true, int32 Index = 0);

	virtual void WeaponAttack();
	virtual void WeaponSkillAttack(EWeaponSkill WeaponSkill);
	virtual void SpellAttack(TSubclassOf<class ASpell> Spell);

	bool GetViewportCenter(FVector& CenterWorldPosition, FVector& CenterWorldDirection);

protected:
	virtual void BeginPlay() override;

	bool bIsOpenMap = false;

	//~ Begin Components
	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class USpellComponent* SpellComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComponent;
	//~ End Components

	//~ Begin Move
	UPROPERTY(EditAnywhere, Category = "Settable")
	float WalkSpeed = 600.0f;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float RunSpeed = 900.0f;

	UPROPERTY(Replicated)
	bool bIsRunning = false;

	//~ End Move

	//~ Begin Weapon
	UFUNCTION(BlueprintCallable)
	void WeaponAttackCheckStart();

	UFUNCTION(BlueprintCallable)
	void WeaponAttackCheckEnd();

	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	FName RightHandSocket = "RightHandSocket";
	//~ End Weapon

	//~ Begin Health
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage,
		const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	bool IsDead();

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> DamagedAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> DeathAnimMontages;

	void DeathTimerFinished();
	//~ End Health

	UPROPERTY(EditAnywhere, Category = "Set Should")
	ETeamType TeamType;

private:
	//~ Begin Weapon
	class AWeapon* SpawnWeapon(bool IsAttach);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void ThrowWeapon();

	UFUNCTION(Server, Reliable)
	void ServerThrowWeapon(const FVector& Direction);

	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<AWeapon> EquippedWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TMap<EWeaponSkill, UAnimMontage*> WeaponSkillAnimMontages;
	//~ End Weapon

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
	void SpellEffect();


public:
	bool GetIsRunning() const {return bIsRunning;}
	bool GetIsAttacking() const;
	AWeapon* GetEquippedWeapon() const;
	float GetWeaponAttackRange() const;
	float GetWeaponAttackAngle() const;
	int32 GetAttackAnimMontagesLen() const {return AttackAnimMontages.Num();}
	float GetAttackAnimMontageLen (int32 MontageNum);
	float GetWeaponSkillAnimPlayLen(EWeaponSkill WeaponSkill) const;
	int32 GetDamagedAnimMontagesLen() const {return DamagedAnimMontages.Num();}
	int32 GetDeathAnimMontagesLen() const {return DeathAnimMontages.Num();}
	float GetCurrentHealth() const;
	float GetMaxHealth() const;
	bool GetIsOpenMap() const {return bIsOpenMap;}
	void SetIsOpenMap(bool SetIsOpenMap) {bIsOpenMap = SetIsOpenMap;}
	ETeamType GetTeamType() const {return TeamType;}
	void SetTeamType(ETeamType InTeamType) {TeamType = InTeamType;}

};
