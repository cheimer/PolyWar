// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarAICharacter.h"

#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/PolyWarAIController.h"

APolyWarAICharacter::APolyWarAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APolyWarAICharacter::BeginPlay()
{
	Super::BeginPlay();

	StartOrder(EOrderType::EOD_Hold, GetActorLocation());
}

void APolyWarAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(PolyWarAIController && !GetIsAttacking() && !IsDead())
	{
		AActor* ClosestEnemy = PolyWarAIController->GetClosestEnemy();
		if(ClosestEnemy)
		{
			const FVector ToVector = ClosestEnemy->GetActorLocation();
			const FVector FromVector = GetActorLocation();
			const FRotator TargetRotator = (ToVector - FromVector).Rotation();

			SetActorRotation(FMath::RInterpTo(
				GetActorRotation(), TargetRotator, DeltaSeconds, 5.0f));
		}
	}

}

void APolyWarAICharacter::StartOrder(EOrderType Order, FVector OrderPos)
{
	if(HasAuthority())
	{
		SetDestination(OrderPos);

		LastOrder = Order;
		LastOrderPos = OrderPos;
	}
}

void APolyWarAICharacter::SetDestination(FVector OrderPos)
{
	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(!PolyWarAIController || !PolyWarAIController->GetBlackboardComponent()) return;

	PolyWarAIController->GetBlackboardComponent()->SetValueAsVector("Destination", OrderPos);
}

void APolyWarAICharacter::SetPlayerDeath()
{
	Super::SetPlayerDeath();

	PolyWarAIController = PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
	if(!PolyWarAIController || !PolyWarAIController->GetBrainComponent()) return;

	PolyWarAIController->GetBrainComponent()->StopLogic("PlayerDeath");
}
