// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarAICharacter.h"

#include "Controller/PolyWarAIController.h"
#include "Perception/AIPerceptionComponent.h"

APolyWarAICharacter::APolyWarAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void APolyWarAICharacter::OrderMove(FVector OrderPos)
{
	bChaseEnemy = false;
	bCanAttack = false;
	MoveTo(OrderPos);
}

void APolyWarAICharacter::OrderAttack(FVector OrderPos)
{
	bChaseEnemy = true;
	bCanAttack = true;
	MoveTo(OrderPos);
}

void APolyWarAICharacter::OrderRush(FVector OrderPos)
{
	bChaseEnemy = false;
	bCanAttack = true;
	MoveTo(OrderPos);
}

void APolyWarAICharacter::OrderStop()
{
	bChaseEnemy = true;
	bCanAttack = true;
}

void APolyWarAICharacter::OrderHold()
{
	bChaseEnemy = false;
	bCanAttack = true;
}

void APolyWarAICharacter::MoveTo(FVector OrderPos)
{
	if(HasAuthority())
	{
		PolyWarAIController == nullptr ? Cast<APolyWarAIController>(GetController()) : PolyWarAIController;
		//PolyWarAIController->GetBlackboardComponent()

	}

}
