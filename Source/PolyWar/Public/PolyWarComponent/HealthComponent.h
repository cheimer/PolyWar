// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class POLYWAR_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ReceiveDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser);

	void UpdateHUDHealth();

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
	void DeathTimerFinished();

	UFUNCTION()
	void CharacterRemoved(AActor* DestroyedActor);

public:
	void SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter);
	float GetMaxHealth() const {return MaxHealth;}
	float GetCurrentHealth() const {return CurrentHealth;}

};
