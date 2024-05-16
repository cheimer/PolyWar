// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/PolyWarDestinationService.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PolyWarAICharacter.h"

UPolyWarDestinationService::UPolyWarDestinationService()
{
	NodeName = "Update Destination";
}

void UPolyWarDestinationService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if(!Blackboard) return;

	const AAIController* AIController = OwnerComp.GetAIOwner();
	if(!AIController) return;

	APolyWarAICharacter* OwnerCharacter = Cast<APolyWarAICharacter>(AIController->GetPawn());
	if(!OwnerCharacter) return;

	AActor* ClosestEnemy = Cast<AActor>(Blackboard->GetValueAsObject("ClosestEnemy"));

	if(ClosestEnemy)
	{
		if(OwnerCharacter->GetLastOrder() == EOrderType::EOD_Attack || OwnerCharacter->GetLastOrder() == EOrderType::EOD_Stop)
		{
			Blackboard->SetValueAsVector("Destination", ClosestEnemy->GetActorLocation());

		}
		else if(OwnerCharacter->GetLastOrder() == EOrderType::EOD_Rush)
		{
			// Check: If make Enemy Block Size
			if(FVector::Distance(OwnerCharacter->GetActorLocation(), ClosestEnemy->GetActorLocation()) < 100.0f)
			{
				Blackboard->SetValueAsVector("Destination", ClosestEnemy->GetActorLocation());
			}
		}
	}

}
