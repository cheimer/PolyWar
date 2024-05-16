// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "PolyWarCanAttackDeco.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API UPolyWarCanAttackDeco : public UBTDecorator
{
	GENERATED_BODY()

public:
	UPolyWarCanAttackDeco();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector ClosestEnemyKey;

};
