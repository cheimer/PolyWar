// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PolyWarAICharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

APolyWarAIController::APolyWarAIController()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComp"));
}

void APolyWarAIController::BeginPlay()
{
	Super::BeginPlay();

	if(BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);

		if(GetBlackboardComponent())
		{
			GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->GetBlackboardAsset());
		}
	}
}

void APolyWarAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(GetPerceptionComponent())
	{
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdated);
	}
}

void APolyWarAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	PolyWarAICharacter == nullptr ? PolyWarAICharacter = Cast<APolyWarAICharacter>(GetPawn()) : PolyWarAICharacter;
	if(!PolyWarAICharacter) return;

}
