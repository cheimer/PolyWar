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
	void ReceiveDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser);

	void UpdateHUDHealth();

	bool IsDead();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<class APolyWarBaseCharacter> OwnerCharacter;
	TObjectPtr<class APolyWarPlayerController> OwnerPlayerController;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	float MaxHealth = 100.0f;

	UPROPERTY(ReplicatedUsing = "OnRep_CurrentHealth")
	float CurrentHealth = MaxHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	void Damaged();
	void Death();

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter);
	float GetMaxHealth() const {return MaxHealth;}
	float GetCurrentHealth() const {return CurrentHealth;}

};
