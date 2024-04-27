// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/HealthComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UHealthComponent::ReceiveDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser)
{
	if(!OwnerCharacter || !DamageCauser || Damage <= 0.0f) return;

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	if(CurrentHealth <= 0.0f)
	{
		Death();
	}
	else
	{
		Damaged();
	}

}


void UHealthComponent::OnRep_CurrentHealth()
{
	if(!OwnerCharacter) return;

	if(CurrentHealth <= 0.0f)
	{
		Death();
	}
	else
	{
		Damaged();
	}
}

void UHealthComponent::Damaged()
{
	APolyWarBaseCharacter* DamagedPolyCharacter = Cast<APolyWarBaseCharacter>(OwnerCharacter);

	if(DamagedPolyCharacter)
	{
		DamagedPolyCharacter->PlayDamagedAnimMontage(true);
	}

}

void UHealthComponent::Death()
{
	APolyWarBaseCharacter* DamagedPolyCharacter = Cast<APolyWarBaseCharacter>(OwnerCharacter);

	if(DamagedPolyCharacter)
	{
		DamagedPolyCharacter->PlayDeathAnimMontage(true);

		DamagedPolyCharacter->SetPlayerDeath();
		DamagedPolyCharacter->OnDestroyed.AddDynamic(this, &ThisClass::CharacterRemoved);

		FTimerHandle DeathTimer;
		DamagedPolyCharacter->GetWorldTimerManager().SetTimer(DeathTimer, this, &ThisClass::DeathTimerFinished, 3.0f);
	}
}

void UHealthComponent::DeathTimerFinished()
{
	APolyWarBaseCharacter* DamagedPolyCharacter = Cast<APolyWarBaseCharacter>(OwnerCharacter);

	if(DamagedPolyCharacter)
	{
		DamagedPolyCharacter->Destroy();
	}
}

void UHealthComponent::CharacterRemoved(AActor* DestroyedActor)
{
	APolyWarBaseCharacter* DamagedPolyCharacter = Cast<APolyWarBaseCharacter>(OwnerCharacter);

	if(DamagedPolyCharacter && DamagedPolyCharacter->GetEquippedWeapon())
	{
		DamagedPolyCharacter->GetEquippedWeapon()->Destroy();
	}
}

/*
 * Get, Set Func
 */
void UHealthComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;

	OwnerCharacter = InOwnerCharacter;
}
