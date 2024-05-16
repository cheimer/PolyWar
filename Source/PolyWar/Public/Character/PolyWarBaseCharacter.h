// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PolyWarTypes/TeamType.h"
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
	void PlayDamagedAnimMontage(bool RandPlay = true, int32 Index = 0);
	void PlayDeathAnimMontage(bool RandPlay = true, int32 Index = 0);

	virtual void Attack();

protected:
	virtual void BeginPlay() override;

	bool bIsOpenMap = false;

	//~ Begin Components
	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UHealthComponent* HealthComponent;

	TObjectPtr<class UAIPerceptionStimuliSourceComponent> AIPerceptionSourceComponent;
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
	UPROPERTY(EditAnywhere, Category = "Settable")
	TSubclassOf<class AWeapon> EquippedWeaponClass;

	UPROPERTY(EditAnywhere, Category = "Settable")
	TArray<UAnimMontage*> AttackAnimMontages;

	UFUNCTION(BlueprintCallable)
	void WeaponAttackStart();

	UFUNCTION(BlueprintCallable)
	void WeaponAttackEnd();

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
	void SpawnWeapon();

public:
	bool GetIsRunning() const {return bIsRunning;}
	bool GetIsAttacking() const;
	AWeapon* GetEquippedWeapon() const;
	float GetWeaponAttackRange() const;
	float GetWeaponAttackAngle() const;
	int32 GetAttackAnimMontagesLen() const {return AttackAnimMontages.Num();}
	int32 GetDamagedAnimMontagesLen() const {return DamagedAnimMontages.Num();}
	int32 GetDeathAnimMontagesLen() const {return DeathAnimMontages.Num();}
	float GetCurrentHealth() const;
	float GetMaxHealth() const;
	bool GetIsOpenMap() const {return bIsOpenMap;}
	void SetIsOpenMap(bool SetIsOpenMap) {bIsOpenMap = SetIsOpenMap;}
	ETeamType GetTeamType() const {return TeamType;}
	void SetTeamType(ETeamType InTeamType) {TeamType = InTeamType;}

};
