// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/PolyWarClosestEnemyService.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Controller/PolyWarAIController.h"

UPolyWarClosestEnemyService::UPolyWarClosestEnemyService()
{
	NodeName = "Update ClosestEnemy";
}

void UPolyWarClosestEnemyService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if(!Blackboard) return;

	APolyWarAIController* PolyWarAIController = Cast<APolyWarAIController>(OwnerComp.GetAIOwner());
	if(!PolyWarAIController) return;

	AActor* ClosestEnemy = PolyWarAIController->GetClosestEnemy();
	Blackboard->SetValueAsObject("ClosestEnemy", ClosestEnemy);

}
