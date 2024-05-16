// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "PolyWarMoveToTask.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarMoveToTask : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UPolyWarMoveToTask();

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

};
