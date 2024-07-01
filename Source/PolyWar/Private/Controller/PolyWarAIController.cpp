// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PolyWarAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PolyWarAICharacter.h"
#include "Perception/AIPerceptionComponent.h"

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
	PolyWarAICharacter = PolyWarAICharacter == nullptr ? Cast<APolyWarAICharacter>(GetPawn()) : PolyWarAICharacter;
	APolyWarBaseCharacter* PerceptedActor = Cast<APolyWarBaseCharacter>(Actor);
	if(!PolyWarAICharacter || !PerceptedActor) return;

	if(PolyWarAICharacter->GetTeamType() != PerceptedActor->GetTeamType())
	{
		// PerceptedActor In
		if(!InSightEnemies.Contains(PerceptedActor))
		{
			if(PerceptedActor->IsDead())
			{
				return;
			}
			InSightEnemies.Emplace(PerceptedActor);
			PerceptedActor->OnCharacterDeathDelegate.AddDynamic(this, &ThisClass::OnPerceptedActorDeath);
		}
		// PerceptedActor Out
		else
		{
			InSightEnemies.Remove(PerceptedActor);
			PerceptedActor->OnCharacterDeathDelegate.RemoveDynamic(this, &ThisClass::OnPerceptedActorDeath);
		}
	}

}

void APolyWarAIController::OnPerceptedActorDeath(APolyWarBaseCharacter* DeathCharacter)
{
	if(!DeathCharacter) return;

	if(InSightEnemies.Contains(DeathCharacter))
	{
		InSightEnemies.Remove(DeathCharacter);
		DeathCharacter->OnCharacterDeathDelegate.RemoveDynamic(this, &ThisClass::OnPerceptedActorDeath);
	}
}

bool APolyWarAIController::IsEnemyInSight()
{
	if(InSightEnemies.Num() > 0)
		return true;
	else
		return false;
}

AActor* APolyWarAIController::GetClosestEnemy()
{
	if(!GetPawn() || InSightEnemies.Num() == 0) return nullptr;

	const FVector ActorLocation = GetPawn()->GetActorLocation();
	float DistanceTwoActor = 10000.0f;
	AActor* ClosestActor = nullptr;
	for(const auto InSightEnemy : InSightEnemies)
	{
		const float NewDistance = FVector::Distance(ActorLocation, InSightEnemy->GetActorLocation());
		if(NewDistance < DistanceTwoActor)
		{
			DistanceTwoActor = NewDistance;
			ClosestActor = InSightEnemy;
		}
	}

	return ClosestActor;
}
