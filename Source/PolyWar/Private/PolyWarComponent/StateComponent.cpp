// Fill out your copyright notice in the Description page of Project Settings.


#include "PolyWarComponent/StateComponent.h"

#include "Character/PolyWarBaseCharacter.h"
#include "Controller/PolyWarPlayerController.h"
#include "Net/UnrealNetwork.h"

UStateComponent::UStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStateComponent, CurrentHealth);
}

void UStateComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UStateComponent::ReceiveDamage(float Damage, AController* InstigatedBy, AActor* DamageCauser)
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

void UStateComponent::OnRep_CurrentHealth()
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

void UStateComponent::UpdateHUDHealth()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? Cast<APolyWarPlayerController>(OwnerCharacter->GetController()) : OwnerPlayerController;
	if(OwnerPlayerController)
	{
		OwnerPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

bool UStateComponent::IsDead()
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

void UStateComponent::Damaged()
{
	APolyWarBaseCharacter* DamagedPolyCharacter = Cast<APolyWarBaseCharacter>(OwnerCharacter);

	if(DamagedPolyCharacter)
	{
		DamagedPolyCharacter->PlayDamagedAnimMontage(true);
	}

}

void UStateComponent::Death()
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
void UStateComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;

	OwnerCharacter = InOwnerCharacter;
}
