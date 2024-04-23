// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarBaseCharacter.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/Weapon.h"

APolyWarBaseCharacter::APolyWarBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);

}

void APolyWarBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnWeapon();

}

void APolyWarBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

}

void APolyWarBaseCharacter::Attack()
{

}
