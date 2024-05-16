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

	OrderHold();
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

// TODO: Add Client Order
void APolyWarAICharacter::OrderMove(FVector OrderPos)
{
	SetDestination(OrderPos);

	LastOrder = EOrderType::EOD_Move;
	LastOrderPos = OrderPos;
}

void APolyWarAICharacter::OrderAttack(FVector OrderPos)
{
	SetDestination(OrderPos);

	LastOrder = EOrderType::EOD_Attack;
	LastOrderPos = OrderPos;
}

void APolyWarAICharacter::OrderRush(FVector OrderPos)
{
	SetDestination(OrderPos);

	LastOrder = EOrderType::EOD_Rush;
	LastOrderPos = OrderPos;
}

void APolyWarAICharacter::OrderStop()
{
	SetDestination(GetActorLocation());

	LastOrder = EOrderType::EOD_Stop;
	LastOrderPos = GetActorLocation();
}

void APolyWarAICharacter::OrderHold()
{
	SetDestination(GetActorLocation());

	LastOrder = EOrderType::EOD_Hold;
	LastOrderPos = GetActorLocation();
}

void APolyWarAICharacter::SetDestination(FVector OrderPos)
{
	if(!HasAuthority()) return;

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
