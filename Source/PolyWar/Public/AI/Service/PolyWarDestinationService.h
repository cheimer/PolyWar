// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "PolyWarDestinationService.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarDestinationService : public UBTService
{
	GENERATED_BODY()

public:
	UPolyWarDestinationService();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
