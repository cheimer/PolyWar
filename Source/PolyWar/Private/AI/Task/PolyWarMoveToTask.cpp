// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/PolyWarMoveToTask.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/PolyWarAICharacter.h"
#include "Controller/PolyWarAIController.h"
#include "GameState/PolyWarGameStateBase.h"

UPolyWarMoveToTask::UPolyWarMoveToTask()
{
	NodeName = "Move To";

	//Check: AcceptableRadius = AttackRange
}

void UPolyWarMoveToTask::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	const auto Controller = Cast<APolyWarAIController>(OwnerComp.GetAIOwner());
	if(!Controller) return;

	const auto AICharacter = Cast<APolyWarAICharacter>(Controller->GetPawn());
	if(!AICharacter) return;

	if(AICharacter->IsAttackInThisOrder())
	{
		AICharacter->AttackInThisOrder(false);
		switch(AICharacter->GetLastOrder())
		{
		case EOrderType::EOD_Attack:
			AICharacter->StartOrder(EOrderType::EOD_Attack, AICharacter->GetLastOrderPos());
			break;
		case EOrderType::EOD_Rush:
			AICharacter->StartOrder(EOrderType::EOD_Rush, AICharacter->GetLastOrderPos());
			break;
		case EOrderType::EOD_Move:
		case EOrderType::EOD_Hold:
		case EOrderType::EOD_Stop:
		default:
			break;
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
