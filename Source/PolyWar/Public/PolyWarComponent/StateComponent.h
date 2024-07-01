// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StateComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API UStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStateComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ReceiveDamage(float Damage, AActor* DamageCauser);

	void UpdateHUDHealth();

	bool IsDead();

	void DOTDamaged(float DamagePerSecond, float Seconds);
	void Slowed(float Rate, float Seconds);
	void SpeedBuf(float Rate, float Seconds);
	void PowerBuf(float Rate, float Seconds);
	void SpellBuf(float Rate, float Seconds);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class APolyWarBaseCharacter* OwnerCharacter;
	TObjectPtr<class APolyWarPlayerController> OwnerPlayerController;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentHealth")
	float CurrentHealth = 0.0f;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void Damaged();
	void Death();

	void ArrayValuePlus(TArray<float>& OutArray, float Value, float Seconds);
	void ArrayValueMultiply(TArray<float>& OutArray, float Value, float Seconds);

	void DotDamageManage();
	void BufManage();

	TArray<float> DotDamageRemains;
	TArray<float> SpeedRates;
	TArray<float> PowerRates;
	TArray<float> SpellPowerRates;

	void SpeedChange(float Rate, AActor* ChangedActor);

	float ApplyDamageRate = 1.0f;

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter);
	float GetMaxHealth() const {return MaxHealth;}
	float GetCurrentHealth() const {return CurrentHealth;}
	float GetPowerRate() const;
	float GetSpellPowerRate() const;
	void SetApplyDamageRate(const float InApplyDamageRate) {ApplyDamageRate = InApplyDamageRate;}

};
