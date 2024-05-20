// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "PolyWarTypes/OrderType.h"
#include "PolyWarTypes/UnitNum.h"
#include "MapButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitButtonClickedEvent, EUnitNum, UnitNum);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOrderButtonClickedEvent, EOrderType, OrderType);

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

	bool bIsOrderButton = false;
	FOnOrderButtonClickedEvent OnOrderButtonClicked;
	EOrderType OrderType;

protected:
	UFUNCTION()
	void MapButtonClicked();
	
};
