// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PolyWarComponent/CombatComponent.h"
#include "PolyWarComponent/HealthComponent.h"
#include "Weapon/Weapon.h"

APolyWarBaseCharacter::APolyWarBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);

}

void APolyWarBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(CombatComponent)
	{
		CombatComponent->SetOwnerCharacter(this);
	}
	if(HealthComponent)
	{
		HealthComponent->SetOwnerCharacter(this);
	}
}

void APolyWarBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APolyWarBaseCharacter, bIsRunning);
}

void APolyWarBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	if(HealthComponent)
	{
		HealthComponent->UpdateHUDHealth();
	}
}

void APolyWarBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APolyWarBaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage,
                                          const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if(!HealthComponent) return;

	HealthComponent->ReceiveDamage(Damage, InstigatedBy, DamageCauser);
}

void APolyWarBaseCharacter::SetPlayerDeath()
{
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void APolyWarBaseCharacter::SpawnWeapon()
{
	if(!EquippedWeaponClass || !GetMesh() || !GetWorld() || !HasAuthority()) return;

	AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
	Weapon->SetOwner(this);

	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(RightHandSocket);
	if(HandSocket)
	{
		HandSocket->AttachActor(Weapon, GetMesh());
	}
	if(CombatComponent && Weapon)
	{
		CombatComponent->SetEquippedWeapon(Weapon);
	}

}

void APolyWarBaseCharacter::Attack()
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return;
	if(bIsOpenMap) return;

	CombatComponent->BeginAttack();
}

void APolyWarBaseCharacter::PlayAttackAnimMontage(bool RandPlay, int32 Index)
{
	if(AttackAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, AttackAnimMontages.Num() - 1);

		if(!AttackAnimMontages[RandNum]) return;
		PlayAnimMontage(AttackAnimMontages[RandNum]);
	}
	else
	{
		if(AttackAnimMontages.Num() <= Index || !AttackAnimMontages[Index]) return;
		PlayAnimMontage(AttackAnimMontages[Index]);
	}
}

void APolyWarBaseCharacter::PlayDamagedAnimMontage(bool RandPlay, int32 Index)
{
	if(DamagedAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, DamagedAnimMontages.Num() - 1);

		if(!DamagedAnimMontages[RandNum]) return;
		PlayAnimMontage(DamagedAnimMontages[RandNum]);
	}
	else
	{
		if(DamagedAnimMontages.Num() <= Index || !DamagedAnimMontages[Index]) return;
		PlayAnimMontage(DamagedAnimMontages[Index]);
	}
}

void APolyWarBaseCharacter::PlayDeathAnimMontage(bool RandPlay, int32 Index)
{
	if(DeathAnimMontages.Num() <= 0) return;

	if(RandPlay)
	{
		const int32 RandNum = FMath::RandRange(0, DeathAnimMontages.Num() - 1);

		if(!DeathAnimMontages[RandNum]) return;
		PlayAnimMontage(DeathAnimMontages[RandNum]);
	}
	else
	{
		if(DeathAnimMontages.Num() <= Index || !DeathAnimMontages[Index]) return;
		PlayAnimMontage(DeathAnimMontages[Index]);
	}

}

void APolyWarBaseCharacter::WeaponAttackStart()
{
	if(CombatComponent)
	{
		CombatComponent->WeaponAttackStart();
	}
}

void APolyWarBaseCharacter::WeaponAttackEnd()
{
	if(CombatComponent)
	{
		CombatComponent->WeaponAttackEnd();
	}
}


/*
 * Get, Set Func
 */
bool APolyWarBaseCharacter::GetIsAttacking() const
{
	if(!CombatComponent) return false;

	return CombatComponent->GetIsAttacking();
}

AWeapon* APolyWarBaseCharacter::GetEquippedWeapon() const
{
	if(!CombatComponent || !CombatComponent->GetEquippedWeapon()) return nullptr;

	return CombatComponent->GetEquippedWeapon();
}

float APolyWarBaseCharacter::GetCurrentHealth() const
{
	if(!HealthComponent) return 0.0f;

	return HealthComponent->GetCurrentHealth();
}

float APolyWarBaseCharacter::GetMaxHealth() const
{
	if(!HealthComponent) return 0.0f;

	return HealthComponent->GetMaxHealth();
}
