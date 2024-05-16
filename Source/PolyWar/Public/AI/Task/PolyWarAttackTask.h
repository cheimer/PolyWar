// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "PolyWarAttackTask.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarAttackTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPolyWarAttackTask();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
