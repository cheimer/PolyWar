// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "PolyWarClosestEnemyService.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarClosestEnemyService : public UBTService
{
	GENERATED_BODY()

public:
	UPolyWarClosestEnemyService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
