// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PolyWarBaseCharacter.h"
#include "PolyWarTypes/OrderType.h"
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
	virtual void Tick(float DeltaSeconds) override;

	void StartOrder(EOrderType Order, FVector OrderPos);

	void AttackInThisOrder(const bool bAttack) {bAttackInThisOrder = bAttack;}

	virtual void SetPlayerDeath() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Set Should")
	EUnitNum UnitNum;

private:
	TObjectPtr<class APolyWarAIController> PolyWarAIController;

	void SetDestination(FVector OrderPos);

	EOrderType LastOrder = EOrderType::EOD_Hold;
	FVector LastOrderPos = FVector();
	bool bAttackInThisOrder = false;

public:
	EUnitNum GetUnitNum() const {return UnitNum;}
	EOrderType GetLastOrder() const {return LastOrder;}
	FVector GetLastOrderPos() const {return LastOrderPos;}
	bool IsAttackInThisOrder() const {return bAttackInThisOrder;}

};
