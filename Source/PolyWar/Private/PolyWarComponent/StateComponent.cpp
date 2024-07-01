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

	CurrentHealth = MaxHealth;

	FTimerHandle DotDamage;
	OwnerCharacter->GetWorldTimerManager().SetTimer(DotDamage, this, &ThisClass::DotDamageManage, 1.0f, true);

	FTimerHandle BufManageTimer;
	OwnerCharacter->GetWorldTimerManager().SetTimer(BufManageTimer, this, &ThisClass::BufManage, 1.0f, true);
}

void UStateComponent::ReceiveDamage(float Damage, AActor* DamageCauser)
{
	if(!OwnerCharacter || !DamageCauser || Damage <= 0.0f) return;
	if(CurrentHealth <= 0.0f) return;

	Damage = Damage * ApplyDamageRate;

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
	OwnerPlayerController = OwnerPlayerController == nullptr ?
		Cast<APolyWarPlayerController>(OwnerCharacter->GetController()) : OwnerPlayerController;

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
		//DamagedPolyCharacter->PlayDamagedAnimMontage(true);
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

// OutArray -> Index[0] per second.
// [0]: 1 Second, [1]: 2 Second ...
void UStateComponent::ArrayValuePlus(TArray<float>& OutArray, float Value, float Seconds)
{
	for(int i = 0; i < Seconds; i++)
	{
		if(OutArray.Num() > i)
		{
			OutArray[i] += Value;
		}
		else
		{
			OutArray.Add(Value);
		}
	}
}

// OutArray -> Index[0] per second.
// [0]: 1 Second, [1]: 2 Second ...
void UStateComponent::ArrayValueMultiply(TArray<float>& OutArray, float Value, float Seconds)
{
	for(int i = 0; i < Seconds; i++)
	{
		if(OutArray.Num() > i)
		{
			OutArray[i] *= Value;
		}
		else
		{
			OutArray.Add(Value);
		}
	}
}

void UStateComponent::DotDamageManage()
{
	if(!OwnerCharacter || DotDamageRemains.Num() == 0) return;

	if(OwnerCharacter->HasAuthority())
	{
		ReceiveDamage(DotDamageRemains[0], OwnerCharacter);
	}

	DotDamageRemains.RemoveAt(0);
}

void UStateComponent::BufManage()
{
	if(!OwnerCharacter || !OwnerCharacter->HasAuthority()) return;

	if(SpeedRates.Num() > 0)
	{
		SpeedChange(SpeedRates[0], OwnerCharacter);
		SpeedRates.RemoveAt(0);
	}
	else
	{
		// Speed Reset
		SpeedChange(1.0f, OwnerCharacter);
	}

	if(PowerRates.Num() > 0)
	{
		PowerRates.RemoveAt(0);
	}

	if(SpellPowerRates.Num() > 0)
	{
		SpellPowerRates.RemoveAt(0);
	}
}

void UStateComponent::SpeedChange(float Rate, AActor* ChangedActor)
{
	APolyWarBaseCharacter* ChangedCharacter = Cast<APolyWarBaseCharacter>(ChangedActor);
	if(!ChangedCharacter) return;

	ChangedCharacter->SetWalkSpeed(ChangedCharacter->GetDefaultWalkSpeed() * Rate);
	ChangedCharacter->SetRunSpeed(ChangedCharacter->GetDefaultRunSpeed() * Rate);
	ChangedCharacter->UpdateWalkSpeed();
}

void UStateComponent::DOTDamaged(float DamagePerSecond, float Seconds)
{
	ArrayValuePlus(DotDamageRemains, DamagePerSecond, Seconds);
}

void UStateComponent::Slowed(float Rate, float Seconds)
{
	if(!OwnerCharacter) return;

	ArrayValueMultiply(SpeedRates, Rate, Seconds);
	if(SpeedRates.Num() > 0)
	{
		SpeedChange(SpeedRates[0], OwnerCharacter);
	}
}

void UStateComponent::SpeedBuf(float Rate, float Seconds)
{
	if(!OwnerCharacter) return;

	ArrayValueMultiply(SpeedRates, Rate, Seconds);
	if(SpeedRates.Num() > 0)
	{
		SpeedChange(SpeedRates[0], OwnerCharacter);
	}
}

void UStateComponent::PowerBuf(float Rate, float Seconds)
{
	if(!OwnerCharacter) return;

	ArrayValueMultiply(PowerRates, Rate, Seconds);
}

void UStateComponent::SpellBuf(float Rate, float Seconds)
{
	if(!OwnerCharacter) return;

	ArrayValueMultiply(SpellPowerRates, Rate, Seconds);
}

/*
 * Get, Set Func
 */
void UStateComponent::SetOwnerCharacter(APolyWarBaseCharacter* InOwnerCharacter)
{
	if(!InOwnerCharacter) return;

	OwnerCharacter = InOwnerCharacter;
}

float UStateComponent::GetPowerRate() const
{
	if(PowerRates.Num() > 0)
	{
		return PowerRates[0];
	}
	else
	{
		return 1.0f;
	}
}

float UStateComponent::GetSpellPowerRate() const
{
	if(SpellPowerRates.Num() > 0)
	{
		return SpellPowerRates[0];
	}
	else
	{
		return 1.0f;
	}
}
