// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "PolyWarTypes/OrderType.h"
#include "PolyWarTypes/UnitNum.h"
#include "MapButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUnitButtonClickedEvent, EUnitNum, UnitNum, class UTextBlock*, UnitText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnOrderButtonClickedEvent, EOrderType, OrderType, class UTextBlock*, OrderText);

/**
 * 
 */
UCLASS()
class POLYWAR_API UMapButton : public UButton
{
	GENERATED_BODY()

public:
	virtual void SynchronizeProperties() override;

	bool bIsUnitButton = false;
	FOnUnitButtonClickedEvent OnUnitButtonClicked;
	EUnitNum UnitNum;
	TObjectPtr<class UTextBlock> UnitText;

	bool bIsOrderButton = false;
	FOnOrderButtonClickedEvent OnOrderButtonClicked;
	EOrderType OrderType;
	TObjectPtr<class UTextBlock> OrderText;

protected:
	UFUNCTION()
	void MapButtonClicked();
	
};
