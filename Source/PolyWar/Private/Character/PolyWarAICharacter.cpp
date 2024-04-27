// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PolyWarAICharacter.h"

#include "Controller/PolyWarAIController.h"

APolyWarAICharacter::APolyWarAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = APolyWarAIController::StaticClass();

}

void APolyWarAICharacter::BeginPlay()
{
	Super::BeginPlay();


}
