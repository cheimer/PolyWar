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

	void OrderMove(FVector OrderPos);
	void OrderAttack(FVector OrderPos);
	void OrderRush(FVector OrderPos);
	void OrderStop();
	void OrderHold();

protected:
	UPROPERTY(EditAnywhere, Category = "Set Should")
	EUnitNum UnitNum;

	UPROPERTY(EditAnywhere, Category = "Settable")
	float AttackRange = 100.0f;

private:
	TObjectPtr<class APolyWarAIController> PolyWarAIController;

	bool bChaseEnemy = true;
	bool bCanAttack = true;

	void MoveTo(FVector OrderPos);

public:
	EUnitNum GetUnitNum() const {return UnitNum;}
	float GetAttackRange() const {return AttackRange;}
	bool GetIsChaseEnemy() const {return bChaseEnemy;}
	bool GetIsCanAttack() const {return bCanAttack;}

};
