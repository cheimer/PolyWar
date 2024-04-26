// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarCharacterAnimInstance.h"

#include "Character/PolyWarBaseCharacter.h"

void UPolyWarCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PolyWarCharacter = Cast<APolyWarBaseCharacter>(TryGetPawnOwner());
}

void UPolyWarCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	PolyWarCharacter = PolyWarCharacter == nullptr ? Cast<APolyWarBaseCharacter>(TryGetPawnOwner()) : PolyWarCharacter;
	if(!PolyWarCharacter) return;

	FVector Velocity = PolyWarCharacter->GetVelocity();
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	bIsRunning = PolyWarCharacter->GetIsRunning();
	bIsAttacking = PolyWarCharacter->GetIsAttacking();

}
