// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PolyWarBaseCharacter.h"
#include "PolyWarTypes/UnitNum.h"
#include "PolyWarAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class POLYWAR_API APolyWarAICharacter : public APolyWarBaseCharacter
{
	GENERATED_BODY()

public:
	APolyWarAICharacter();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Set Should")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Set Should")
	EUnitNum UnitNum;

public:
	EUnitNum GetUnitNum() const {return UnitNum;}
	
};
