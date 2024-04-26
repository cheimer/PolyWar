// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarBaseCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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
}

void APolyWarBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

}

void APolyWarBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APolyWarBaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("%s to %s %f damage"), *DamageCauser->GetName(), *DamagedActor->GetName(), Damage);
}

void APolyWarBaseCharacter::SpawnWeapon()
{
	if(!EquippedWeaponClass || !GetMesh() || !GetWorld()) return;

	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(EquippedWeaponClass);
	EquippedWeapon->SetOwner(this);

	FName SocketName("RightHandSocket");

	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(SocketName);
	if(HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, GetMesh());
	}

	if(CombatComponent && EquippedWeapon)
	{
		CombatComponent->SetEquippedWeapon(EquippedWeapon);
	}

}

void APolyWarBaseCharacter::Attack()
{
	if(!CombatComponent || !EquippedWeapon) return;

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
