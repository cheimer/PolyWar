// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/HealthComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Controller/PolyWarPlayerController.h"
#include "GameMode/PolyWarGameModeBase.h"
#include "GameState/PolyWarGameStateBase.h"
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

	UpdateHUDHealth();
	if(IsDead())
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

	UpdateHUDHealth();
	if(CurrentHealth <= 0.0f)
	{
		Death();
	}
	else
	{
		Damaged();
	}
}

void UHealthComponent::UpdateHUDHealth()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? Cast<APolyWarPlayerController>(OwnerCharacter->GetController()) : OwnerPlayerController;
	if(OwnerPlayerController)
	{
		OwnerPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

bool UHealthComponent::IsDead()
{
	if(CurrentHealth <= 0.0f)
	{
		return true;
	}
	else
	{
		return false;
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
		DamagedPolyCharacter->SetPlayerDeath();
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
