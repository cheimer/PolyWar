// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/PolyWarAttackTask.h"

#include "AIController.h"
#include "Character/PolyWarAICharacter.h"
#include "Character/PolyWarBaseCharacter.h"

UPolyWarAttackTask::UPolyWarAttackTask()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UPolyWarAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const auto Controller = OwnerComp.GetAIOwner();
	if (!Controller) return EBTNodeResult::Failed;

	APolyWarAICharacter* TaskOwner = Cast<APolyWarAICharacter>(Controller->GetPawn());
	if (!TaskOwner) return EBTNodeResult::Failed;

	TaskOwner->Attack();
	TaskOwner->AttackInThisOrder(true);

	return EBTNodeResult::Succeeded;
}
